/*
 * Default (Common) Configuration
 * Caltech Racing 2016
 */
#ifndef CONFIG_H
#define CONFIG_H

#ifdef LINT
    #define _RTE_
#endif

#include <stm32f4xx.h>

#include "macros.h"
#include "addresses.h"

/* Grab the correct configuration file */
#if   defined(LINT)
#include "config_board_defaults.h"
#elif defined(CORE_BOARD_A)
#include "config_core_a.h"
#elif defined(CORE_BOARD_B)
#include "config_core_b.h"
#elif defined(CORE_BOARD_C)
#include "config_core_c.h"
#elif defined(IO_BOARD_A)
#include "config_io_a.h"
#elif defined(IO_BOARD_B)
#include "config_io_b.h"
#elif defined(IO_BOARD_C)
#include "config_io_c.h"
#elif defined(COMM_BOARD_A)
#include "config_comm_a.h"
#elif defined(COMM_BOARD_B)
#include "config_comm_b.h"
#elif defined(COMM_BOARD_C)
#include "config_comm_c.h"
#elif defined(SHUTDOWN_BOARD_B)
#include "config_shutdown_b.h"
#elif defined(SHUTDOWN_BOARD_C)
#include "config_shutdown_c.h"
#else
#error "Need to define a board configuration file" 
#endif


/* Unused pins and peripherals get unique values */
#define PIN_NONE              0
#define UART_NONE             0
#define ADC_NO_CHANNEL        0xFF


/* 
 * CAN Controller
 */
#ifndef CAN_CHANNEL_MSG
    #define CAN_CHANNEL_MSG   CAN_CHANNEL_1
#endif

/* CAN bit rate, in Kbits/sec */
#define CAN1_SPEED            125
/* NOTE: CAN2_SPEED is board specific */

/* Default CAN filters */
#ifndef CAN1_FILTER0_MASK
    #define CAN1_FILTER0_MASK ( CAN_ADDRESS_MASK << CAN_ADDRESS_SHIFT )
#endif
#ifndef CAN1_FILTER1_MASK
    #define CAN1_FILTER1_MASK ( CAN_ADDRESS_MASK << CAN_ADDRESS_SHIFT )
#endif
#ifndef CAN2_FILTER0_MASK
    #define CAN2_FILTER0_MASK (0x0000) /* Disabled */
#endif
#ifndef CAN2_FILTER1_MASK
    #define CAN2_FILTER1_MASK (0x0000) /* Disabled */
#endif

#ifndef CAN1_FILTER0_ID
    #define CAN1_FILTER0_ID   ( ADDRESS_SELF << CAN_ADDRESS_SHIFT )
#endif
#ifndef CAN1_FILTER1_ID
    #define CAN1_FILTER1_ID   ( ADDRESS_BROADCAST << CAN_ADDRESS_SHIFT )
#endif
#ifndef CAN2_FILTER0_ID
    #define CAN2_FILTER0_ID   (0xFFFF) /* Disabled */
#endif
#ifndef CAN2_FILTER1_ID
    #define CAN2_FILTER1_ID   (0xFFFF) /* Disabled */
#endif

/* LCD Configuration */
#ifndef LCD_USE_STM32F429_DISCOVERY // needed for weird TM driver
#define LCD_USE_STM32F429_DISCOVERY
#endif

/* Set pixel settings */
#define LCD_PIXEL_WIDTH        480
#define LCD_PIXEL_HEIGHT       272
#define LCD_PIXEL_SIZE         3

/* Other LCD Settings */
#define LCD_HSYNC              41
#define LCD_HBP                47
#define LCD_HFP                8
#define LCD_VSYNC              10
#define LCD_VBP                2
#define LCD_VFP                3

/* Frequency division for LTDC */
#define LCD_FREQUENCY_DIV      4

/*
 * UART Configuration
 */
#define UART_STOPBITS         UART_STOPBITS_1
#define UART_WORDLENGTH       UART_WORDLENGTH_8B
#define UART_FLOWCONTROL      UART_HWCONTROL_NONE
#define UART_PARITY           UART_PARITY_NONE

/*
 * ADC Configuration
 */
#define ADC_BUFFER_LENGTH         1024
#define ADC_MIN                   0x100
#define ADC_MAX                   0xF00

/*
 * Temperature Sensor Configuration
 */
#define I2C_3_FOR_TEMP_SENSOR	    1

/*
 * Device Family Pin Map
 */
#if defined(STM32F429xx)

/* I2C */
#define I2C1_SCL_GPIO_PIN     (GPIO_PIN_6)
#define I2C1_SCL_GPIO_GROUP   (GPIOB)
#define I2C1_SDA_GPIO_PIN     (GPIO_PIN_7)
#define I2C1_SDA_GPIO_GROUP   (GPIOB)
#define I2C2_SCL_GPIO_PIN     (GPIO_PIN_10)
#define I2C2_SCL_GPIO_GROUP   (GPIOB)
#define I2C2_SDA_GPIO_PIN     (GPIO_PIN_11)
#define I2C2_SDA_GPIO_GROUP   (GPIOB)
#define I2C3_SCL_GPIO_PIN     (GPIO_PIN_8)
#define I2C3_SCL_GPIO_GROUP   (GPIOA)
#define I2C3_SDA_GPIO_PIN     (GPIO_PIN_9)
#define I2C3_SDA_GPIO_GROUP   (GPIOC)

/* UART */
#define USART1_TX_GPIO_PIN    (GPIO_PIN_9)
#define USART1_TX_GPIO_GROUP  (GPIOA)
#define USART1_RX_GPIO_PIN    (GPIO_PIN_10)
#define USART1_RX_GPIO_GROUP  (GPIOA)
#define USART2_TX_GPIO_PIN    (GPIO_PIN_2)
#define USART2_TX_GPIO_GROUP  (GPIOA)
#define USART2_RX_GPIO_PIN    (GPIO_PIN_3)
#define USART2_RX_GPIO_GROUP  (GPIOA)
#define USART3_TX_GPIO_PIN    (GPIO_PIN_10)
#define USART3_TX_GPIO_GROUP  (GPIOB)
#define USART3_RX_GPIO_PIN    (GPIO_PIN_11)
#define USART3_RX_GPIO_GROUP  (GPIOB)
#define USART4_TX_GPIO_PIN    (GPIO_PIN_0)
#define USART4_TX_GPIO_GROUP  (GPIOA)
#define USART4_RX_GPIO_PIN    (GPIO_PIN_1)
#define USART4_RX_GPIO_GROUP  (GPIOA)
#define USART5_TX_GPIO_PIN    (GPIO_PIN_12)
#define USART5_TX_GPIO_GROUP  (GPIOC)
#define USART5_RX_GPIO_PIN    (GPIO_PIN_2)
#define USART5_RX_GPIO_GROUP  (GPIOD)
#define USART6_TX_GPIO_PIN    (GPIO_PIN_6)
#define USART6_TX_GPIO_GROUP  (GPIOC)
#define USART6_RX_GPIO_PIN    (GPIO_PIN_7)
#define USART6_RX_GPIO_GROUP  (GPIOC)
#define USART7_TX_GPIO_PIN    (GPIO_PIN_8)
#define USART7_TX_GPIO_GROUP  (GPIOE)
#define USART7_RX_GPIO_PIN    (GPIO_PIN_7)
#define USART7_RX_GPIO_GROUP  (GPIOE)
#define USART8_TX_GPIO_PIN    (GPIO_PIN_1)
#define USART8_TX_GPIO_GROUP  (GPIOE)
#define USART8_RX_GPIO_PIN    (GPIO_PIN_0)
#define USART8_RX_GPIO_GROUP  (GPIOE)

#define UART_N                8

/* ADC */
#define ADC1_C0_PIN           GPIO_PIN_0
#define ADC1_C0_GROUP         GPIOA
#define ADC1_C1_PIN           GPIO_PIN_1
#define ADC1_C1_GROUP         GPIOA
#define ADC1_C2_PIN           GPIO_PIN_2
#define ADC1_C2_GROUP         GPIOA
#define ADC1_C3_PIN           GPIO_PIN_3
#define ADC1_C3_GROUP         GPIOA
#define ADC1_C4_PIN           GPIO_PIN_4
#define ADC1_C4_GROUP         GPIOA
#define ADC1_C5_PIN           GPIO_PIN_5
#define ADC1_C5_GROUP         GPIOA
#define ADC1_C6_PIN           GPIO_PIN_6
#define ADC1_C6_GROUP         GPIOA
#define ADC1_C7_PIN           GPIO_PIN_7
#define ADC1_C7_GROUP         GPIOA
#define ADC1_C8_PIN           GPIO_PIN_0
#define ADC1_C8_GROUP         GPIOB
#define ADC1_C9_PIN           GPIO_PIN_1
#define ADC1_C9_GROUP         GPIOB
#define ADC1_C10_PIN          GPIO_PIN_0
#define ADC1_C10_GROUP        GPIOC
#define ADC1_C11_PIN          GPIO_PIN_1
#define ADC1_C11_GROUP        GPIOC
#define ADC1_C12_PIN          GPIO_PIN_2
#define ADC1_C12_GROUP        GPIOC
#define ADC1_C13_PIN          GPIO_PIN_3
#define ADC1_C13_GROUP        GPIOC
#define ADC1_C14_PIN          GPIO_PIN_4
#define ADC1_C14_GROUP        GPIOC
#define ADC1_C15_PIN          GPIO_PIN_5
#define ADC1_C15_GROUP        GPIOC
#define ADC2_C0_PIN           GPIO_PIN_0
#define ADC2_C0_GROUP         GPIOA
#define ADC2_C1_PIN           GPIO_PIN_1
#define ADC2_C1_GROUP         GPIOA
#define ADC2_C2_PIN           GPIO_PIN_2
#define ADC2_C2_GROUP         GPIOA
#define ADC2_C3_PIN           GPIO_PIN_3
#define ADC2_C3_GROUP         GPIOA
#define ADC2_C4_PIN           GPIO_PIN_4
#define ADC2_C4_GROUP         GPIOA
#define ADC2_C5_PIN           GPIO_PIN_5
#define ADC2_C5_GROUP         GPIOA
#define ADC2_C6_PIN           GPIO_PIN_5
#define ADC2_C6_GROUP         GPIOA
#define ADC2_C7_PIN           GPIO_PIN_7
#define ADC2_C7_GROUP         GPIOA
#define ADC2_C8_PIN           GPIO_PIN_0
#define ADC2_C8_GROUP         GPIOB
#define ADC2_C9_PIN           GPIO_PIN_1
#define ADC2_C9_GROUP         GPIOB
#define ADC2_C10_PIN          GPIO_PIN_0
#define ADC2_C10_GROUP        GPIOC
#define ADC2_C11_PIN          GPIO_PIN_1
#define ADC2_C11_GROUP        GPIOC
#define ADC2_C12_PIN          GPIO_PIN_2
#define ADC2_C12_GROUP        GPIOC
#define ADC2_C13_PIN          GPIO_PIN_3
#define ADC2_C13_GROUP        GPIOC
#define ADC2_C14_PIN          GPIO_PIN_4
#define ADC2_C14_GROUP        GPIOC
#define ADC2_C15_PIN          GPIO_PIN_5
#define ADC2_C15_GROUP        GPIOC
#define ADC3_C0_PIN           GPIO_PIN_0
#define ADC3_C0_GROUP         GPIOA
#define ADC3_C1_PIN           GPIO_PIN_1
#define ADC3_C1_GROUP         GPIOA
#define ADC3_C2_PIN           GPIO_PIN_2
#define ADC3_C2_GROUP         GPIOA
#define ADC3_C3_PIN           GPIO_PIN_3
#define ADC3_C3_GROUP         GPIOA
#define ADC3_C4_PIN           GPIO_PIN_6
#define ADC3_C4_GROUP         GPIOF
#define ADC3_C5_PIN           GPIO_PIN_7
#define ADC3_C5_GROUP         GPIOF
#define ADC3_C6_PIN           GPIO_PIN_8
#define ADC3_C6_GROUP         GPIOF
#define ADC3_C7_PIN           GPIO_PIN_9
#define ADC3_C7_GROUP         GPIOF
#define ADC3_C8_PIN           GPIO_PIN_10
#define ADC3_C8_GROUP         GPIOF
#define ADC3_C9_PIN           GPIO_PIN_3
#define ADC3_C9_GROUP         GPIOF
#define ADC3_C10_PIN          GPIO_PIN_0
#define ADC3_C10_GROUP        GPIOC
#define ADC3_C11_PIN          GPIO_PIN_1
#define ADC3_C11_GROUP        GPIOC
#define ADC3_C12_PIN          GPIO_PIN_2
#define ADC3_C12_GROUP        GPIOC
#define ADC3_C13_PIN          GPIO_PIN_3
#define ADC3_C13_GROUP        GPIOC
#define ADC3_C14_PIN          GPIO_PIN_4
#define ADC3_C14_GROUP        GPIOF
#define ADC3_C15_PIN          GPIO_PIN_5
#define ADC3_C15_GROUP        GPIOF

#define ADC_N                 3
#define ADC_CHANNEL_N         16

/* Temperature calibration registers */
#define TEMPERATURE_REG_CAL30  0x1FFF7A2C
#define TEMPERATURE_REG_CAL110 0x1FFF7A2E

#elif defined(STM32F407xx)

/* I2C */
#define I2C1_SCL_GPIO_PIN     (GPIO_PIN_8)
#define I2C1_SCL_GPIO_GROUP   (GPIOB)
#define I2C1_SDA_GPIO_PIN     (GPIO_PIN_9)
#define I2C1_SDA_GPIO_GROUP   (GPIOB)
#define I2C2_SCL_GPIO_PIN     (GPIO_PIN_10)
#define I2C2_SCL_GPIO_GROUP   (GPIOB)
#define I2C2_SDA_GPIO_PIN     (GPIO_PIN_11)
#define I2C2_SDA_GPIO_GROUP   (GPIOB)
#define I2C3_SCL_GPIO_PIN     (GPIO_PIN_8)
#define I2C3_SCL_GPIO_GROUP   (GPIOA)
#define I2C3_SDA_GPIO_PIN     (GPIO_PIN_9)
#define I2C3_SDA_GPIO_GROUP   (GPIOC)

/* USART */
#define USART1_TX_GPIO_PIN    (GPIO_PIN_9)
#define USART1_TX_GPIO_GROUP  (GPIOA)
#define USART1_RX_GPIO_PIN    (GPIO_PIN_10)
#define USART1_RX_GPIO_GROUP  (GPIOA)
#define USART2_TX_GPIO_PIN    (GPIO_PIN_2)
#define USART2_TX_GPIO_GROUP  (GPIOA)
#define USART2_RX_GPIO_PIN    (GPIO_PIN_3)
#define USART2_RX_GPIO_GROUP  (GPIOA)
#define USART3_TX_GPIO_PIN    (GPIO_PIN_10)
#define USART3_TX_GPIO_GROUP  (GPIOB)
#define USART3_RX_GPIO_PIN    (GPIO_PIN_11)
#define USART3_RX_GPIO_GROUP  (GPIOB)
#define USART4_TX_GPIO_PIN    (GPIO_PIN_10)
#define USART4_TX_GPIO_GROUP  (GPIOC)
#define USART4_RX_GPIO_PIN    (GPIO_PIN_11)
#define USART4_RX_GPIO_GROUP  (GPIOC)
#define USART5_TX_GPIO_PIN    PIN_NONE
#define USART5_TX_GPIO_GROUP  PIN_NONE
#define USART5_RX_GPIO_PIN    PIN_NONE
#define USART5_RX_GPIO_GROUP  PIN_NONE
#define USART6_TX_GPIO_PIN    PIN_NONE
#define USART6_TX_GPIO_GROUP  PIN_NONE
#define USART6_RX_GPIO_PIN    PIN_NONE
#define USART6_RX_GPIO_GROUP  PIN_NONE

#define UART_N                6

/* ADC */
#define ADC1_C0_PIN           GPIO_PIN_0
#define ADC1_C0_GROUP         GPIOA
#define ADC1_C1_PIN           GPIO_PIN_1
#define ADC1_C1_GROUP         GPIOA
#define ADC1_C2_PIN           GPIO_PIN_2
#define ADC1_C2_GROUP         GPIOA
#define ADC1_C3_PIN           GPIO_PIN_3
#define ADC1_C3_GROUP         GPIOA
#define ADC1_C4_PIN           GPIO_PIN_4
#define ADC1_C4_GROUP         GPIOA
#define ADC1_C5_PIN           GPIO_PIN_5
#define ADC1_C5_GROUP         GPIOA
#define ADC1_C6_PIN           GPIO_PIN_6
#define ADC1_C6_GROUP         GPIOA
#define ADC1_C7_PIN           GPIO_PIN_7
#define ADC1_C7_GROUP         GPIOA
#define ADC1_C8_PIN           GPIO_PIN_0
#define ADC1_C8_GROUP         GPIOB
#define ADC1_C9_PIN           GPIO_PIN_1
#define ADC1_C9_GROUP         GPIOB
#define ADC1_C10_PIN          GPIO_PIN_0
#define ADC1_C10_GROUP        GPIOC
#define ADC1_C11_PIN          GPIO_PIN_1
#define ADC1_C11_GROUP        GPIOC
#define ADC1_C12_PIN          GPIO_PIN_2
#define ADC1_C12_GROUP        GPIOC
#define ADC1_C13_PIN          GPIO_PIN_3
#define ADC1_C13_GROUP        GPIOC
#define ADC1_C14_PIN          GPIO_PIN_4
#define ADC1_C14_GROUP        GPIOC
#define ADC1_C15_PIN          GPIO_PIN_5
#define ADC1_C15_GROUP        GPIOC
#define ADC2_C0_PIN           GPIO_PIN_0
#define ADC2_C0_GROUP         GPIOA
#define ADC2_C1_PIN           GPIO_PIN_1
#define ADC2_C1_GROUP         GPIOA
#define ADC2_C2_PIN           GPIO_PIN_2
#define ADC2_C2_GROUP         GPIOA
#define ADC2_C3_PIN           GPIO_PIN_3
#define ADC2_C3_GROUP         GPIOA
#define ADC2_C4_PIN           GPIO_PIN_4
#define ADC2_C4_GROUP         GPIOA
#define ADC2_C5_PIN           GPIO_PIN_5
#define ADC2_C5_GROUP         GPIOA
#define ADC2_C6_PIN           GPIO_PIN_6
#define ADC2_C6_GROUP         GPIOA
#define ADC2_C7_PIN           GPIO_PIN_7
#define ADC2_C7_GROUP         GPIOA
#define ADC2_C8_PIN           GPIO_PIN_0
#define ADC2_C8_GROUP         GPIOB
#define ADC2_C9_PIN           GPIO_PIN_1
#define ADC2_C9_GROUP         GPIOB
#define ADC2_C10_PIN          GPIO_PIN_0
#define ADC2_C10_GROUP        GPIOC
#define ADC2_C11_PIN          GPIO_PIN_1
#define ADC2_C11_GROUP        GPIOC
#define ADC2_C12_PIN          GPIO_PIN_2
#define ADC2_C12_GROUP        GPIOC
#define ADC2_C13_PIN          GPIO_PIN_3
#define ADC2_C13_GROUP        GPIOC
#define ADC2_C14_PIN          GPIO_PIN_4
#define ADC2_C14_GROUP        GPIOC
#define ADC2_C15_PIN          GPIO_PIN_5
#define ADC2_C15_GROUP        GPIOC
#define ADC3_C0_PIN           GPIO_PIN_0
#define ADC3_C0_GROUP         GPIOA
#define ADC3_C1_PIN           GPIO_PIN_1
#define ADC3_C1_GROUP         GPIOA
#define ADC3_C2_PIN           GPIO_PIN_2
#define ADC3_C2_GROUP         GPIOA
#define ADC3_C3_PIN           GPIO_PIN_3
#define ADC3_C3_GROUP         GPIOA
#define ADC3_C4_PIN           GPIO_PIN_6
#define ADC3_C4_GROUP         GPIOF
#define ADC3_C5_PIN           GPIO_PIN_7
#define ADC3_C5_GROUP         GPIOF
#define ADC3_C6_PIN           GPIO_PIN_8
#define ADC3_C6_GROUP         GPIOF
#define ADC3_C7_PIN           GPIO_PIN_9
#define ADC3_C7_GROUP         GPIOF
#define ADC3_C8_PIN           GPIO_PIN_10
#define ADC3_C8_GROUP         GPIOF
#define ADC3_C9_PIN           GPIO_PIN_3
#define ADC3_C9_GROUP         GPIOF
#define ADC3_C10_PIN          GPIO_PIN_0
#define ADC3_C10_GROUP        GPIOC
#define ADC3_C11_PIN          GPIO_PIN_1
#define ADC3_C11_GROUP        GPIOC
#define ADC3_C12_PIN          GPIO_PIN_2
#define ADC3_C12_GROUP        GPIOC
#define ADC3_C13_PIN          GPIO_PIN_3
#define ADC3_C13_GROUP        GPIOC
#define ADC3_C14_PIN          GPIO_PIN_4
#define ADC3_C14_GROUP        GPIOF
#define ADC3_C15_PIN          GPIO_PIN_5
#define ADC3_C15_GROUP        GPIOF

#define ADC_N                 3
#define ADC_CHANNEL_N         16

/* Temperature calibration registers */
#define TEMPERATURE_REG_CAL30  0x1FFF7A2C
#define TEMPERATURE_REG_CAL110 0x1FFF7A2E

#else
    #error "Must select either the STM32F407xx (modules) or STM32F429xx (core)"
#endif

/* 
 * Unavailable pins and peripherals
 */
/* UART */
#ifndef USART7_TX_GPIO_PIN
    #define USART7_TX_GPIO_PIN   PIN_NONE
#endif
#ifndef USART7_TX_GPIO_GROUP
    #define USART7_TX_GPIO_GROUP PIN_NONE
#endif
#ifndef USART7_RX_GPIO_PIN
    #define USART7_RX_GPIO_PIN   PIN_NONE
#endif
#ifndef USART7_RX_GPIO_GROUP
    #define USART7_RX_GPIO_GROUP PIN_NONE
#endif
#ifndef USART8_TX_GPIO_PIN
    #define USART8_TX_GPIO_PIN   PIN_NONE
#endif
#ifndef USART8_TX_GPIO_GROUP
    #define USART8_TX_GPIO_GROUP PIN_NONE
#endif
#ifndef USART8_RX_GPIO_PIN
    #define USART8_RX_GPIO_PIN   PIN_NONE
#endif
#ifndef USART8_RX_GPIO_GROUP
    #define USART8_RX_GPIO_GROUP PIN_NONE
#endif

#ifndef UART_WIRELESS
    #define UART_WIRELESS        UART_NONE
#endif
#ifndef UART_OPENLOG
    #define UART_OPENLOG         UART_NONE
#endif
#ifndef UART_DEBUG
    #define UART_DEBUG           UART_NONE
#endif


/* Kill switch */
#ifndef KILL_NONRESETTABLE_GPIO_PIN
    #define KILL_NONRESETTABLE_GPIO_PIN   PIN_NONE
#endif
#ifndef KILL_NONRESETTABLE_GPIO_GROUP
    #define KILL_NONRESETTABLE_GPIO_GROUP PIN_NONE
#endif
#ifndef KILL_RESETTABLE_GPIO_PIN
    #define KILL_NONRESETTABLE_GPIO_PIN   PIN_NONE
#endif
#ifndef KILL_RESETTABLE_GPIO_GROUP
    #define KILL_NONRESETTABLE_GPIO_GROUP PIN_NONE
#endif

/* I2C mux */



#ifndef I2C_MUX1_SLAVE_ADDRESS
    #define I2C_MUX1_SLAVE_ADDRESS (0x74)
#endif
#ifndef I2C_MUX2_SLAVE_ADDRESS
    #define I2C_MUX2_SLAVE_ADDRESS TODO
#endif

//TODO update this based on the final mux hardware config
#ifndef I2C_CONFLICT_TEMP_SENSOR_ADDRESS
    #define I2C_CONFLICT_SENSOR_ADDRESS 15
#endif

//TODO update this for new IO Board
#ifndef I2C_MUX_CHANNEL
    #define I2C_MUX_CHANNEL I2C_3
#endif

/* CAN defaults */
#ifndef CAN2_ENABLE
    #define CAN2_TX_GPIO_PINS        TODO
    #define CAN2_TX_GPIO_GROUP       TODO
    #define CAN2_RX_GPIO_PINS        TODO
    #define CAN2_RX_GPIO_GROUP       TODO
#endif

/**       OTHER VALUES         */  
#define STM32F4xx           100 // for TM Libraries




#endif /* CONFIG_H */

