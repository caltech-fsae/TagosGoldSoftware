/*
 * IO Board Revision B Configuration
 * Caltech Racing 2016
 */
#ifndef IO_BOARD_H
#define IO_BOARD_H

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

/* CAN */
#define ADDRESS_SELF ADDRESS_IO_PEDALS_1
#define CAN2_ENABLE         1
#define CAN2_SPEED          125

/* ADC */
#define ADC_THROTTLE              ADC_1                    
#define ADC_THROTTLE_CHANNEL      ADC_CHANNEL_7
#define ADC_THROTTLE_SAMPLINGTIME ADC_SAMPLETIME_3CYCLES 
#define ADC_BRAKE                 ADC_2                   
#define ADC_BRAKE_CHANNEL         ADC_CHANNEL_6
#define ADC_BRAKE_SAMPLINGTIME    ADC_SAMPLETIME_3CYCLES 
#define ADC_STEERING              ADC_1                      
#define ADC_STEERING_CHANNEL      ADC_CHANNEL_11
#define ADC_STEERING_SAMPLINGTIME ADC_SAMPLETIME_3CYCLES

/* USART Ports */
#define UART_DEBUG 3

/*
 * Board-specific Pin Map
 */

/* CAN Pin Map */
#define CAN1_TX_GPIO_PINS        (GPIO_PIN_12)
#define CAN1_TX_GPIO_GROUP       (GPIOA)
#define CAN1_RX_GPIO_PINS        (GPIO_PIN_11)
#define CAN1_RX_GPIO_GROUP       (GPIOA)
#define CAN2_TX_GPIO_PINS        (GPIO_PIN_5)
#define CAN2_TX_GPIO_GROUP       (GPIOB)
#define CAN2_RX_GPIO_PINS        (GPIO_PIN_6)
#define CAN2_RX_GPIO_GROUP       (GPIOB)

#endif /* IO_BOARD_H */
