/*
 * IO Board Specific Firmware
 * Caltech Racing 2016
 */

/* Standard libraries */
#ifdef DEBUG
#include <stdio.h>
#endif

/* STM libraries */
#include <stm32f4xx.h>
#include <stm32f4xx_hal_dma.h>
#include <stm32f4xx_hal_adc.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_rcc.h>

/* RTX */
#include <cmsis_os.h>

/* FSAE Firmware */
#include "common.h"
#include "boards/shutdown.h"
#include "can.h"
#include "rtc.h"
#include "i2c.h"
#ifdef DEBUG
#include "retarget.h"
#endif

/* Local types */

/* Local constants */

/* Local variables */

/* Local function prototypes */

/* Extern */
extern osMessageQId msgQ_id_IRQ;

/* 
 * Function implementations
 */

StatusInt InitDevices( void )
{
    StatusInt status = STATUSINT_SUCCESS;

    if ( status == STATUSINT_SUCCESS ) {
        //status = RTC_Init();
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = CAN_Init(CAN_CHANNEL_1);
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = CAN_Init(CAN_CHANNEL_2);
    }

#if DEBUG
    fprintf(stddebug, "Done loading\n");
#endif

    return status;
}

void CheckIRQ( void const* arg )
{
    osEvent ev;
    uint32_t msg;
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
                    // unhandled IRQ type
                    for(;;){}
            }
        }
    }
}


/*
 * Board-specific functions
 */
