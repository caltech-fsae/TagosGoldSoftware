/*
 * Comm Board Revision B Configuration
 * Caltech Racing 2016
 */
#ifndef COMM_BOARD_H
#define COMM_BOARD_H

/*
 * Device Configuration
 */
#if !defined(STM32F407xx)
    #error "Compile for STM32F407xx."
#endif
#if ( HSE_VALUE != 8000000 )   /* HSE * PLLN / PLLM = 160000000 */
    #error "Define HSE_VALUE=8000000 in the target settings."
#endif
#define PLLM_VALUE          8
#define PLLN_VALUE          320

/* CAN controller */
#define CAN2_ENABLE         0
#define CAN2_SPEED          125
#define CAN1_FILTER0_MASK   ( 0x0000 << CAN_ADDRESS_SHIFT  ) /* All messages through */
#define CAN1_FILTER0_ID     ( ADDRESS_COMM << CAN_ADDRESS_SHIFT  ) /* Only messages for comm board through */

#define ADDRESS_SELF        ADDRESS_COMM

/* USART Ports */
#define UART_WIRELESS       1
#define UART_OPENLOG        2
#define UART_DEBUG          UART_OPENLOG

/*
 * Board-specific Pin Map
 */

/* Fault Read Pin Map */
#define FAULT_I_GPIO_PIN    GPIO_PIN_12
#define FAULT_I_GPIO_GROUP  GPIOA
#define FAULTN_I_GPIO_PIN   GPIO_PIN_12
#define FAULTN_I_GPIO_GROUP GPIOA
#define FAULT_O_GPIO_PIN    GPIO_PIN_12
#define FAULT_O_GPIO_GROUP  GPIOA
#define FAULTN_O_GPIO_PIN   GPIO_PIN_12
#define FAULTN_O_GPIO_GROUP GPIOA

/* CAN */
#define CAN1_TX_GPIO_PINS   GPIO_PIN_12
#define CAN1_TX_GPIO_GROUP  GPIOA
#define CAN1_RX_GPIO_PINS   GPIO_PIN_11
#define CAN1_RX_GPIO_GROUP  GPIOA
#define CAN2_TX_GPIO_PINS   GPIO_PIN_6
#define CAN2_TX_GPIO_GROUP  GPIOB
#define CAN2_RX_GPIO_PINS   GPIO_PIN_5
#define CAN2_RX_GPIO_GROUP  GPIOB

#endif /* COMM_BOARD_H */
