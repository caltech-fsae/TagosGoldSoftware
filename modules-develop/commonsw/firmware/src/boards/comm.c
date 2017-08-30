/*
 * IO Board Specific Firmware
 * Caltech Racing 2016
 */

/* Standard libraries */
#include <stdio.h>

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
#include "boards/comm.h"
#include "can.h"
#include "uart.h"
#include "rtc.h"
#include "i2c.h"
#include "retarget.h"

/* Local types */

/* Local constants */

/* Local variables */

/* Local function prototypes */
extern StatusInt init_messages_nofilter(void);
extern osMessageQId msgQ_id_IRQ;

/* 
 * Function implementations
 */

StatusInt InitDevices( void )
{
    StatusInt status = STATUSINT_SUCCESS;

    /* Enable Wireless UART communication */
    PIO_Init(GPIO_PIN_2, GPIOB, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, PIN_NONE);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);

    /* Init both UARTs */
    if ( status == STATUSINT_SUCCESS ) {
        status = UART_Init(UART_OPENLOG, 9600);
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = UART_Init(UART_WIRELESS, 9600);
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = RTC_Init();
    }
    status = STATUSINT_SUCCESS;

    if ( status == STATUSINT_SUCCESS ) {
        status = CAN_Init(CAN_CHANNEL_1);
    }

    fprintf(stddebug, "Done loading\n");

    return status;
}

void CheckIRQ( void const* arg )
{
    uint32_t msg;
    osEvent ev;
    while(1) {
        ev = osMessageGet(msgQ_id_IRQ, osWaitForever);
        if(ev.status == osEventMessage){
            msg = (uint32_t)ev.value.p;
            switch(msg){
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
