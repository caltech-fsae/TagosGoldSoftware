/*
 * Core Board Revision A Configuration
 * Caltech Racing 2016
 */
#ifndef CORE_BOARD_H
#define CORE_BOARD_H

/*
 * Device Configuration
 */
#if !defined(STM32F429xx)
#error "Compile for STM32F429xx."
#endif
#if ( HSE_VALUE != 20000000 )   /* HSE * PLLN / PLLM = 160000000 */
#error "Define HSE_VALUE=20000000 in the target settings."
#endif
#define PLLM_VALUE          8
#define PLLN_VALUE          320

/* CAN controller */
#define CAN2_ENABLE         1
#define CAN2_SPEED          250
#define CAN_CHANNEL_MOTORS  CAN_CHANNEL_2
#define CAN2_FILTER0_MASK   ( CAN_ADDRESS_MASK << CAN_ADDRESS_SHIFT )
#define CAN2_FILTER0_ID     ( ADDRESS_MOTOR << CAN_ADDRESS_SHIFT )

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

/*
* CAN pin out
*/
/* CAN */
#define CAN1_TX_GPIO_PINS        (GPIO_PIN_0)
#define CAN1_TX_GPIO_GROUP       (GPIOD)
#define CAN1_RX_GPIO_PINS        (GPIO_PIN_1)
#define CAN1_RX_GPIO_GROUP       (GPIOD)
#define CAN2_TX_GPIO_PINS        (GPIO_PIN_5)
#define CAN2_TX_GPIO_GROUP       (GPIOB)
#define CAN2_RX_GPIO_PINS        (GPIO_PIN_13)
#define CAN2_RX_GPIO_GROUP       (GPIOB)




#endif /* CORE_BOARD_H */
