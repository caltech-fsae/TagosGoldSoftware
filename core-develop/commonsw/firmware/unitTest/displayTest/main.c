/**
  ******************************************************************************
  * @file    main.c
  * @author  Pierpaolo Bagnasco
  * @version V0.1
  * @date    15-July-2014
  * @brief   This file provides an example of how to use the LCD of the
  * 		 STM32F429-DISCOVERY.
  ******************************************************************************
 */
#include "common.h"
#include "tm_stm32_lcd.h"
#include "stm32f4xx_hal.h"

// need to define LCD_USE_STM32F429_EVAL
int main(void) {
    float value = .5;
    char output[50];
	
    /* Init system */
    InitBoard();
		
    /* Init LCD */
    TM_LCD_Init();
    TM_LCD_SetOrientation(1);	// Possible values are: 0, 1, 2, 3	
	
    TM_LCD_Fill(LCD_COLOR_RED);
    TM_LCD_SetFont(&TM_Font_11x18);
    TM_LCD_SetXY(10,10);
    sprintf(output, "Value is: %f", value);
    TM_LCD_Puts(output);
		
    while (1) {
    }
}
