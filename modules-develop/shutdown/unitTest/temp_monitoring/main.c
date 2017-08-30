#include <stm32f4xx_hal.h>
#include <cmsis_os.h>
#include <common.h>
#include "temp_monitoring.h"
#include <can.h>


StatusInt InitDevices( void ) {
    StatusInt status;
    status = CAN_Init(CAN_CHANNEL_1);
    return status;
}

void CheckIRQ( void const* arg ) {
    while (true) { 
        osSignalWait( SIGNAL_CAN, osWaitForever );
        CAN_HandleMessages();
    }
}

int main(void)
{
    StatusInt status;
    status = InitBoard();
    if ( status == STATUSINT_SUCCESS ) {   
        Init_TempMonitor();
    }
	while (1){}
}
