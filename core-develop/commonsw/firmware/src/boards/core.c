/*
 * Core Board Specific Firmware
 * Caltech Racing 2016
 */

/* Standard libraries */

/* STM libraries */
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal.h>              // Keil::Device:STM32Cube HAL:Common

/* RTX */
#include <cmsis_os.h>

/* FSAE Firwmare */
#include "common.h"
#include "boards/core.h"
#include "motors.h"
#include "can.h"
#include "imu.h"
#include "i2c.h"
#include "display.h"
#ifdef DEBUG
#include "retarget.h"
#endif

extern osMessageQId msgQ_id_IRQ;
/* Shared variables. */

/*
 * Common function implementations
 */
StatusInt InitDevices( void )
{
    StatusInt status = STATUSINT_SUCCESS;

    if ( status == STATUSINT_SUCCESS ) {
        status = CAN_Init(CAN_CHANNEL_1);
    }

    if ( status == STATUSINT_SUCCESS ) {
        //status = Motor_Init( 100 );
    }
    
    if ( status == STATUSINT_SUCCESS ) 
    {
        status = Display_Init();
    }

#ifdef DEBUG
    fprintf(stddebug, "Done loading\n");
#endif

    return status;
}

void CheckIRQ( void const* arg )
{
    uint32_t msg;
    osEvent ev;
    while(1) {
        ev = osMessageGet(msgQ_id_IRQ, osWaitForever);
        if(ev.status == osEventMessage) {
            msg = (uint32_t)ev.value.p;
            switch(msg) {
                case IRQ_TYPE_CAN:
                    CAN_HandleMessages();
                    break;
                case IRQ_TYPE_I2C_RX:
                    I2C_HandleReceives();
                    break;
                case IRQ_TYPE_I2C_TX:
                    I2C_HandleTransmit();
                    break;
                default:
                    // unhandle IRQ type
                    for(;;){}
            }
        }
    }
}


/*
 * Board-specific functions
 */
StatusInt writeDisplay( uint32_t *displayValues, uint32_t size )
{
    return 1;
}
