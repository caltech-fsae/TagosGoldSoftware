
#ifndef CONFIG_SHUTDOWN_D
#define CONFIG_SHUTDOWN_D

#if defined(STM32F429xx)
    #error "Compile for STM32F407xx."
#endif
#if ( HSE_VALUE != 20000000 )   /* HSE * PLLN / PLLM = 160000000 */
    #error "Define HSE_VALUE=20000000 in the target settings."
#endif
#define PLLM_VALUE          16
#define PLLN_VALUE          256

/* CAN controller */
#define CAN2_ENABLE         1

#define ADDRESS_SELF ADDRESS_SHUTDOWN

/*
 * Board-specific Pin Map
 */

/* FAULT Output Pins */
#define FAULT_O_GPIO_PIN     GPIO_PIN_11
#define FAULT_O_GPIO_GROUP   GPIOC
#define FAULTN_O_GPIO_PIN    GPIO_PIN_10
#define FAULTN_O_GPIO_GROUP  GPIOC

/* FAULT Pins */
#define FAULT_I_GPIO_PIN     GPIO_PIN_14
#define FAULT_I_GPIO_GROUP   GPIOE
#define FAULTN_I_GPIO_PIN    GPIO_PIN_11
#define FAULTN_I_GPIO_GROUP  GPIOE

/* Relay Pins */
#define DCHG_GPIO_PIN    GPIO_PIN_11
#define DCHG_GPIO_GROUP  GPIOA
#define PCHG_GPIO_PIN    GPIO_PIN_12
#define PCHG_GPIO_GROUP  GPIOA
#define AIR1H_GPIO_PIN   GPIO_PIN_6
#define AIR1H_GPIO_GROUP GPIOC
#define AIR1L_GPIO_PIN   GPIO_PIN_15
#define AIR1L_GPIO_GROUP GPIOD
#define AIR2H_GPIO_PIN   GPIO_PIN_8
#define AIR2H_GPIO_GROUP GPIOC
#define AIR2L_GPIO_PIN   GPIO_PIN_7
#define AIR2L_GPIO_GROUP GPIOC
#define AIR3H_GPIO_PIN   GPIO_PIN_10
#define AIR3H_GPIO_GROUP GPIOA
#define AIR3L_GPIO_PIN   GPIO_PIN_9
#define AIR3L_GPIO_GROUP GPIOA

/* Interlock Pins */
#define INTERLOCK_GPIO_PIN      GPIO_PIN_10
#define INTERLOCK_GPIO_GROUP    GPIOE

#define KILL_NONRESETTABLE_GPIO_PIN   TODO
#define KILL_NONRESETTABLE_GPIO_GROUP TODO
#define KILL_RESETTABLE_GPIO_PIN      TODO
#define KILL_RESETTABLE_GPIO_GROUP    TODO

/* CAN Pin Map */
#define CAN1_TX_GPIO_PINS        (GPIO_PIN_9)
#define CAN1_TX_GPIO_GROUP       (GPIOB)
#define CAN1_RX_GPIO_PINS        (GPIO_PIN_8)
#define CAN1_RX_GPIO_GROUP       (GPIOB)
#define CAN2_TX_GPIO_PINS        (GPIO_PIN_13)
#define CAN2_TX_GPIO_GROUP       (GPIOB)
#define CAN2_RX_GPIO_PINS        (GPIO_PIN_12)
#define CAN2_RX_GPIO_GROUP       (GPIOB)


#define CAN1_RS_GPIO_PINS        (GPIO_PIN_1)
#define CAN1_RS_GPIO_GROUP       (GPIOE)
#define CAN2_RS_GPIO_PINS        (GPIO_PIN_15)
#define CAN2_RS_GPIO_GROUP       (GPIOB)


#endif
