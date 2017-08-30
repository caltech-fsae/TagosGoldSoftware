/*
 * IMU Driver
 * Caltech Racing 2016
 */

/* TODO: Add gyroscope reading (CC-66). */
/* TODO: Add scaling to readings (CC-67). */

/* Standard libraries */

/* STM libraries */
#ifdef LINT
    #define _RTE_
    #include <stm32f4xx.h>
#endif

/* RTX */

/* FSAE Firmware */
#include "i2c.h"
#include "imu.h"

/* Local types */

/* Local constants */

/* Local variables */
static uint8_t receiveBuffer[ 8 ];
static uint8_t sendBuffer[ 2 ];

/* Local function prototypes */
static StatusInt readFromIMURegister( uint8_t reg, uint8_t* rcvData, uint8_t size );
static StatusInt writeToIMURegister( uint8_t reg, uint8_t value );



/* 
 * Function implementations
 */
 
/**
  * @brief: initializes the IMU.
  * @return: StatusInt
  */
StatusInt IMU_Init( void )
{
    StatusInt status;
    
    /* Initialize I2C channel */
    status = I2C_Init( IMU_I2C, I2C_MODE_POLLING );
    if ( status == STATUSINT_SUCCESS )
    {
       /* Verify communication */
       status = readFromIMURegister( IMU_REG_ID, receiveBuffer, 1 );
    }
    
    if ( ( status == STATUSINT_SUCCESS ) && ( receiveBuffer[ 0 ] == IMU_DEVICE_ID ) )
    {
       /* I2C communication is solid. Configure acceleration measurement */
       status = writeToIMURegister( IMU_REG_CTRL1_XL, IMU_ACCEL_CONFIG );
    }

    return status;
}

/**
  * @brief: gets x, y, z acceleration and returns them in linear representation
  *         from -2G to 2G.
  * @param[out] x : pointer to returned x acceleration 
  * @param[out] y : pointer to returned y acceleration 
  * @param[out] z : pointer to returned z acceleration 
  * @return: StatusInt
  */
StatusInt IMU_getAcceleration( int16_t *x, int16_t *y, int16_t *z )
{
    StatusInt status;

    status = readFromIMURegister( IMU_REG_OUTX_L_XL, receiveBuffer, 6 );

    if ( status == STATUSINT_SUCCESS )
    {
        *x = ( receiveBuffer[ IMU_ACCEL_X_HI ] << 8 ) | receiveBuffer[ IMU_ACCEL_X_LO ];
        *y = ( receiveBuffer[ IMU_ACCEL_Y_HI ] << 8 ) | receiveBuffer[ IMU_ACCEL_Y_LO ];
        *z = ( receiveBuffer[ IMU_ACCEL_Z_HI ] << 8 ) | receiveBuffer[ IMU_ACCEL_Z_LO ];
    }

    return status;
}

/**
  * @brief: reads from register "reg" in IMU and returns
  *         the "size" number of bytes read to "rcvData"
  * @param[in] regAddr: register address to read from
  * @param[out] rcvData: pointer to data that will be received
  * @param[in] size: number of bytes to read
  * @return: StatusInt
  */
static StatusInt readFromIMURegister( uint8_t reg, uint8_t* rcvData, uint8_t size )
{
    StatusInt status;

    status = I2C_Write( IMU_I2C, IMU_ADDRESS, &reg, 1 );

    if ( status == STATUSINT_SUCCESS )
    {
        status = I2C_Read( IMU_I2C, IMU_ADDRESS, rcvData, size );
    }

    return status;
}

/**
  * @brief: writes value to register "reg" in IMU
  * @param[in] regAddr: register address to send data to
  * @param[in] value: data to be written
  * @return: StatusInt
  */
static StatusInt writeToIMURegister( uint8_t reg, uint8_t value )
{
    StatusInt status;

    sendBuffer[ 0 ] = reg;
    sendBuffer[ 1 ] = value;
    status = I2C_Write( IMU_I2C, IMU_ADDRESS, sendBuffer, 2 );

    return status;
}
