/*----------------------------------------------------------------------------
 * Name:    Main.c
 * Purpose: Common Firmware Prototype for CS/ME/EE 75 
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2015 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"
#include "common.h"
#include "adc.h"

StatusInt InitDevices( void )
{
    StatusInt status = STATUSINT_SUCCESS;

    if ( status == STATUSINT_SUCCESS ) {
        status = ADC_Init( ADC_BRAKE, ADC_BRAKE_CHANNEL1, ADC_BRAKE_CHANNEL2, ADC_BRAKE_SAMPLINGTIME );
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = ADC_Init( ADC_THROTTLE, ADC_THROTTLE_CHANNEL1, ADC_THROTTLE_CHANNEL2, ADC_THROTTLE_SAMPLINGTIME );
    }

    if ( status == STATUSINT_SUCCESS ) {
        status = ADC_Init( ADC_STEERING, ADC_STEERING_CHANNEL1, ADC_STEERING_CHANNEL2, ADC_STEERING_SAMPLINGTIME );
    }

#ifdef DEBUG
    fprintf(stddebug, "Done loading\n");
#endif

    return status;
}

void CheckIRQ( void const* arg )
{
    
}

int main(void) {
    GPIO_InitTypeDef GPIO_InitDef;
    uint32_t brakeLevel1 = ADC_MIN;
    uint32_t brakeLevel2 = ADC_MIN;
    uint32_t throttleLevel1 = ADC_MIN;
    uint32_t throttleLevel2 = ADC_MIN;
    uint32_t steeringLevel1 = ADC_MIN;
    StatusInt status = STATUSINT_SUCCESS;
	__HAL_RCC_GPIOD_CLK_ENABLE();
 
    GPIO_InitDef.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitDef.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitDef.Pull = GPIO_NOPULL;
    GPIO_InitDef.Speed = GPIO_SPEED_FREQ_HIGH;
	
    HAL_GPIO_Init(GPIOD, &GPIO_InitDef);

	status = InitDevices();
	if (status != HAL_OK)
	{
        return status;
	}
			
		// NOTE
		// LED_GREEN      12 
		// LED_ORANGE     13 
		// LED_RED        14 
		// LED_BLUE       15 

    while (1) 
    {
		// lights up orange when 0, nothing otherwise
		// lights up red when broken or not initialzied right
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
        status = ADC_Read(&brakeLevel1, &brakeLevel2, ADC_BRAKE);
        status = ADC_Read(&throttleLevel1, &throttleLevel2, ADC_THROTTLE);
        status = ADC_Read(&steeringLevel1, &steeringLevel1, ADC_STEERING);
        if (status == HAL_OK) 
        {
            if ( brakeLevel1 <= ADC_MIN ) 
            {
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
            }
            else if ( brakeLevel1 >= ADC_MAX )
            {
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
            }
            else 
            {
                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
            }
        }
		else 
        {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
        }
	}
}
