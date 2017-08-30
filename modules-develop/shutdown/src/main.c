

#include <common.h>
#include <boards/shutdown.h>
#include "shutdown.h"
#include "relays.h"
#include <messaging.h>
#include <heartbeat.h>
#include <messages.h>
#include "bms_monitoring.h"

#include <stm32f4xx_hal_gpio.h>
#include <cmsis_os.h>

int main() {
    StatusInt status;
    // Pull FAULT_IO and FAULTN_IO to low, so that it doesn't latch.

    HAL_GPIO_WritePin(
        FAULT_O_GPIO_GROUP,
        FAULT_O_GPIO_PIN,
        GPIO_PIN_SET);
    HAL_GPIO_WritePin(
        FAULTN_O_GPIO_GROUP,
        FAULTN_O_GPIO_PIN,
        GPIO_PIN_SET);

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
    PIO_Init(
        FAULT_RES_GPIO_PIN,
        FAULT_RES_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);

    // Init board and devices.
    status = InitBoard();

    if (status != STATUSINT_SUCCESS) {
        Shutdown_Fault(MSG_V_FAULT_INIT);

        return 1;
    }

    // Set Fault res high then low
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

    // Init commonsw libraries
    if ( status == STATUSINT_SUCCESS ) {
        status = Init_Heartbeat();
    }
    if ( status == STATUSINT_SUCCESS ) {
        status = init_messaging();
    }

    if ( status != STATUSINT_SUCCESS ) {
        Shutdown_Fault(MSG_V_FAULT_INIT);

        return 1;
    }

    // Begin monitoring faults
    status = Shutdown_MonitoringInit();

    if (status != STATUSINT_SUCCESS) {
        Shutdown_Fault(MSG_V_FAULT_INIT);

        return 1;
    }
    status = BMS_MonitoringInit();
	
    if (status != STATUSINT_SUCCESS) {
        Shutdown_Fault(MSG_V_FAULT_INIT);

        return 1;
    }

    // Start threads and add callbacks.
    status = Shutdown_Init();

    // Prevent board from stopping.
    while (1) { }
}



/* 
Functions:
Pull FAULT_IO low
Detect starup message from core:

If faults are set, or interlock is open, wait for reset faults message

Sequence turning on relays
(SET (open) discharge relay, SET (close) relays
1h 50ms 1l, 50ms (2h, 2l)
Wait some time (500ms configurable), SET (close) precharge relay)

Wait for message (Fault Reset) from all boards -> Drive fault reset line low
Wait for brake messages, turn on brake lights if > config value
Watch all 4 faults, and all 4 latched faults
Relay information to core board about current fault status.
Needs a red light that tells status of IMD

{
HVO, HVW, 1-3 (only using one)
HVO means 1+ relay is closed
HVW means voltage is > 60 inside the larger tractive system.

HVA (high voltage active) (combination of hvo/hvw)

Interlock signal (Rev C and D) signals when interlock circuit is closed.

Interlock open -> soft reset, wait for driver-reset fault.

Current is current going through sensor to motor
}
Sent to core

If current and brake are pressed for 500 ms then pull FAULTN_IO high.

Temperature sensors.
Check all temps, relay to core.
If temp > 60C then throw FAULT_IO

For Rev C:
BMS monitoring, send important voltages to core, send all voltages less frequently

*/
