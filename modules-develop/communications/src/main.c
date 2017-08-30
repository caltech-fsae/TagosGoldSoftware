
#include <common.h>
#include <stm32f4xx_hal.h>

#include "message_callbacks_nofilter.h"
#include <messaging.h>
#include <heartbeat.h>
#include <fault.h>

#include <common.h>
#include <retarget.h>
#include <uart.h>
#include <rtc.h>

#include <cmsis_os.h>



int main() { 
    StatusInt status;

    init_fault_pio();

    status = InitBoard();

    if ( status == STATUSINT_SUCCESS ) {
        status = init_messaging();
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = init_fault();
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = Init_Heartbeat();
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = init_messages_nofilter();
    }
    
    if ( status != STATUSINT_SUCCESS ) {
        fault_trigger_nonresettable();
        // Send a message over CAN
    }

    while (1) {}
}
