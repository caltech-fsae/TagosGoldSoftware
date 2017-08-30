/**
 * This unit test will send CAN message through CAN1 and CAN2.
 * To check the CAN bus functionality, one should see message with
 * ID = 100, 200 sent out every second. Message received for each
 * rx FIFO will be stored in result[4]. The address filter depends
 * on the board configuration.
 *
 * Note
 * 1. To use CanDriver, need to follow the initialization
 *    osKernalInitialize()
 *    Init_ThreadIRQ
 *    osKernalStart()
 *    CANxInit()
 *    CANRxCallBackRegister()
 * 2. Note that if using osKernal, should overwrite HAL_GetTick
 * 		to use os_time instead.
*/


#ifdef __cplusplus
extern "C"
#endif

#include <stm32f4xx_hal.h>
#include <cmsis_os.h>

#include "common.h"
#include "addresses.h"
#include "can.h"
#include "bms.h"
#include <string.h>



uint8_t count = 0;
int result[4];


/* Jump here if any error occure in initialization */
void error_call(){
    for(;;){}
}

StatusInt InitDevices() {

    StatusInt status;

	status = CAN_Init(CAN_CHANNEL_BMS);
    if (status != 0){error_call();}

    return STATUSINT_SUCCESS;
}

void CheckIRQ( void const* arg )
{
    while( true )
    {
        osSignalWait( SIGNAL_CAN, osWaitForever );
        CAN_HandleMessages();
    }
}

/**
  * Main function
  */



int main (void) {

    bms_stat_t bms_stat;
    
    StatusInt status;
    
    memset(&bms_stat, 0x0,BMS_MSG_LENGTH);
    status = InitBoard();
    if (status != STATUSINT_SUCCESS){error_call();}
    status = BMS_Init();
    if (status != STATUSINT_SUCCESS){error_call();}
    
    while (1) {
        osDelay(10000);
        status = BMS_GetStatus(&bms_stat);


/*
        if ((bms_stat.state & BMS_MASTER_FAULT)!= 0){
            error_call();
        }*/
    }
}
