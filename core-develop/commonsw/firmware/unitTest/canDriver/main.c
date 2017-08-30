
/**
 * This unit test will send CAN message through CAN1 and CAN2. 
 * To check the CAN bus functionality, one should see message with 
 * ID = 0x100, 0x200 on CAN1 and CAN2, respectively, sent out every second.
 * Message received for each rx FIFO will be stored in result[4]. The 
 * address filter depends on the board configuration. 
 * 
*/


#ifdef __cplusplus
extern "C"
#endif

#include "stm32f4xx_hal.h"              
#include "cmsis_os.h"                   

#include "common.h"
#include "can.h"
#include "addresses.h"

#include <string.h>
// assume coreboard for can1




int rxflag = 0;
int rxflag1 = 0;
uint8_t count = 0;
int result[4];
void can_callbacktest(CANRxStruct_t* rx)
{
    switch (((rx->ID) & (CAN_ADDRESS_MASK)) << CAN_ADDRESS_SHIFT){
        case ADDRESS_CORE:
            result[0] +=1;
            break;
        case ADDRESS_BROADCAST:
            result[1] +=1;
            break;
        default:
            break;
    }
        
}

/* Jump here if any error occure in initialization */
void error_call(){
    for(;;){}
}

void CheckIRQ(const void* arg){
    while(1){
        osSignalWait(SIGNAL_CAN, osWaitForever);
        CAN_HandleMessages();
    }
}

StatusInt InitDevices(){
    StatusInt status;
    status = CAN_Init(CAN_CHANNEL_1);
    if (status != 0){error_call();}
#if (CAN2_ENABLE == 1)
	status = CAN_Init(CAN_CHANNEL_2);
    if (status != 0){error_call();}
#endif
    CAN_RegisterCallback(CAN_CHANNEL_1, &(can_callbacktest));
	CAN_RegisterCallback(CAN_CHANNEL_2, &(can_callbacktest));
    return status;
}

/**
  * Main function
  */
int main (void) {
    CANTxStruct_t tx_info;

    InitBoard();

    memset((void*)result,0x0,4*sizeof(int));
    tx_info.length = 8;
    tx_info.data[0] = 'C';


    while (1) {
        // To test CAN2 without CAN1, do not send message through CAN, code will report error in CAN1
        osDelay(500);
        tx_info.data[1] = count;
        tx_info.ID=0x100;
        CAN_SendMessage(&tx_info,CAN_CHANNEL_1);
#if (CAN2_ENABLE == 1)
        tx_info.ID=0x200;
        osDelay(500);
        CAN_SendMessage(&tx_info,CAN_CHANNEL_2);
#endif
        count++;
    }
}
