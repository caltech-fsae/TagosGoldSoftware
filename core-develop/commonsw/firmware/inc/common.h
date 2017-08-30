/*
 * Common Firmware Components
 * Caltech Racing 2016
 */
#ifndef COMMON_H
#define COMMON_H

/* General includes */
#include <stdbool.h>
#include <stdint.h>
#include "macros.h"
#include "addresses.h"
#include "config.h"

/*
 * Status
 */
typedef uint32_t StatusInt;

#define STATUSINT_SUCCESS           ((StatusInt) 0)
#define STATUSINT_ERROR_GENERAL     ((StatusInt) -1)
#define STATUSINT_ERROR_BADARG      ((StatusInt) -2)
#define STATUSINT_ERROR_BADADDR     ((StatusInt) -3)
#define STATUSINT_ERROR_UNKNOWN     ((StatusInt) -4)
#define STATUSINT_BMS_BUSY          ((StatusInt) -5)
#define STATUSINT_BMS_INIT_FAIL     ((StatusInt) -6)

/*
 * Signals
 */
enum signals 
{
    SIGNAL_IRQ = 0x1 << 0,
    SIGNAL_FAULT = 0x1 << 1,
    SIGNAL_CLEAR_FAULT = 0x1 << 2,
    SIGNAL_I2C_1 = 0x1 << 3,
    SIGNAL_I2C_2 = 0x1 << 4,
    SIGNAL_I2C_3 = 0x1 << 5,
};

enum irq_types
{
    IRQ_TYPE_CAN = 0,
    IRQ_TYPE_I2C_RX,
    IRQ_TYPE_I2C_TX,
    IRQ_TYPE_I2C_ER,
};

#define SIGNAL_SET_FAIL INT32_MIN // 0x80000000
#define STARTUP_TIME 2000000
#define MUTEX_TIMEOUT 50

/*
 * Common functions and their definitions
 */

StatusInt InitBoard( void );
extern StatusInt InitDevices( void );
extern void CheckIRQ( void const* arg );

#define GPIO_SET_AF_NONE 0

/**********************************************************************
 *                  Common Firmware                                   *
 **********************************************************************/
 /**
  * Initializes the given pin(s) in a port. See stm32f4xx_hal_gpio.h for 
    * possible values of the arguments.
    */
void PIO_Init( uint32_t pin, GPIO_TypeDef * port, uint32_t in_out, uint32_t pull, uint32_t alternate );

#endif /* COMMON_H */
