
#include <common.h>
#include <stm32f4xx_hal.h>

#include "shutdown.h"
#include "relays.h"

#include <heartbeat.h>
#include <messaging.h>
#include <messages.h>

#include <cmsis_os.h>

/* Local function definitions */
void Transition_Faults(uint32_t fault_signals, uint32_t external_faults);
void Enter_Faults(uint32_t fault_signals);
void Clear_Faults(uint32_t fault_signals);
void Message_Clear_Faults(void);
void Ready_To_Drive(void);
void send_fault(uint32_t fault);
/* Doesn't need fault call backs. Should continue monitoring regardless of failure state of shutdown board. */
void monitoring_thread(const void * args);

/* Callbacks */
void status_callback( uint8_t sender, uint16_t id, uint32_t val );

/* Thread definitions */

static osThreadId monitoringThread;
osThreadDef( monitoring_thread, osPriorityNormal, 1, 0 );

/* Local Variables */
uint32_t current_faults = 0;
uint32_t current_external_faults = 0;
bool r2d = false;
bool faulted = false;

/* Function implementations */
StatusInt Shutdown_MonitoringInit( void ) {
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
        AMS_I_GPIO_PIN,
        AMS_I_GPIO_GROUP,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        IMD_I_GPIO_PIN,
        IMD_I_GPIO_GROUP,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        FAULTN_LATCH_GPIO_PIN,
        FAULTN_LATCH_GPIO_GROUP,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        FAULT_LATCH_GPIO_PIN,
        FAULT_LATCH_GPIO_GROUP,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        AMS_LATCH_GPIO_PIN,
        AMS_LATCH_GPIO_GROUP,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        IMD_LATCH_GPIO_PIN,
        IMD_LATCH_GPIO_GROUP,
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);

    monitoringThread = osThreadCreate(osThread(monitoring_thread), NULL);
    if ( monitoringThread == NULL ) {
        // Check return values MB-112
    }

    status = Relays_Init();

    return status;
}

void monitoring_thread(const void * args) {
    osStatus os_status;
    GPIO_PinState state, latch_state;
    uint32_t fault_signals;
    while (1) {
        while (1) {
            fault_signals = 0;

            state = HAL_GPIO_ReadPin(
                FAULT_I_GPIO_GROUP,
                FAULT_I_GPIO_PIN);
            latch_state = HAL_GPIO_ReadPin(
                FAULT_LATCH_GPIO_GROUP,
                FAULT_LATCH_GPIO_PIN);

            if ( state == GPIO_PIN_RESET || latch_state == GPIO_PIN_RESET ) {
                fault_signals |= FAULT_SIGNAL;
            }

            state = HAL_GPIO_ReadPin(
                FAULTN_I_GPIO_GROUP,
                FAULTN_I_GPIO_PIN);
            latch_state = HAL_GPIO_ReadPin(
                FAULTN_LATCH_GPIO_GROUP,
                FAULTN_LATCH_GPIO_PIN);

            if ( state == GPIO_PIN_RESET || latch_state == GPIO_PIN_RESET ) {
                fault_signals |= FAULTN_SIGNAL;
            }

            state = HAL_GPIO_ReadPin(
                AMS_I_GPIO_GROUP,
                AMS_I_GPIO_PIN);
            latch_state = HAL_GPIO_ReadPin(
                AMS_LATCH_GPIO_GROUP,
                AMS_LATCH_GPIO_PIN);

            if ( state == GPIO_PIN_RESET || latch_state == GPIO_PIN_RESET ) {
                fault_signals |= AMS_SIGNAL;
            }

            state = HAL_GPIO_ReadPin(
                IMD_I_GPIO_GROUP,
                IMD_I_GPIO_PIN);
            latch_state = HAL_GPIO_ReadPin(
                IMD_LATCH_GPIO_GROUP,
                IMD_LATCH_GPIO_PIN);

            if ( state == GPIO_PIN_RESET || latch_state == GPIO_PIN_RESET ) {
                fault_signals |= IMD_SIGNAL;
            }

#ifndef SHUTDOWN_BOARD_B
            state = HAL_GPIO_ReadPin(
                INTERLOCK_GPIO_GROUP,
                INTERLOCK_GPIO_PIN);

            if ( state == GPIO_PIN_RESET ) {
                fault_signals |= INTERLOCK_SIGNAL;
            }
#endif
            if (fault_signals != current_faults) break;
            
            os_status = osDelay(10);
            if ( os_status != osOK ) {
                // Check return values MB-112
            }
        }

        Transition_Faults(fault_signals, current_external_faults);
    }
}

void Shutdown_ExternalFaults( uint32_t fault_signals ) {
    uint32_t external_faults = current_external_faults;
    switch (fault_signals) {
        case 0:
            external_faults = 0;
            break;
        case FAULT_SIGNAL:
            external_faults |= FAULT_SIGNAL;
            break;
        case FAULTN_SIGNAL:
            external_faults |= FAULTN_SIGNAL;
            break;
    }
    Transition_Faults(current_faults, external_faults);
}

void Transition_Faults(uint32_t fault_signals, uint32_t external_faults) {
    uint32_t signals_transition_high = ~current_faults &  fault_signals;
    uint32_t signals_transition_low  =  current_faults & ~fault_signals;

    if (signals_transition_high) {
        if (!current_faults) {
            Disable_Relays();
            faulted = true;
        }

        Enter_Faults(signals_transition_high);
    }

    if (signals_transition_low) {
        if (!fault_signals) {
            // Can't enable relays until car says go

        }

        Clear_Faults(signals_transition_low);
    }

    current_faults = fault_signals;
    current_external_faults = external_faults;
}

uint32_t Transition_BMS_Faults(uint32_t fault_signals, uint32_t current_faults) {
    uint32_t signals_transition_high = ~current_faults &  fault_signals;
    uint32_t signals_transition_low  =  current_faults & ~fault_signals;

    if (signals_transition_high) {
        if (!current_faults) {
            Disable_Relays();
            faulted = true;
        }

        Enter_Faults(signals_transition_high);
    }

    if (signals_transition_low) {
        if (!fault_signals) {
            // Can't enable relays until car says go

        }

        Clear_Faults(signals_transition_low);
    }

    return fault_signals;
}

void Enter_Faults(uint32_t fault_signals) {
    StatusInt status;
    // Shutdown all other running threads (not heartbeat though)

    // Signal Core
    if (fault_signals & FAULT_SIGNAL) {
        status = send_int(ADDRESS_BROADCAST, 0, MSG_T_FAULT, 0, MSG_V_FAULT_SWR);
        if ( status != STATUSINT_SUCCESS ) {
            // Check return values MB-112
        }
    }

    if (fault_signals & FAULTN_SIGNAL) {
        status = send_int(ADDRESS_BROADCAST, 0, MSG_T_FAULT, 0, MSG_V_FAULT_SWNR);
        if ( status != STATUSINT_SUCCESS ) {
            // Check return values MB-112
        }
    }

    if (fault_signals & INTERLOCK_SIGNAL) {
        status = send_int(ADDRESS_CORE, 0, MSG_T_INTERLOCK, 0, MSG_V_INTERLOCK_DISCONNECTED);
        if ( status != STATUSINT_SUCCESS ) {
            // Check return values MB-112
        }
    }
}

void Clear_Faults(uint32_t fault_signals) {
    StatusInt status;
    if (fault_signals & INTERLOCK_SIGNAL) {
        status = send_int(ADDRESS_CORE, 0, MSG_T_INTERLOCK, 0, MSG_V_INTERLOCK_CONNECTED);
        if ( status != STATUSINT_SUCCESS ) {
            // Check return values MB-112
        }
    }
}

StatusInt Shutdown_Init() {
    StatusInt status = STATUSINT_SUCCESS;
    if ( status == STATUSINT_SUCCESS ) {
        status = register_callback_int(MSG_T_STATUS, status_callback);
    }

    return status;
}

// Checks incoming MSG_T_STATUS messages for messages from core either
// Resetting faults or saying "ready to drive"
void status_callback( uint8_t sender, uint16_t id, uint32_t val ) {
    if (sender == ADDRESS_CORE) {
        if (val == MSG_V_STATUS_RESETD || val == MSG_V_STATUS_RESETM) {
            Message_Clear_Faults();
        }
        if (val == MSG_V_STATUS_R2D) {
            Ready_To_Drive();
        }
    }
}

void Message_Clear_Faults() {
    osStatus os_status;
    int32_t previous_signals;
    GPIO_PinState state = GPIO_PIN_RESET;
    // Make sure that we aren't in a swnr fault
    if (( current_faults & ALL_FAULTN_SIGNAL ) != 0 ||
        ( current_external_faults & FAULTN_SIGNAL ) != 0) {
        // Ignore the fault reset
        return;
    }

    // Clear fault pin
    HAL_GPIO_WritePin(
        FAULT_O_GPIO_GROUP,
        FAULT_O_GPIO_PIN,
        GPIO_PIN_RESET);

    // Wait for other boards to clear fault pin
    os_status = osDelay(500);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }

    // Clear fault latch pin
    HAL_GPIO_WritePin(
        FAULT_RES_GPIO_GROUP,
        FAULT_RES_GPIO_PIN,
        GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        FAULT_RES_GPIO_GROUP,
        FAULT_RES_GPIO_PIN,
        GPIO_PIN_SET);
    HAL_GPIO_WritePin(
        FAULT_RES_GPIO_GROUP,
        FAULT_RES_GPIO_PIN,
        GPIO_PIN_RESET);

    // Check fault pin, ignore clear if pin still set.
    state = HAL_GPIO_ReadPin(
                FAULT_I_GPIO_GROUP,
                FAULT_I_GPIO_PIN);
    if ( state == GPIO_PIN_RESET ) {
        return;
    }

    if (r2d) {
        Enable_Relays();
    }

    // Clear all fault signals
    previous_signals = osSignalClear(monitoringThread, FAULT_SIGNAL);\
    faulted = false;

    // Signal all threads with FAULT_RESET_SIGNAL
    previous_signals = osSignalSet(monitoringThread, FAULT_RESET_SIGNAL);
}

void Ready_To_Drive(void) {
    r2d = true;
    if (!faulted) {
        Enable_Relays();
    }
}

void Shutdown_Fault(uint32_t fault) {
    osStatus os_status;
    HAL_GPIO_WritePin(
        FAULTN_O_GPIO_GROUP,
        FAULTN_O_GPIO_PIN,
        GPIO_PIN_SET);

    // Fault all active threads
    //Heartbeat_Fault(FAULTN_SIGNAL);
    //Monitoring_Fault(FAULTN_SIGNAL);
    //Listeners_Fault(FAULTN_SIGNAL);
    // Broadcast fault to all other boards.
    while (1) {
        send_fault(fault);
        os_status = osDelay(10);
        if ( os_status != osOK ) {
            // Check return values MB-112
        }
    }
}

void send_fault(uint32_t fault) {
    StatusInt status;
    // Broadcast fault
    status = send_int(ADDRESS_BROADCAST, MSG_P_FAULT, MSG_T_FAULTN, MSG_I_NONE, fault);
    if ( status != STATUSINT_SUCCESS ) {
        // We're already in a fault. Maybe try send a different can message?
    }
}
