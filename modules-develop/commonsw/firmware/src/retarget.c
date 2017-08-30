/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the µVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

/* Standard libraries */
#include <stdio.h>
#include <rt_misc.h>

/* STM libraries */
#ifdef LINT
    #define _RTE_
    #include <stm32f4xx.h>
#endif
#include <stm32f4xx_hal.h>

/* RTX */
#include <cmsis_os.h>

/* FSAE Firmware */
#include "uart.h"



#pragma import(__use_no_semihosting_swi)


struct __FILE { int uartPort; };
FILE __stdout = { UART_NONE };
FILE __stdin = { UART_NONE };
FILE __stdlog = { UART_OPENLOG };
FILE __stdbeam = { UART_WIRELESS };
FILE __stddebug = { UART_DEBUG };

int fputc( int ch, FILE *f ) 
{
    int ret = ch;
    if ( f->uartPort != UART_NONE )
    {
        if ( UART_Write( f->uartPort, (uint8_t*)&ch, 1 ) != STATUSINT_SUCCESS )
        {
            ret = EOF;
        }
    }
    return ret;
}

int fputs( const char* s, FILE *f)
{
    int pos = 0;

    while ( s[pos] != '\0' )
    {
        if ( fputc( s[pos], f ) != s[pos] )
        {
            return EOF;
        }
        pos++;
    }
    return 0;
}

int fgetc(FILE *f) {
    return ('\n');
}


int ferror(FILE *f) {
    /* Your implementation of ferror */
    return EOF;
}


void _ttywrch(int c) {
    //sendchar(c);
}

void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}


