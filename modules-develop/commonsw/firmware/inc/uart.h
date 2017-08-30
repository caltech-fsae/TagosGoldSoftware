/*
 * UART Driver
 * Caltech Racing 2016
 */
#ifndef UART_H
#define UART_H

#include "common.h" /* StatusInt */
 
#define UART_TIMEOUT        5000

StatusInt UART_Init( uint8_t uart_num, uint32_t baud );
StatusInt UART_Write( uint8_t uart_num, uint8_t *pData, uint16_t size );

#endif
