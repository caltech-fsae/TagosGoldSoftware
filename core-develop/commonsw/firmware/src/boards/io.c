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
#include "adc.h"
#include "boards/io.h"
#include "can.h"
#include "i2c.h"
#ifdef DEBUG
#include "retarget.h"
#endif

extern osMessageQId msgQ_id_IRQ;
/* Local types */

/* Local constants */

/* Local variables */

/* Local function prototypes */

extern void update_brake (const void* args);

/*
 * Function implementations
 */

StatusInt InitDevices( void )
{
    StatusInt status = STATUSINT_SUCCESS;

    if ( status == STATUSINT_SUCCESS ) {
        status = IO_InitBrake();
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = IO_InitThrottle();
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = IO_InitSteering();
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = CAN_Init(CAN_CHANNEL_1);
    }

#ifdef DEBUG
    fprintf(stddebug, "Done loading\n");
#endif

    return status;
}

void CheckIRQ( void const* arg )
{
    osEvent ev;
    uint32_t msg;
    while(1){
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
StatusInt IO_InitBrake( void )
{
    StatusInt retVal = STATUSINT_SUCCESS;

    retVal = ADC_Init( ADC_BRAKE, ADC_BRAKE_CHANNEL1, ADC_BRAKE_CHANNEL2, ADC_BRAKE_SAMPLINGTIME );

    return retVal;
}

StatusInt IO_InitThrottle( void )
{
    StatusInt retVal = STATUSINT_SUCCESS;

    retVal = ADC_Init( ADC_THROTTLE, ADC_THROTTLE_CHANNEL1, ADC_THROTTLE_CHANNEL2, ADC_THROTTLE_SAMPLINGTIME );

    return retVal;
}

StatusInt IO_InitSteering( void )
{
    StatusInt retVal = STATUSINT_SUCCESS;

    retVal = ADC_Init( ADC_STEERING, ADC_STEERING_CHANNEL1, ADC_STEERING_CHANNEL2, ADC_STEERING_SAMPLINGTIME );

    return retVal;
}

// This gets the value of the throttle
StatusInt IO_GetThrottle( uint32_t *value1, uint32_t *value2) 
{
    StatusInt status;

    status = ADC_Read( value1, value2, ADC_THROTTLE);

    return status;
}

// This gets the value of the brake
StatusInt IO_GetBrake( uint32_t *value1, uint32_t *value2 ) 
{
    StatusInt status;

    status = ADC_Read( value1, value2, ADC_BRAKE );

    return status;
}

// This gets the value of the steering column
StatusInt IO_GetSteering( uint32_t *value1, uint32_t *value2 ) 
{
    StatusInt status;

    status = ADC_Read( value1, value2, ADC_STEERING );

    return status;
}

/* TODO: Implement and move these functions to the appropriate board. (MB-97) */
StatusInt setBrakeLight(int lightStatus)
{
    return 1;
}

StatusInt getTSActiveLightStatus(int *lightStatus)
{
    return 1;
}

StatusInt getBrakeLightStatus(int *lightStatus)
{
    return 1;
}
