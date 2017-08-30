#include <stm32f4xx_hal.h>
#include "rtc.h"

#ifdef __cplusplus
extern "C"
#endif
/*
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}*/

int main(void)
{
	time_t time;
	char str[30];

	HAL_Init();
 
	if(RTC_Init() == STATUSINT_SUCCESS)
	{
		if(RTC_getTimeStr(str) == STATUSINT_SUCCESS)
		{
			if(RTC_getTimeT(&time) == STATUSINT_SUCCESS)
			{
				return 0;
			}
		}
	}
	return -1;
}
