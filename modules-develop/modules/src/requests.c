#include <stdbool.h>

#include <common.h>
#include <fault.h>
#include <boards/io.h>
#include <addresses.h>
#include <messages.h>

#include <messaging.h>

#include <cmsis_os.h>

#include "requests.h"

#define THROTTLE1_BASE   0x240 /* to 0x738 */
#define THROTTLE2_BASE   0xAC8 /* to 0x570 */
#define THROTTLE1_RANGE  0x4F8
#define THROTTLE2_RANGE  0x558

#define BRAKE1_BASE   0x400 /* to 0xB40 */
#define BRAKE2_BASE   0xC40 /* to 0x4C0 */
#define BRAKE1_RANGE  0x740
#define BRAKE2_RANGE  0x780

#define DELAY (1000/100 - 1)

static osThreadId updateGasBrakeThread;

/* Local function prototypes */
static void update_gas (void);
static void update_brake (void);
static void update_wheel (void);
static void update_gas_brake ( void const * );


osThreadDef( update_gas_brake, osPriorityNormal, 1, 0 );

bool currently_updating = false;

// Make some threads
// Send Brake, Wheel, and Gas values over CAN.
StatusInt init_requests(void) {
    StatusInt status = STATUSINT_SUCCESS;

    if (status == STATUSINT_SUCCESS) {
        status = IO_InitBrake();
    }
    if (status == STATUSINT_SUCCESS) {
        status = IO_InitThrottle();
    }

    if (status == STATUSINT_SUCCESS) {
        currently_updating = true;
        updateGasBrakeThread = osThreadCreate( osThread( update_gas_brake ), NULL );
        if ( updateGasBrakeThread == NULL ) {
            status = STATUSINT_ERROR_GENERAL;
            currently_updating = false;
        }
    }

    return status;
}


void update_brake(void const * args) {
    uint32_t current1, current2; /* L-H, H-L */
    float adjusted_current1, adjusted_current2, diff, average;
    StatusInt status = IO_GetBrake(&current1, &current2);
    
    if (status == STATUSINT_SUCCESS) {
        // Take values, convert to floats
        // Convert onto 0-1
        adjusted_current1 = ((float)current1 - BRAKE1_BASE) / BRAKE1_RANGE;
        adjusted_current2 = (BRAKE2_BASE - (float)current2) / BRAKE2_RANGE;
        // Compare diff to range (10%)
        diff = adjusted_current1 - adjusted_current2;
        if (diff < 0) { diff = -diff; }
        
        if (diff > 0.1f) {
            // Fault because of pedal mismatch
            fault_trigger_resettable();
        } else {
            // Take average and send
            average = (adjusted_current1 + adjusted_current2) / 2;
            // Scale to be between 0.05 and 0.95
            average = average * 0.9f + 0.05f;
            status = send_int(ADDRESS_CORE, 0, MSG_T_BRAKE, 0, current1);
            if (status != STATUSINT_SUCCESS) {
                fault_trigger_resettable();
            }
        }
    } else if ( status == STATUSINT_ERROR_UNKNOWN || status == STATUSINT_ERROR_GENERAL ) {
        fault_trigger_resettable();
    } else if ( status == STATUSINT_ERROR_BADADDR ) {
        // Bug in my code.
    }
}

void update_gas(void const * args) {
    uint32_t current1, current2; /* L-H, H-L */
    float adjusted_current1, adjusted_current2, diff, average;
    StatusInt status = IO_GetThrottle(&current1, &current2);
    
    if (status == STATUSINT_SUCCESS) {
        // Take values, convert to floats
        // Convert onto 0-1
        adjusted_current1 = ((float)current1 - THROTTLE1_BASE) / THROTTLE1_RANGE;
        adjusted_current2 = (THROTTLE2_BASE - (float)current2) / THROTTLE2_RANGE;
        // Compare diff to range (10%)
        diff = adjusted_current1 - adjusted_current2;
        if (diff < 0) { diff = -diff; }
        
        if (diff > 0.1f) {
            // Fault because of pedal mismatch
            fault_trigger_resettable();
        } else {
            // Take average and send
            average = (adjusted_current1 + adjusted_current2) / 2;
            // Scale to be between 0.05 and 0.95
            average = average * 0.9f + 0.05f;
            status = send_int(ADDRESS_CORE, 0, MSG_T_THROTTLE, 0, (int)(average * PEDAL_MAX_VALUE));
            if (status != STATUSINT_SUCCESS) {
                fault_trigger_resettable();
            }
        }
    } else if (status == STATUSINT_ERROR_UNKNOWN) {
        fault_trigger_resettable();
    } else if (status == STATUSINT_ERROR_BADADDR) {
        // Bug in my code.
    }
}

void update_gas_brake(void const * args) {
    while (currently_updating) {
        update_gas();
        update_brake();
        update_wheel();
        osDelay(DELAY);
    }
}

void update_wheel(void) {
    while (currently_updating) {
        uint32_t current1, current2;
        StatusInt status = IO_GetSteering(&current1, &current2);
        if (status == STATUSINT_SUCCESS) {
            status = send_int(ADDRESS_CORE, 0, MSG_T_STEER, 0, current1);
            if (status != STATUSINT_SUCCESS) {
                fault_trigger_resettable();
            }
        } if (status == STATUSINT_ERROR_UNKNOWN) {
            fault_trigger_resettable();
        } else if (status == STATUSINT_ERROR_BADADDR) {
            // Bug in my code.
        }

        osDelay(DELAY);
    }
}
