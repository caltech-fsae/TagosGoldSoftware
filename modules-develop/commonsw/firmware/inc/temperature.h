/*
 * Temperature Sensor Driver
 * Caltech Racing 2016
 */
#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "common.h" /* StatusInt */
 
#define ADC_TEMPERATURE 1   /* ADC used for temperature sensing */
#define MUX_STATUS_INVALID (0xFFFF)
#define CHANNEL_NO_PENDING_SENSOR (0xFFFF)
#define TEMP_I2C_ERROR (0xFFFF)
#define NUM_CHANNEL_PER_MUX 8
#define NUM_SENSOR_PER_CHANNEL (27)
#define NUM_MUX_PER_I2C (2)

enum{
    I2C_CHANNEL_INACTIVE = 0,
    I2C_CHANNEL_TEMPERATURE
};

typedef void(*temp_callback_t)(uint8_t i2c_channel, uint16_t sensor_id, uint16_t temp);


StatusInt temperature_InitExtUnit(temp_callback_t f);
StatusInt temperature_InitExt(uint8_t i2c_channel_id, uint8_t mux_id, uint16_t sensor_id);
StatusInt temperature_InitInt( void);
StatusInt temperature_GetExtInteger( uint8_t i2c_channel_id, uint8_t mux_id, uint16_t sensor_id);
StatusInt temperature_GetInt( float* temp );
int temperature_get_i2c_channel_status(uint8_t i2c_channel);

#endif

