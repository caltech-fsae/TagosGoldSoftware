/*
 * I2C Driver
 * Caltech Racing 2016
 */
#ifndef I2C_H
#define I2C_H

#include "common.h"
#include <stdint.h>

#define I2C_CLOCKSPEED  100000
#define I2C_TIMEOUT     5000
#define I2C_MAX_DATA_SIZE 8 //TODO check this number later
#define I2C_MAX_BUSY_TRY 50

/*
 * I2C states
 */
enum {
	I2C_STATE_IDLE = 0,
	I2C_STATE_TRANSMIT,
	I2C_STATE_TIMEOUT,
	I2C_STATE_RECEIVE,
	I2C_STATE_ERROR
};

/*
 * I2C channels
 */
enum {
    I2C_1 = 0,
    I2C_2,
    I2C_3,
    I2C_CHANNEL_N
};

/*
 * I2C interface mode. In order to use I2C_Write_IT or I2C_Read_IT,
 * I2C need to be initialized as I2C_MODE_IT
 */
enum {
	I2C_MODE_IT = 0,
	I2C_MODE_POLLING
};

/**
 * info message type
 */
typedef struct
{
    uint8_t data[I2C_MAX_DATA_SIZE];
    uint32_t address;
    uint16_t length;
	uint8_t state;
} I2CInfoStruct_t;




/**
 * I2C event callback type
 */
typedef void ( *I2CRxCallBack_t )( uint8_t i2c_channel, I2CInfoStruct_t* rx_info );


StatusInt I2C_Init( uint8_t i2c_num, uint8_t mode );
StatusInt I2C_Write( uint8_t i2c_num, uint16_t devAddress, uint8_t *pData, uint16_t size );
StatusInt I2C_Read( uint8_t i2c_num, uint16_t devAddress, uint8_t *pData, uint16_t size );
StatusInt I2C_ErrorHandle(int i2c_num);
StatusInt I2C_RegisterCallback(uint8_t i2c_channel, I2CRxCallBack_t f);
StatusInt I2C_Write_IT( uint8_t i2c_num, uint16_t devAddress, uint8_t *pData, uint16_t size );
StatusInt I2C_Read_IT( uint8_t i2c_num, uint16_t devAddress, uint8_t *pData, uint16_t size );

void I2C_HandleReceives(void);
void I2C_HandleTransmit(void);

#endif /* I2C_H */
