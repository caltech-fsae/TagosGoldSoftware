/*
 * Temperature Sensor Driver
 * Caltech Racing 2016
 */

/* Standard libraries */

/* STM libraries */
#ifdef LINT
#define _RTE_
    #include <stm32f4xx.h>
#endif
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_adc.h>

/* RTX */
#include <assert.h>

/* FSAE Firmware */
#include "common.h"
#include "i2c.h"
#include "adc.h"
#include "temperature.h"


/* Local types */

/* Local constants */
/**
  * below is used to map the address of the temperature 
  * indexed by 9 * A2 + 3 * A1 + A0 where each A2, A1, A0 
  * can be 2 (float), 1 (high), 0 (low) to the slave 
  * address of the map.
  */

static const uint8_t slave_addr[ 27 ] =
{
    0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x35, 0x36, 0x37
};



/**
** i2c mux slave channel select control register value.
* The following value must be write to control register on i2c mux 
* in order to enable to the slave channel
*/
static const uint8_t mux_slave_select[] = {
    0x8, 0x9, 0xA, 0xB, 
    0xC, 0xD, 0xE, 0xF
};



/**
 * i2c mux slave addresses
 */
static const uint16_t mux_slave_address[NUM_MUX_PER_I2C] = {
        I2C_MUX1_SLAVE_ADDRESS,
        I2C_MUX2_SLAVE_ADDRESS
};



/* Local function prototypes */

StatusInt i2c_mux_enable(uint8_t i2c_channel_id, uint16_t mux_id, uint16_t mux_channel);
void temperature_monitor_I2C_callback(uint8_t i2c_channel, I2CInfoStruct_t* rx_info);

/* Local Variables */
static temp_callback_t temperature_rx_callback = NULL;


// currently assume 2 mux per i2c channel
static uint32_t mux_channel_status[I2C_CHANNEL_N][NUM_MUX_PER_I2C] = {
	{MUX_STATUS_INVALID, MUX_STATUS_INVALID},
	{MUX_STATUS_INVALID, MUX_STATUS_INVALID},
	{MUX_STATUS_INVALID, MUX_STATUS_INVALID}
};

static uint16_t channel_pending_sensor_id[I2C_CHANNEL_N]={
	CHANNEL_NO_PENDING_SENSOR,CHANNEL_NO_PENDING_SENSOR,CHANNEL_NO_PENDING_SENSOR
};

static uint16_t i2c_channel_status[I2C_CHANNEL_N]={
        I2C_CHANNEL_INACTIVE, I2C_CHANNEL_INACTIVE, I2C_CHANNEL_INACTIVE
};

/*
 * Function implementations
 */



/**
  *  @brief: enable i2c mux channel 
  *  @param[in] i2c_channel_id number, must be 0 to 2
  *  @param[in] mux_id number, must be 0 to 7
  *  @return: StatusInt. STATUSINT_SUCCESS if no error.
  */
StatusInt i2c_mux_enable(uint8_t i2c_channel_id, uint16_t mux_id, uint16_t mux_channel){
    StatusInt status;
    uint8_t wrData;
	if (( mux_id >= NUM_MUX_PER_I2C)||(mux_channel >= NUM_CHANNEL_PER_MUX)){
        return STATUSINT_ERROR_BADARG;
    }
		// if the channel is enabled already, then just return success
    if (mux_channel_status[i2c_channel_id][mux_id] == (0x1 << mux_channel)){
        return STATUSINT_SUCCESS;
    }else{
        wrData = mux_slave_select[mux_channel];
        status = I2C_Write(i2c_channel_id, mux_slave_address[mux_id],\
        &wrData, sizeof(wrData));
    }
    return status;
}

StatusInt temperature_RegisterRxCallback(temp_callback_t f){
	temperature_rx_callback = f;
	return STATUSINT_SUCCESS;
}

StatusInt temperature_InitExtUnit(temp_callback_t f){
	StatusInt status;
	temperature_rx_callback = f;
#ifdef I2C_1_FOR_TEMP_SENSOR
	status = I2C_Init(I2C_1, I2C_MODE_IT);
	if (status != STATUSINT_SUCCESS){
		return status;
	}
    i2c_channel_status[I2C_1] = I2C_CHANNEL_TEMPERATURE;
	I2C_RegisterCallback(I2C_1, &(temperature_monitor_I2C_callback));
#endif
#ifdef I2C_2_FOR_TEMP_SENSOR
	status = I2C_Init(I2C_2, I2C_MODE_IT);
	if (status != STATUSINT_SUCCESS){
		return status;
	}
	i2c_channel_status[I2C_2] = I2C_CHANNEL_TEMPERATURE;
	I2C_RegisterCallback(I2C_2, &(temperature_monitor_I2C_callback));
#endif
#ifdef I2C_3_FOR_TEMP_SENSOR
	status = I2C_Init(I2C_3, I2C_MODE_IT);
	if (status != STATUSINT_SUCCESS){
		return status;
	}
	i2c_channel_status[I2C_3] = I2C_CHANNEL_TEMPERATURE;
	I2C_RegisterCallback(I2C_3, &(temperature_monitor_I2C_callback));
#endif
	return STATUSINT_SUCCESS;
}

/**
  * @brief: initializes temperature sensor installed under encoded address.
  *         Assumes I2C channel is pre-initialized. Should call temperature_InitExtUnit
  *         before calling this function on each sensor.
  * @param[in] i2c: i2c channel for the given sensor
  * @param[in] mux_id: mux_id on the specified i2c channel. 0 to NUM_MUX_PER_I2C - 1
  * @param[in] sensor_id: sensor_id on the specified mux. 0 to
  * NUM_SENSOR_PER_CHANNEL * NUM_CHANNEL_PER_MUX - 1
  * @return: StatusInt
  */
StatusInt temperature_InitExt(uint8_t i2c_channel_id, uint8_t mux_id, uint16_t sensor_id)
{
    StatusInt status;
    uint8_t sndData[ 1 ] = { 0 };
    status = i2c_mux_enable(i2c_channel_id, mux_id, sensor_id/NUM_SENSOR_PER_CHANNEL);
    if (status != STATUSINT_SUCCESS){
        return status;
    }
    /* Set the pointer register of the device */
    status = I2C_Write(i2c_channel_id, slave_addr[ sensor_id%NUM_SENSOR_PER_CHANNEL ], sndData, sizeof( sndData ) );
    return status;
}

/**
  * @brief: initializes temperature sensor installed under encoded address.
  *         Assumes I2C channel is pre-initialized.
  * @return: StatusInt
  */
/*
StatusInt temperature_InitInt( void )
{
    StatusInt status;

    // Cast channel to uint8 to remove flag.
    status = ADC_Init( ADC_TEMPERATURE, (uint8_t) ADC_CHANNEL_TEMPSENSOR, ADC_NO_CHANNEL, ADC_SAMPLETIME_112CYCLES );

    return status;
}*/

/**
  * @brief: request external temperature using temperature sensor at the address specified.
  * The temperature will be return in temp_rx_callback if available
  * @param[in] i2c: i2c channel for the given sensor
  * @param[in] mux_id: mux_id on the specified i2c channel. 0 to NUM_MUX_PER_I2C - 1
  * @param[in] sensor_id: sensor_id on the specified mux. 0 to
  * NUM_SENSOR_PER_CHANNEL * NUM_CHANNEL_PER_MUX - 1
  * @return: StatusInt
  */
StatusInt temperature_GetExtInteger( uint8_t i2c_channel_id, uint8_t mux_id, uint16_t sensor_id)
{
    StatusInt status;
    uint8_t rcvData[ 2 ];
    uint8_t address;
    /* Enable i2c device on mux */ 
    status = i2c_mux_enable(i2c_channel_id, mux_id, sensor_id/NUM_SENSOR_PER_CHANNEL);
    if (status != STATUSINT_SUCCESS){
        return status;
    }
    /* Read from the temperature sensor. */
    address = sensor_id % NUM_SENSOR_PER_CHANNEL;
    /* NEED TO set pending sensor id before calling i2c read */
    channel_pending_sensor_id[i2c_channel_id] = sensor_id + mux_id * NUM_MUX_PER_I2C * NUM_CHANNEL_PER_MUX* NUM_SENSOR_PER_CHANNEL;
    status = I2C_Read_IT(i2c_channel_id, slave_addr[ address ], rcvData, sizeof( rcvData ) );
    if ( status != STATUSINT_SUCCESS )
    {
        channel_pending_sensor_id[i2c_channel_id] = CHANNEL_NO_PENDING_SENSOR;
    }
    
    return status;
}


/**
  * @brief: gets internal temperature using internal temperature sensor
  * @param[out] *temp: temperature in Celcius if successful.
  * @note: IF THE RETURNED VALUE IS UNREASONABLE, CHECK THAT VCC IS 3.3 V.
  *        THE EXACT VALUE OF VCC IS VERY VERY CRITICAL!!! IF NOT, CHANGE VCC VALUE.
  * @return: StatusInt
  */

StatusInt temperature_GetInt( float *temp )
{
    StatusInt status;
    uint32_t vsense;

    /* Calculate slope based on calibration values */
    float slope = ( *( (uint16_t*)TEMPERATURE_REG_CAL110 ) - *( (uint16_t*)TEMPERATURE_REG_CAL30 ) ) / ( 110.0 - 30.0 );

    /* Get ADC value */
    status = ADC_Read( &vsense, &vsense, ADC_TEMPERATURE );
    if ( status == STATUSINT_SUCCESS ) {
        /* Calculate temperature based on linearity */
        *temp = ( vsense - *( (uint16_t*)TEMPERATURE_REG_CAL30 ) ) / slope + (float)30.0;
    }

    return status;
} 

/*
 * @brief return i2c_channel_status
 * @param[in] i2c_chanel that is requested
 * @return I2C_CHANNEL_INACTIVE if not used for temperature monitoring;
 * I2C_CHANNEL_TEMPERATURE if used for temperature monitoring.
 */
int temperature_get_i2c_channel_status(uint8_t i2c_channel){
    return i2c_channel_status[i2c_channel];
}

/**
 * I2C IT callback function.
 * If receive a data, call registered temperature_rx_callback function with the
 * temperature and sensor ID. Note that temperature is in eighths of the
 * temperature in degree Celsus.
 * If fail to read from temp sensor, will return TEMP_I2C_ERROR.
 */
void temperature_monitor_I2C_callback(uint8_t i2c_channel, I2CInfoStruct_t* rx_info){
  uint32_t temp;
  if (rx_info->state == I2C_STATE_RECEIVE){
      temp = ( (int16_t)rx_info->data[ 0 ] << 8 ) | ( (int16_t)( rx_info->data[ 1 ] & 0xE0 ) );
      temp >>= 5;
  }else if(rx_info->state == I2C_STATE_ERROR){
      temp = TEMP_I2C_ERROR;
  }
    temperature_rx_callback(i2c_channel, channel_pending_sensor_id[i2c_channel], temp);
    channel_pending_sensor_id[i2c_channel] = CHANNEL_NO_PENDING_SENSOR;
}
