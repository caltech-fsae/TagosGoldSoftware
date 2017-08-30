/*
 * UART Driver
 * Caltech Racing 2016
 */

/* Standard libraries */

/* STM libraries */
#ifdef LINT
    #define _RTE_
    #include <stm32f4xx.h>
#endif
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_uart.h>

/* RTX */

/* FSAE Firmware */
#include "common.h"
#include "uart.h"

/* Local types */
struct uart_s 
{
    uint16_t pins[ 2 ];
    GPIO_TypeDef* groups[ 2 ];
    uint8_t alternates;
    USART_TypeDef *instance;
    UART_HandleTypeDef handle;
};

/* Local constants */

/* Local variables */
static struct uart_s uarts[ UART_N ] = 
{
    { { USART1_TX_GPIO_PIN, USART1_RX_GPIO_PIN }, { USART1_TX_GPIO_GROUP, USART1_RX_GPIO_GROUP }, GPIO_AF7_USART1, USART1, {0} },
    { { USART2_TX_GPIO_PIN, USART2_RX_GPIO_PIN }, { USART2_TX_GPIO_GROUP, USART2_RX_GPIO_GROUP }, GPIO_AF7_USART2, USART2, {0} },
    { { USART3_TX_GPIO_PIN, USART3_RX_GPIO_PIN }, { USART3_TX_GPIO_GROUP, USART3_RX_GPIO_GROUP }, GPIO_AF7_USART3, USART3, {0} },
    { { USART4_TX_GPIO_PIN, USART4_RX_GPIO_PIN }, { USART4_TX_GPIO_GROUP, USART4_RX_GPIO_GROUP }, GPIO_AF8_UART4,  UART4,  {0} },
    { { USART5_TX_GPIO_PIN, USART5_RX_GPIO_PIN }, { USART5_TX_GPIO_GROUP, USART5_RX_GPIO_GROUP }, GPIO_AF8_UART5,  UART5,  {0} },
    { { USART6_TX_GPIO_PIN, USART6_RX_GPIO_PIN }, { USART6_TX_GPIO_GROUP, USART6_RX_GPIO_GROUP }, GPIO_AF8_USART6, USART6, {0} },
#ifdef STM32F429xx
    { { USART7_TX_GPIO_PIN, USART7_RX_GPIO_PIN }, { USART7_TX_GPIO_GROUP, USART7_RX_GPIO_GROUP }, GPIO_AF8_UART7, UART7, {0} },
    { { USART8_TX_GPIO_PIN, USART8_RX_GPIO_PIN }, { USART8_TX_GPIO_GROUP, USART8_RX_GPIO_GROUP }, GPIO_AF8_UART8, UART8, {0} },
#endif
};

/* Local function prototypes */



/* 
 * Function implementations
 */

/**
  * @brief: initialize uart channel specified by uart channel number
  * @param[in] uart_num: uart channel number to initialize. (0 ~ 7)
  * @return: StatusInt
  */
StatusInt UART_Init( uint8_t uart_num, uint32_t baud )
{
    StatusInt retVal = STATUSINT_ERROR_UNKNOWN;
    HAL_StatusTypeDef status;
    uint8_t i;
    if ( uart_num < UART_N ) {
        /* Initialize PIO */
        for ( i = 0; i < 2; i++ )
        {
            PIO_Init( uarts[ uart_num ].pins[ i ],
                      uarts[ uart_num ].groups[ i ],
                      GPIO_MODE_AF_OD,
                      GPIO_PULLUP,
                      uarts[ uart_num ].alternates );
        }

        /* Initialize Clock */
        switch ( uart_num )
        {
            case 0:
                __HAL_RCC_USART1_CLK_ENABLE();
                break;
            case 1:
                __HAL_RCC_USART2_CLK_ENABLE();
                break;
            case 2:
                __HAL_RCC_USART3_CLK_ENABLE();
                break;
            case 3:
                __HAL_RCC_UART4_CLK_ENABLE();
                break;
            case 4:
                __HAL_RCC_UART5_CLK_ENABLE();
                break;
            case 5:
                __HAL_RCC_USART6_CLK_ENABLE();
                break;
#ifdef STM32F429xx
            case 6:
                __HAL_RCC_UART7_CLK_ENABLE();
                break;
            case 7:
                __HAL_RCC_UART8_CLK_ENABLE();
                break;
#endif
        }
        uarts[ uart_num ].handle.Instance = uarts[ uart_num ].instance;
        uarts[ uart_num ].handle.Init.BaudRate = baud;
        uarts[ uart_num ].handle.Init.WordLength = UART_WORDLENGTH;
        uarts[ uart_num ].handle.Init.StopBits = UART_STOPBITS;
        uarts[ uart_num ].handle.Init.Parity = UART_PARITY;
        uarts[ uart_num ].handle.Init.Mode = UART_MODE_TX;
        uarts[ uart_num ].handle.Init.HwFlowCtl = UART_FLOWCONTROL;
        uarts[ uart_num ].handle.Init.OverSampling = UART_OVERSAMPLING_16;

        status = HAL_UART_Init( &uarts[ uart_num ].handle );
        if ( status == HAL_OK )
        {
            retVal = STATUSINT_SUCCESS;
        }
    } else {
        retVal = STATUSINT_ERROR_BADARG;
    }
    
    return retVal;
}

/**
  * @brief: writes "size" number of bytes starting from "pData" pointer
  *         to UART channel specified by "uart_num"
  * @param[in] uart_num_h: uart channel number to write to. (0 ~ 7)
  * @param[in] pData: pointer to buffer that we need to write to UART.
  * @param[in] size: number of bytes to write to
  * @return: StatusInt
  */
StatusInt UART_Write( uint8_t uart_num, uint8_t *pData, uint16_t size )
{
    StatusInt retVal = STATUSINT_ERROR_UNKNOWN;
    HAL_StatusTypeDef status;

    if ( uart_num < UART_N ) {
        status = HAL_UART_Transmit( &uarts[ uart_num ].handle, pData, size, UART_TIMEOUT );

        if ( status == HAL_OK )
        {
            retVal = STATUSINT_SUCCESS;
        }
    } else {
        retVal = STATUSINT_ERROR_BADARG;
    }

    return retVal;
}
