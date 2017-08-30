/* 
 * Motor Controller Unit Test
 * Caltech Racing 2016
 *
 * This unit test enables the motor, then spins the motor at two speeds
 * before disabling it.
 */

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "common.h"
#include "can.h"
#include "motors.h"

/* 
 * Common function implementations
 */ 
StatusInt InitDevices( void )
{
    StatusInt status = STATUSINT_SUCCESS;
    status = Motor_Init( 100 );

    return status;
}

extern osMessageQId msgQ_id_IRQ;
void CheckIRQ( void const* arg )
{
    osEvent ev;
    uint32_t msg;
    while(true) {
        ev = osMessageGet(msgQ_id_IRQ, osWaitForever);
        if(ev.status == osEventMessage) {
            msg = (uint32_t)ev.value.p;
            switch(msg) {
                case IRQ_TYPE_CAN:
                    CAN_HandleMessages();
                    break;
                default:
                    // unhandled IRQ type
                    break;
            }
        }
    }
}

/**
  * Main function
  */
int main (void) {                             /* Initialize the HAL Library     */
    StatusInt status = InitBoard();
    
    osDelay(1000);
    
    Motor_Enable();
    osDelay( 5000 );
    Motor_SetSpeed(3277);   // 10% speed
    osDelay( 5000 );
    Motor_SetSpeed( 9831 ); // 20% speed
    osDelay( 5000 );
    Motor_SetSpeed( 0 );    // Stop motor
    osDelay( 1000 );
    Motor_Disable();

    for(;;){}
}
