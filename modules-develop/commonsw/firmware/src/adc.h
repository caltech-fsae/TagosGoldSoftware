/*
 * ADC Driver
 * Caltech Racing 2016
 */
#ifndef ADC_H
#define ADC_H

enum {
    ADC_1 = 0,
    ADC_2,
    ADC_3
};
enum {
    ADC_CHANNEL1 = 0,
    ADC_CHANNEL2,
    ADC_CHANNEL3,
    ADC_CHANNEL4,
    ADC_CHANNEL5,
    ADC_CHANNEL6,
    ADC_CHANNEL7,
    ADC_CHANNEL8,
    ADC_CHANNEL9,
    ADC_CHANNEL10,
    ADC_CHANNEL11,
    ADC_CHANNEL12,
    ADC_CHANNEL13,
    ADC_CHANNEL14,
    ADC_CHANNEL15
};

#define ADC_TIMEOUT     500

StatusInt ADC_Init ( uint8_t adc, uint8_t channel1, uint8_t channel2, uint32_t sampling_time );
StatusInt ADC_Read ( uint32_t *value1, uint32_t *value2, uint8_t adc );

#endif /* ADC_H */
