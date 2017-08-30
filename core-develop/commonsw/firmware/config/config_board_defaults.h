/*
 * Configuration for Linter
 * Caltech Racing 2016
 */
#ifndef CONFIG_BOARD_DEFAULTS_H
#define CONFIG_BOARD_DEFAULTS_H

#define PLLM_VALUE          16
#define PLLN_VALUE          256

/* CAN controller */
#define CAN2_ENABLE         1
#define CAN2_SPEED          250
#define CAN_CHANNEL_MOTORS  CAN_CHANNEL_2
#define CAN_CHANNEL_BMS     CAN_CHANNEL_2
#define ADDRESS_SELF        ADDRESS_BROADCAST

/*
 * ADC Configuration
 */
#define ADC_THROTTLE              ADC_2
#define ADC_THROTTLE_CHANNEL1     ADC_CHANNEL_13
#define ADC_THROTTLE_CHANNEL2     ADC_CHANNEL_6
#define ADC_THROTTLE_SAMPLINGTIME ADC_SAMPLETIME_3CYCLES
#define ADC_BRAKE                 ADC_1
#define ADC_BRAKE_CHANNEL1        ADC_CHANNEL_12
#define ADC_BRAKE_CHANNEL2        ADC_CHANNEL_13
#define ADC_BRAKE_SAMPLINGTIME    ADC_SAMPLETIME_3CYCLES
#define ADC_STEERING              ADC_3
#define ADC_STEERING_CHANNEL1     ADC_CHANNEL_11
#define ADC_STEERING_CHANNEL2     ADC_NO_CHANNEL
#define ADC_STEERING_SAMPLINGTIME ADC_SAMPLETIME_3CYCLES
#define ADC_MIN                   0x100
#define ADC_MAX                   0xF00
#define ADC_NO_CHANNEL            0xFF
#define ADC_BUFFER_LENGTH         1024

/* USART Ports */
#define UART_WIRELESS       1
#define UART_OPENLOG        2
#define UART_DEBUG          3


/* I2C Ports */
#define IMU_I2C             I2C_3
#define TEMP_I2C            I2C_3
#define TEMP_I2C_ADDR       0

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

/*
 * Shutdown board pins
 */

#define FAULT_RES_GPIO_PIN   GPIO_PIN_1
#define FAULT_RES_GPIO_GROUP GPIOB

/* FAULT Pins */
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


#endif //CONFIG_BOARD_DEFAULTS_H

