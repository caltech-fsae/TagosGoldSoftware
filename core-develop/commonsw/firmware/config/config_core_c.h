/*
 * Core Board Revision C Configuration
 * Caltech Racing 2016
 */
#ifndef CORE_BOARD_H
#define CORE_BOARD_H

/*
 * Device Configuration
 */
#if ( HSE_VALUE != 8000000 )   /* HSE * PLLN / PLLM = 160000000 */
    #error "Define HSE_VALUE=8000000 in the target settings."
#endif

#if !defined(STM32F429xx)
    #error "Compile for STM32F429xx."
#endif

#define PLLM_VALUE          8
#define PLLN_VALUE          320

/* CAN controller */
#define CAN2_ENABLE         1
#define CAN2_SPEED          250
#define CAN_CHANNEL_MOTORS  CAN_CHANNEL_2
#define CAN2_FILTER0_MASK   ( CAN_ADDRESS_MASK << CAN_ADDRESS_SHIFT )
#define CAN2_FILTER0_ID     ( ADDRESS_MOTOR << CAN_ADDRESS_SHIFT )
#define CAN2_FILTER1_MASK   ( 0x0000 )
#define CAN2_FILTER1_ID     ( 0x0000 )

#define ADDRESS_SELF        ADDRESS_CORE

/* USART Ports */
#define UART_DEBUG          3

/* I2C Ports */
#define IMU_I2C             I2C_3
#define TEMP_I2C            I2C_3
#define TEMP_I2C_ADDR       0

/*
 * Board-specific Pin Map
 */

/* Fault Read Pin Map */
/*
#define FAULT_I_GPIO_PIN    GPIO_PIN_12
#define FAULT_I_GPIO_GROUP  GPIOA
#define FAULTN_I_GPIO_PIN   GPIO_PIN_12
#define FAULTN_I_GPIO_GROUP GPIOA
#define FAULT_O_GPIO_PIN    GPIO_PIN_12
#define FAULT_O_GPIO_GROUP  GPIOA
#define FAULTN_O_GPIO_PIN   GPIO_PIN_12
#define FAULTN_O_GPIO_GROUP GPIOA
*/

/*
 * CAN pin out
 */
#define CAN1_TX_GPIO_PINS        (GPIO_PIN_13)
#define CAN1_TX_GPIO_GROUP       (GPIOH)
#define CAN1_RX_GPIO_PINS        (GPIO_PIN_9)
#define CAN1_RX_GPIO_GROUP       (GPIOI)
#define CAN2_TX_GPIO_PINS        (GPIO_PIN_12)
#define CAN2_TX_GPIO_GROUP       (GPIOB)
#define CAN2_RX_GPIO_PINS        (GPIO_PIN_13)
#define CAN2_RX_GPIO_GROUP       (GPIOB)

#define CAN_RS_CONTROL
#define CAN1_RS_GPIO_PINS        (GPIO_PIN_14)  
#define CAN1_RS_GPIO_GROUP       (GPIOB)
#define CAN2_RS_GPIO_PINS        (GPIO_PIN_15)
#define CAN2_RS_GPIO_GROUP       (GPIOB)

/* Peripherals */
#define DRIVERBUTTON_GPIO_PIN    (GPIO_PIN_0)
#define DRIVERBUTTON_GPIO_GROUP  (GPIOA)
#define RTDS_GPIO_PIN            (GPIO_PIN_1)
#define RTDS_GPIO_GROUP          (GPIOC)

#endif /* CORE_BOARD_H */
