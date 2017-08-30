
#ifndef CONFIG_SHUTDOWN_C
#define CONFIG_SHUTDOWN_C

#if !defined(STM32F407xx)
    #error "Compile for STM32F407xx."
#endif
#if ( HSE_VALUE != 8000000 )   /* HSE * PLLN / PLLM = 160000000 */
    #error "Define HSE_VALUE=8000000 in the target settings."
#endif
#define PLLM_VALUE          8
#define PLLN_VALUE          320

/* CAN controller */
#define CAN2_ENABLE         1
#define CAN2_SPEED          125

/* Use CAN2 for BMS */
#define CAN_CHANNEL_BMS     CAN_CHANNEL_2
#define CAN2_FILTER0_MASK   ( CAN_ADDRESS_BMS_MASK << CAN_ADDRESS_SHIFT )
#define CAN2_FILTER0_ID     ( ADDRESS_BMS << CAN_ADDRESS_SHIFT )
#define CAN2_FILTER1_MASK   ( 0xFFFF )
#define CAN2_FILTER1_ID     ( 0x0000 )


#define ADDRESS_SELF ADDRESS_SHUTDOWN

/*
 * Board-specific Pin Map
 */

/* FAULT Output Pins */
#define FAULT_O_GPIO_PIN     GPIO_PIN_7
#define FAULT_O_GPIO_GROUP   GPIOE
#define FAULTN_O_GPIO_PIN    GPIO_PIN_2
#define FAULTN_O_GPIO_GROUP  GPIOB
#define FAULT_RES_GPIO_PIN   GPIO_PIN_1
#define FAULT_RES_GPIO_GROUP GPIOB

/* FAULT Pins */
#define FAULT_I_GPIO_PIN     GPIO_PIN_13
#define FAULT_I_GPIO_GROUP   GPIOD
#define FAULTN_I_GPIO_PIN    GPIO_PIN_12
#define FAULTN_I_GPIO_GROUP  GPIOD
#define AMS_I_GPIO_PIN       GPIO_PIN_11
#define AMS_I_GPIO_GROUP     GPIOD
#define IMD_I_GPIO_PIN       GPIO_PIN_10
#define IMD_I_GPIO_GROUP     GPIOD

/* LATCH Pins */
#define FAULT_LATCH_GPIO_PIN    GPIO_PIN_15
#define FAULT_LATCH_GPIO_GROUP  GPIOE
#define FAULTN_LATCH_GPIO_PIN   GPIO_PIN_14
#define FAULTN_LATCH_GPIO_GROUP GPIOE
#define AMS_LATCH_GPIO_PIN      GPIO_PIN_13
#define AMS_LATCH_GPIO_GROUP    GPIOE
#define IMD_LATCH_GPIO_PIN      GPIO_PIN_12
#define IMD_LATCH_GPIO_GROUP    GPIOE

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
#define INTERLOCK_GPIO_PIN      GPIO_PIN_4
#define INTERLOCK_GPIO_GROUP    GPIOA

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


#define CAN_RS_CONTROL
#define CAN1_RS_GPIO_PINS        (GPIO_PIN_1)
#define CAN1_RS_GPIO_GROUP       (GPIOE)
#define CAN2_RS_GPIO_PINS        (GPIO_PIN_15)
#define CAN2_RS_GPIO_GROUP       (GPIOB)

#define CAN_SHDN_CONTROL
#define CAN1_SHDN_GPIO_PIN      GPIO_PIN_0
#define CAN1_SHDN_GPIO_GROUP    GPIOE

#endif
