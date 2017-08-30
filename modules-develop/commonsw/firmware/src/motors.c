/*
 * Motor Controller Driver
 * Caltech Racing 2016
 */

/* Standard libraries */
#include <stdint.h>
#include <stdbool.h>
 
/* STM libraries */

/* RTX */
#include <cmsis_os.h>                   // ARM::CMSIS:RTOS:Keil RTX

/* FSAE Firmware */
#include "common.h"
#include "motors.h"
#include "can.h"
 
/* Local types */

/* Local constants */

/* Local variables */
volatile bool motor_connected;
volatile uint16_t speed;
volatile uint16_t current;
volatile uint16_t temperature;
volatile uint16_t error;
volatile uint32_t receive_time;

/* Local function prototypes */
static void motor_CAN_Handler( CANRxStruct_t* rx );
static void processMotorMessage(CANRxStruct_t* rx );

static StatusInt create_motor_msg( uint8_t reg, uint8_t* data, uint8_t datalen, CANTxStruct_t* tx );
static StatusInt motor_request_one_update( uint8_t reg );
static StatusInt motors_init( uint8_t interval );
static StatusInt update_motor_speed( uint16_t speed );
static StatusInt update_motor_torque( uint16_t torque );
static StatusInt motor_request_one_update( uint8_t reg );
static StatusInt motor_request_permanent_update( uint8_t reg, uint8_t freq );



/* 
 * Function implementations
 */

StatusInt Motor_GetTemperature( uint32_t *data ) 
{
    StatusInt retVal = STATUSINT_ERROR_BADARG;

    if ( temperature != NULL ) 
    {
        *data = temperature;
        retVal = STATUSINT_SUCCESS;
    }

    return retVal;
}

StatusInt Motor_GetCurrent( uint32_t *data ) 
{

    StatusInt retVal = STATUSINT_ERROR_BADARG;
    if ( current != NULL )
    {
        *data = current;
        retVal = STATUSINT_SUCCESS;
    }

    return retVal;
}

StatusInt Motor_GetSpeed( uint32_t *data ) 
{
    StatusInt retVal = STATUSINT_ERROR_BADARG;

    if ( speed != NULL )
    {
        *data = speed;
        retVal = STATUSINT_SUCCESS;
    }

    return retVal;
}

StatusInt Motor_GetVehicleSpeed( uint32_t *data ) 
{
    StatusInt retVal = STATUSINT_ERROR_BADARG;

    if ( speed != NULL )
    {
        *data = (uint32_t)( WHEEL_CIRCUMFRENCE * speed / GEAR_RATIO * MMPM_TO_MPH );
        retVal = STATUSINT_SUCCESS;
    }

    return retVal;
}

StatusInt Motor_GetError( uint32_t *data ) 
{
    StatusInt retVal = STATUSINT_ERROR_BADARG;

    if ( error != NULL )
    {
        *data = error;
        retVal = STATUSINT_SUCCESS;
    }

    return retVal;
}

StatusInt Motor_GetReceiveTime( uint32_t *data ) 
{
    StatusInt retVal = STATUSINT_ERROR_BADARG;

    if ( receive_time != NULL )
    {
        *data = receive_time;
        retVal = STATUSINT_SUCCESS;
    }

    return retVal;
}

StatusInt Motor_SetSpeed( uint32_t setting ) 
{
    StatusInt retVal = STATUSINT_ERROR_BADARG;

    setting = setting * DIRECTION_REVERSE;

    if ( setting <= MOTOR_SPEED_CMD_MAX ) 
    {
        retVal = update_motor_speed( (uint16_t) setting );
    }

    return retVal;
}

StatusInt Motor_SetTorque( uint32_t setting ) 
{
    StatusInt retVal = STATUSINT_ERROR_BADARG;

    if ( setting <= MOTOR_SPEED_CMD_MAX ) 
    {
        retVal = update_motor_torque( (uint16_t) setting );
    }

    return retVal;
}

StatusInt Motor_Init( uint8_t interval ) 
{
    StatusInt retVal = STATUSINT_ERROR_BADARG;

    retVal = motors_init( interval );
    osDelay(100);
    if (retVal == STATUSINT_SUCCESS ) {
        retVal = motor_request_one_update(REG_MOTOR_SPEED);
    }

    return retVal;
}

StatusInt Motor_Enable( void )
{
    StatusInt status;
    uint8_t msg[2];
    CANTxStruct_t tx;
    msg[ 0 ] = 0;
    msg[ 1 ] = 0;
    status = create_motor_msg( REG_MODE, msg, sizeof( msg ), &tx );
    
    if ( status == STATUSINT_SUCCESS )
    {
        status = CAN_SendMessage( &tx, CAN_CHANNEL_MOTORS );
    }

    return status;
}

StatusInt Motor_Disable( void )
{
    StatusInt status;
    uint8_t msg[2];
    CANTxStruct_t tx;
    msg[ 0 ] = 0x04;
    msg[ 1 ] = 0;
    status = create_motor_msg( REG_MODE, msg, sizeof( msg ), &tx );

    if ( status == STATUSINT_SUCCESS )
    {
        status = CAN_SendMessage( &tx, CAN_CHANNEL_MOTORS );
    }

    return status;
}


/*
 * Local
 */


/**
 * Creates a motor controller CAN message from the given params. 
 */
static StatusInt create_motor_msg( uint8_t reg, uint8_t* data, uint8_t datalen, CANTxStruct_t* tx ) 
{
    uint8_t i;
    StatusInt status = STATUSINT_SUCCESS;

    if ( ( datalen + 1 ) > 8 )
    {
        status = STATUSINT_ERROR_BADARG;
    }

    if ( status == STATUSINT_SUCCESS )
    {
        tx->ID = TRANSMIT_ADDRESS;
        tx->length = datalen + 1;
        tx->data[ 0 ] = reg;

        for ( i = 0; i < datalen; i++ ) 
        {
            tx->data[ i + 1 ] = data[ i ];
        }
    }

    return status;
}

/**
 * A callback registered to recieve motor controller messages. Updates 
 * the appropriate motor controller variable. 
 */
static void motor_CAN_Handler( CANRxStruct_t* rx ) 
{
    processMotorMessage(rx);
    //TODO Update this to actually set time properly
    //getTimeInInteger(&receive_time);

}

static void processMotorMessage(CANRxStruct_t* rx ) {
     uint8_t type = rx->data[ 0 ];

    if ( rx != NULL )
    {
        if ( rx->length >= 1 )
        {
            switch( type ) 
            {
                case ( REG_STATE ):
                    motor_connected = true;
                    break;
                case ( REG_MODE ):
                    /* Do nothing */
                    break;
                case ( REG_MOTOR_SPEED ):
                    speed = MOTOR_N_MAX * DIRECTION_REVERSE * (( rx->data[ 2 ] << 8 ) + rx->data[ 1 ]) / MOTOR_SPEED_CMD_MAX;
                    break;
                case ( REG_ERROR ):
                    error = ( rx->data[ 2 ] << 8 ) + rx->data[ 1 ];
                    break;
                case ( REG_MOTOR_CURRENT ):
                    //TODO: Calculate motor current
                    current = ( rx->data[ 2 ] << 8 ) + rx->data[ 1 ];
                    break;
                case ( REG_MOTOR_TEMP ):
                    //TODO: Calculate motor temp
                    temperature = ( rx->data[ 2 ] << 8 ) + rx->data[ 1 ];
                    break;
                default:
                    break;      
            }
        }
    }   
}

/**
 * Initializes the motor controller, check that it is able to 
 * send messages, and then requests periodic sending of 
 * motor speed, current, temperature, and RPM. Return STATUSINT_SUCCESS if successful.
 * 
 * @param interval number of ms between each motor controller update.
 */
static StatusInt motors_init( uint8_t interval )
{
    StatusInt status;
    osStatus os_status;
    CANTxStruct_t tx;

    /* Initialize motor CAN */
    status = CAN_Init( CAN_CHANNEL_MOTORS );
    
    /* Install callbacks */
    if ( status == STATUSINT_SUCCESS )
    {
        CAN_RegisterCallback( CAN_CHANNEL_MOTORS, motor_CAN_Handler );
    }
    
    /* Initialize shared variables */
    if ( status == STATUSINT_SUCCESS )
    {
        speed = MOTOR_SPEED_CMD_MIN;
        temperature = 0;
        current = 0;
        error = ERROR_NONE;
    }

    /* Make sure we can receive messages from the motor controller */
    if ( status == STATUSINT_SUCCESS ) {
        status = motor_request_one_update( REG_STATE );
    }

    // Wait for the response to be received
    /* TODO: Should use signaling. (CC-65) */
    while ( motor_connected == false ) 
    {
        osThreadYield();
    }

    /* Ensure motor is disabled */
    if ( status == STATUSINT_SUCCESS )
    {
        status = Motor_Disable();
    }

    /* Ensure motor is stopped. */
    if ( status == STATUSINT_SUCCESS )
    {
        status = update_motor_speed( MOTOR_SPEED_CMD_MIN );
    }
    
    if ( status == STATUSINT_SUCCESS )
    {
        status = motor_request_permanent_update( REG_MOTOR_SPEED, interval );
    }

    if ( status == STATUSINT_SUCCESS )
    {
        status = motor_request_permanent_update( REG_ERROR, interval );
    }

    if ( status == STATUSINT_SUCCESS )
    {
        status = motor_request_permanent_update( REG_MOTOR_TEMP, interval );
    }

    if ( status == STATUSINT_SUCCESS )
    {
        status = motor_request_permanent_update( REG_MOTOR_CURRENT, interval );
    }

    return status;
}

static StatusInt update_motor_speed( uint16_t new_speed )
{
    StatusInt status;
    uint8_t msg[2];
    CANTxStruct_t tx;
    msg[ 0 ] = lobyte( new_speed );
    msg[ 1 ] = hibyte( new_speed );
    status = create_motor_msg( REG_SPEED_CMD, msg, sizeof( msg ), &tx );

    if ( status == STATUSINT_SUCCESS )
    {
        status = CAN_SendMessage( &tx, CAN_CHANNEL_MOTORS );
    }

    return status;
}

static StatusInt update_motor_torque( uint16_t new_torque )
{
    StatusInt status;
    uint8_t msg[2];
    CANTxStruct_t tx;
    msg[ 0 ] = lobyte( new_torque );
    msg[ 1 ] = hibyte( new_torque );
    status = create_motor_msg( REG_TORQUE_CMD, msg, sizeof( msg ), &tx );

    if ( status == STATUSINT_SUCCESS )
    {
        status = CAN_SendMessage( &tx, CAN_CHANNEL_MOTORS );
    }

    return status;
}

/** Requests a single update from the given register. */
static StatusInt motor_request_one_update( uint8_t reg )
{
    StatusInt status;
    uint8_t msg[ 2 ];
    CANTxStruct_t tx;
    msg[ 0 ] = reg;
    msg[ 1 ] = 0;
    status = create_motor_msg( REG_TRANSMIT, msg, sizeof( msg ), &tx );
    if ( status == STATUSINT_SUCCESS )
    {
        status = CAN_SendMessage( &tx, CAN_CHANNEL_MOTORS );
    }
    return status;
}

/** Request updates for the given register to be sent every interval milliseconds. */
static StatusInt motor_request_permanent_update( uint8_t reg, uint8_t interval ) 
{
    StatusInt status;
    uint8_t msg[ 2 ];
    CANTxStruct_t tx;
    msg[ 0 ] = reg;
    msg[ 1 ] = interval;
    status = create_motor_msg( REG_TRANSMIT, msg, sizeof( msg ), &tx );
    if ( status == STATUSINT_SUCCESS )
    {
        status = CAN_SendMessage( &tx, CAN_CHANNEL_MOTORS );
    }
    return status;
}
