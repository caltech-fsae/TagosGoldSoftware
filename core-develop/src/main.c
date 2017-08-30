/*
 * Main of core board
 * Caltech Racing 2016
 */
#include <cmsis_os.h>

#include <common.h>
#include <messaging.h>
#include <messages.h>
#include <heartbeat.h>

#include "core.h"

int main() {
    StatusInt status = STATUSINT_SUCCESS;

    status = InitBoard();

    if ( status == STATUSINT_SUCCESS ) {
        status = init_messaging();
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = Init_Heartbeat();
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = core_Init();
    }
    if (status != STATUSINT_SUCCESS) {
        // Send CAN message saying FAULT_INIT
        send_int ( ADDRESS_SHUTDOWN, MSG_P_FAULT, MSG_T_FAULT, MSG_I_NONE, MSG_V_FAULT_INIT );
	}

    while(1) { };
}

