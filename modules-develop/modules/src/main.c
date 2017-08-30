
#include "requests.h"
#include <common.h>
#include <messaging.h>
#include <heartbeat.h>
#include <fault.h>

#include <cmsis_os.h>

int main() {
    StatusInt status = STATUSINT_SUCCESS;

    // Init fault pins for fault_trigger
    init_fault_pio();

    status = InitBoard();

    if (status == STATUSINT_SUCCESS)
    {
        // CANInit and setup messaging
        status = init_messaging();
    }

    if ( status == STATUSINT_SUCCESS ) {
        // Init fault monitoring and CAN monitoring
        status = init_fault();
    }

    if (status == STATUSINT_SUCCESS)
    {
        // Init requests for potentiometer update
        status = init_requests();
    }

    if (status == STATUSINT_SUCCESS)
    {
        // Init heartbeat and start sending heartbeats
        status = Init_Heartbeat();
    }

    if ( status != STATUSINT_SUCCESS ) {
        // Send CAN message saying FAULT_INIT
        fault_trigger_nonresettable();
    }

    while(1){}
}
