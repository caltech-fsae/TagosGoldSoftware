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
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Board_LED.h"                  // ::Board Support:LED
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "RTE_Components.h"             // Component selection
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_gpio.h"

#include "../../inc/Common.h"

#define KILL_SWITCH_PIN_NUM GPIO_PIN_6 // TODO update these to the correct values based on schematics. May vary for each board.
#define KILL_SWITCH_PORT GPIOC
 
/**
 * Wire the killSwitch pin to pin C1. Board LEDs will light up if killSwitch is working.
 */
void testKillSwitch(void) {
	initPIO(KILL_SWITCH_PIN_NUM, KILL_SWITCH_PORT, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP);
	
	// pin killswitch is connected to
  initPIO(GPIO_PIN_1, GPIOC, GPIO_MODE_INPUT, GPIO_PULLDOWN);

	// LED and buttons
  initPIO(GPIO_PIN_13 | GPIO_PIN_14, GPIOD, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
  initPIO(GPIO_PIN_0, GPIOA, GPIO_MODE_INPUT, GPIO_PULLDOWN);
	
	while (1) {
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
			killSwitch();
		} else {
			reviveSwitch();
		}
    if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1)) {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);
    }
  }	
}

int main(void) {
	testKillSwitch();
}
