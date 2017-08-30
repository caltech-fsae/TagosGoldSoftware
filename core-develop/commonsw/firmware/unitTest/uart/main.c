#include <stm32f4xx_hal.h>
#include "uart.h"

#include "cmsis_os.h"

#include "retarget.h"
#include "common.h"

StatusInt InitDevices( void ) {
    return STATUSINT_SUCCESS;
}

void CheckIRQ( void const* arg ) {
}

int main(void)
{
    int UART_CHANNEL_NUM = 3;
    char str[30] = "hello\n";

    InitBoard();

    if (UART_Init(UART_CHANNEL_NUM - 1, 9600) == STATUSINT_SUCCESS) {
        while (1) {
            if (UART_Write(UART_CHANNEL_NUM - 1, (uint8_t*)str, 7) != STATUSINT_SUCCESS)
            {    
                break;
            }
        }
    }

    return -1;
}
