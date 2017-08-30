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
#include "common.h"
#include "fault.h"

/* Local types */

/* Local constants */

/* Local variables */
static FaultId *faults[NUM_PERMANENT_FAULTS];
static uint32_t current_index = 0;

static uint32_t current_signals = 0x00;
static uint32_t current_permanent_signals = 0x00;

static bool clearing_faults = false;

/* Local function prototypes */
void clear_faults( void );
void transition_faults( uint32_t fault_signals, uint32_t permanent_fault_signals );
void monitor_faults( const void * args );
void fault_update_all( void );
void fault_clear_resettable( void );
void finish_clearing_faults( const void * args );

static osTimerId fault_clear_timer;
osTimerDef( fault_clear_timer, finish_clearing_faults );

static osThreadId monitoringThread;
osThreadDef( monitor_faults, osPriorityNormal, 1, 0 );

/*
 * Function implementations
 */

StatusInt fault_init() {
    StatusInt status = STATUSINT_SUCCESS;

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

void clear_faults( void ) {
    uint32_t i;
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
        osTimerStart(fault_clear_timer, 500);
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
}

void fault_trigger_resettable() {
    HAL_GPIO_WritePin(
        FAULT_O_GPIO_GROUP,
        FAULT_O_GPIO_PIN,
        GPIO_PIN_SET);
}

void fault_trigger_nonresettable() {
    HAL_GPIO_WritePin(
        FAULTN_O_GPIO_GROUP,
        FAULTN_O_GPIO_PIN,
        GPIO_PIN_SET);
}

void fault_clear_resettable() {
    HAL_GPIO_WritePin(
        FAULT_O_GPIO_GROUP,
        FAULT_O_GPIO_PIN,
        GPIO_PIN_RESET);
}


