/*
 * Fault management code
 * Caltech Racing 2016
 */

/* Standard libraries */

/* STM libraries */
#ifdef LINT
    #define _RTE_
    #include <stm32f4xx.h>
#endif
#include <stm32f4xx_hal.h>
#include <cmsis_os.h>

/* RTX */

/* FSAE Firmware */
#include <common.h>
#include "messaging.h"
#include "messages.h"
#include "fault.h"

#ifdef SHUTDOWN_BOARD
extern void Shutdown_ExternalFaults( uint32_t signals );
#endif

/* Local types */

/* Local constants */

/* Local variables */
static FaultId *faults[NUM_PERMANENT_FAULTS];
static uint32_t current_index = 0;

static uint32_t current_signals = 0x00;
static uint32_t current_permanent_signals = 0x00;

static bool clearing_faults = false;

/* Local function prototypes */
/* clear_faults is used in core to deal with the lack of loopback. */
void clear_faults( void );
static void transition_faults( uint32_t fault_signals, uint32_t permanent_fault_signals );
static void monitor_faults( const void * args );
static void fault_update_all( void );
static void fault_clear_resettable( void );
static void finish_clearing_faults( const void * args );

static void status_callback( uint8_t sender, uint16_t key, uint32_t value );

static osTimerId fault_clear_timer;
osTimerDef( fault_clear_timer, finish_clearing_faults );

static osThreadId monitoringThread;
osThreadDef( monitor_faults, osPriorityNormal, 1, 0 );

/*
 * Function implementations
 */

void init_fault_pio() {
#ifndef SHUTDOWN_BOARD
#ifndef SINGLE_FAULT_PIN
    PIO_Init(
        FAULT_I_GPIO_PIN,
        FAULT_I_GPIO_GROUP,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        FAULTN_I_GPIO_PIN,
        FAULTN_I_GPIO_GROUP,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
#endif
#ifdef SINGLE_FAULT_PIN
#else
    PIO_Init(
        FAULT_O_GPIO_PIN,
        FAULT_O_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        FAULTN_O_GPIO_PIN,
        FAULTN_O_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
#endif

#ifdef SINGLE_FAULT_PIN
#else
    HAL_GPIO_WritePin(
        FAULT_O_GPIO_GROUP,
        FAULT_O_GPIO_PIN,
        GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        FAULTN_O_GPIO_GROUP,
        FAULTN_O_GPIO_PIN,
        GPIO_PIN_RESET);
#endif
#endif
}

StatusInt init_fault() {
    StatusInt status = STATUSINT_SUCCESS;

    if ( status == STATUSINT_SUCCESS ) {
        status = register_callback_int( MSG_T_STATUS, status_callback );
    }

    if ( status == STATUSINT_SUCCESS ) {
        monitoringThread = osThreadCreate(osThread(monitor_faults), NULL);
        if ( monitoringThread == NULL ) {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    if ( status == STATUSINT_SUCCESS ) {
        fault_clear_timer = osTimerCreate( osTimer(fault_clear_timer), osTimerOnce, (void *) NULL );
    }

    return status;
}

StatusInt fault_add( FaultId *fault_id, bool permanent, bool resettable ) {
    if ( current_index == NUM_PERMANENT_FAULTS ) {
        return STATUSINT_ERROR_GENERAL;
    }

    fault_id->index = current_index;
    faults[current_index] = fault_id;
    fault_id->triggered = false;
    fault_id->permanent = permanent;
    fault_id->resettable = resettable;
    current_index = current_index + 1;
    return STATUSINT_SUCCESS;
}

void fault_trigger( FaultId *fault_id ) {
    fault_id->triggered = true;

    fault_update_all();
}

void fault_clear(FaultId *fault_id ) {
    faults[fault_id->index]->triggered = false;

    fault_update_all();
}

void fault_clear_all() {
    uint16_t index;
    for (index = 0; index < NUM_PERMANENT_FAULTS; index++) {
        if (faults[index]->permanent == false) {
            faults[index]->triggered = false;
        }
    }

    fault_update_all();
}

void fault_update_all() {
    uint32_t fault_permanent_signals = 0x0;
    uint16_t index;

    for (index = 0; index < NUM_PERMANENT_FAULTS; index++) {
        if (faults[index]->triggered == true) {
            if (faults[index]->resettable == true) {
                fault_permanent_signals |= FAULT_SIGNAL;
            } else {
                fault_permanent_signals |= FAULTN_SIGNAL;
            }
        }
    }

    if ( fault_permanent_signals != current_permanent_signals ) {
        transition_faults(current_signals, fault_permanent_signals);
    }
}

void status_callback( uint8_t sender, uint16_t key, uint32_t value ) {
    if (sender == ADDRESS_CORE) {
        if ( value == MSG_V_STATUS_RESETM || value == MSG_V_STATUS_RESETD ) {
            clear_faults();
        }
    }
}

void clear_faults( void ) {
    uint32_t i;
    osStatus os_status;
    bool should_clear_fault = true;
    for (i = 0; i < NUM_PERMANENT_FAULTS; i++) {
        if ( faults[i]->triggered && faults[i]->permanent ) {
            should_clear_fault = false;
        }
    }

    if (current_signals & FAULTN_SIGNAL) {
        should_clear_fault = false;
    }

    if (should_clear_fault) {
        clearing_faults = true;
        fault_clear_all();
        fault_clear_resettable();
        os_status = osTimerStart(fault_clear_timer, 500);
        if ( os_status != osOK ) {
            fault_trigger_nonresettable();
        }
    }
}

void finish_clearing_faults( const void * args ) {
    clearing_faults = false;
}

void transition_faults(uint32_t fault_signals, uint32_t fault_permanent_signals) {
    uint32_t signals = fault_signals | fault_permanent_signals;
    uint32_t current = current_signals | current_permanent_signals;
    uint32_t signals_transition_high = ~current &  signals;

    if (signals_transition_high) {
        if (signals_transition_high & FAULT_SIGNAL) {
            fault_trigger_resettable();
        }
        if (signals_transition_high & FAULTN_SIGNAL) {
            fault_trigger_nonresettable();
        }
    }

    current_signals = fault_signals;
    current_permanent_signals = fault_permanent_signals;
}

void monitor_faults(const void * args) {
#ifndef SINGLE_FAULT_PIN
    osStatus os_status;
    GPIO_PinState state;
    uint32_t fault_signals;
    while (1) {
        while (1) {
            fault_signals = 0;

            state = HAL_GPIO_ReadPin(
                FAULT_I_GPIO_GROUP,
                FAULT_I_GPIO_PIN);

            if ( state == GPIO_PIN_RESET ) {
                fault_signals |= FAULT_SIGNAL;
            }

            state = HAL_GPIO_ReadPin(
                FAULTN_I_GPIO_GROUP,
                FAULTN_I_GPIO_PIN);

            if ( state == GPIO_PIN_RESET ) {
                fault_signals |= FAULTN_SIGNAL;
            }

            if ( (fault_signals != current_signals) && (clearing_faults == false) ) break;

            os_status = osDelay(10);
            if ( os_status != osOK ) {
                // Check return values MB-112
            }
        }

        transition_faults(fault_signals, current_permanent_signals);
    }
#endif
}

void fault_trigger_resettable() {
#ifdef SHUTDOWN_BOARD
    // Tell shutdown monitoring code
    Shutdown_ExternalFaults( FAULT_SIGNAL );
#else
#ifdef SINGLE_FAULT_PIN
    HAL_GPIO_WritePin(
        FAULT_O_GPIO_GROUP,
        FAULT_O_GPIO_PIN,
        GPIO_PIN_RESET);
#else
    HAL_GPIO_WritePin(
        FAULT_O_GPIO_GROUP,
        FAULT_O_GPIO_PIN,
        GPIO_PIN_SET);
#endif
#endif
}

void fault_trigger_nonresettable() {
#ifdef SHUTDOWN_BOARD
    // Tell shutdown monitoring code
    Shutdown_ExternalFaults( FAULTN_SIGNAL );
#else
#ifdef SINGLE_FAULT_PIN
    HAL_GPIO_WritePin(
        FAULTN_O_GPIO_GROUP,
        FAULTN_O_GPIO_PIN,
        GPIO_PIN_RESET);
#else
    HAL_GPIO_WritePin(
        FAULTN_O_GPIO_GROUP,
        FAULTN_O_GPIO_PIN,
        GPIO_PIN_SET);
#endif
#endif
}

void fault_clear_resettable() {
#ifdef SHUTDOWN_BOARD
    // Tell shutdown monitoring code
    Shutdown_ExternalFaults( 0 );
#else
#ifdef SINGLE_FAULT_PIN
    HAL_GPIO_WritePin(
        FAULT_O_GPIO_GROUP,
        FAULT_O_GPIO_PIN,
        GPIO_PIN_SET);
#else
    HAL_GPIO_WritePin(
        FAULT_O_GPIO_GROUP,
        FAULT_O_GPIO_PIN,
        GPIO_PIN_RESET);
#endif
#endif
}


