/*
 * Internal messaging library
 * Caltech Racing 2016
 */

/* TODO: update documentation.
    send_message_int: send a message with a value of type int to another board.
    send_message_float: send a message with a value of type float to another board.
    register_callback_int : associates a callback function with a message_type so this function
        is called when that message is recieved (when expecting an int value).
    register_callback_int : associates a callback function with a message_type so this function
        is called when that message is recieved (when expecting a float value).
*/

/* Standard libraries */
#include <string.h>
#include <stdio.h> 

/* RTX */

/* FSAE Firmware */
#include <common.h>
#include <can.h>

/* Software */
#include "messages.h"
#include "messaging.h"

/* Local types */
union data_store
{
    uint32_t i[ CAN_MESSAGE_LENGTH / sizeof( uint32_t ) ];
    uint8_t c[ CAN_MESSAGE_LENGTH ];
    float f[ CAN_MESSAGE_LENGTH / sizeof( float ) ];
};

/* Local constants */

/* Local variables */
// store callback functions as an array of function pointers
// each message type can map to up to N_MAX_FUNCTION_PER_CALLBACK functions
static void ( *callbacks_int[ N_MESSAGE_TYPES ][ N_MAX_FUNCTION_PER_CALLBACK ] )( uint8_t sender, uint16_t key, uint32_t val );
static void ( *callbacks_float[ N_MESSAGE_TYPES ][ N_MAX_FUNCTION_PER_CALLBACK ] )( uint8_t sender, uint16_t key, float val );
static void ( *callbacks_int_all[ N_MAX_FUNCTION_PER_CALLBACK ] )( uint16_t type, uint8_t sender, uint16_t key, uint32_t val );
static void ( *callbacks_float_all[ N_MAX_FUNCTION_PER_CALLBACK ] )( uint16_t type, uint8_t sender, uint16_t key, float val );

/* Local function prototypes */
static void recieved_message( CANRxStruct_t* rx_info );



/* 
 * Function implementations
 */

/**
    Called when a message gets sent to this board. Firmware calls this.
    body: stores serialized 64 bits of information (2 consecutive ints)
*/
static void recieved_message( CANRxStruct_t* rx_info )
{
    uint8_t i;
    uint16_t message_type;
    uint16_t key;
    bool int_notfloat;
    uint32_t val_i;
    float val_f;
    uint8_t sender;

    union data_store *data = (union data_store*)(rx_info->data);

    /* Extract data from message header */
    key = ( data->i[ 0 ] & MESSAGE_ID_MASK ) >> MESSAGE_ID_SHIFT;
    message_type = ( data->i[ 0 ] & MESSAGE_TYPE_MASK ) >> MESSAGE_TYPE_SHIFT;
    sender = ( data->i[ 0 ] & MESSAGE_SENDER_MASK ) >> MESSAGE_SENDER_SHIFT;

    if ( message_type >= N_MESSAGE_TYPES ) {
        // TODO: Process Error (CS-98)
        return;
    }

    /* TODO: Float support. (CS-68) */
    int_notfloat = true;

    if ( int_notfloat == true )
    {
        /* It's an integer */
        val_i = data->i[ 1 ];

        /* Call all the callbacks */
        for ( i = 0; i < N_MAX_FUNCTION_PER_CALLBACK; i++ ) 
        {
            if ( callbacks_int[ message_type ][ i ] != NULL ) 
            {
                callbacks_int[ message_type ][ i ]( sender, key, val_i );
            }
            else
            {
                break;
            }

        }
        for ( i = 0; i < N_MAX_FUNCTION_PER_CALLBACK; i++ )
        {
            if ( callbacks_int_all[ i ] != NULL )
            {
                callbacks_int_all[ i ]( message_type, sender, key, val_i );
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        /* It's a float */
        val_f = data->f[ 1 ];

        /* Call all the callbacks */
        for ( i = 0; i < N_MAX_FUNCTION_PER_CALLBACK; i++ )
        {
            if ( callbacks_float[ message_type ][ i ] != NULL )
            {
                callbacks_float[ message_type ][ i ]( sender, key, val_f );
            }
            else
            {
                break;
            }

        }
        for ( i = 0; i < N_MAX_FUNCTION_PER_CALLBACK; i++ )
        {
            if ( callbacks_float_all[ i ] != NULL )
            {
                callbacks_float_all[ i ]( message_type, sender, key, val_f );
            }
            else
            {
                break;
            }
        }
    }
}

StatusInt init_messaging( void )
{
    StatusInt status = STATUSINT_SUCCESS;
    if ( status == STATUSINT_SUCCESS )
    {
        CAN_RegisterCallback( CAN_CHANNEL_MSG, recieved_message );
    }

    return status;
}


/**
    Whenever a message of type message_type is recieved, the callback function will be called
    with a dictionary of key-value pairs as the parameter.
    message_type: in [0,255] , when a message is passed with this type, the corresponding
        function will be called
    callback: this is the function to be called when a message of type message_type is recieved,
        function should take as a parameter a hashTableChrInt pointer
    return Nothing
*/
StatusInt register_callback_int( uint16_t message_type, void ( *callback )( uint8_t sender, uint16_t id, uint32_t val ) )
{
    uint8_t i;
    StatusInt status = STATUSINT_ERROR_GENERAL;

    for ( i = 0; i < N_MAX_FUNCTION_PER_CALLBACK; i++ )
    {
        if ( callbacks_int[ message_type ][ i ] == NULL )
        {
            callbacks_int[ message_type ][ i ] = *callback;
            status = STATUSINT_SUCCESS;
            break;
        }
    }

    return status;
}

/**
    Whenever a message of type message_type is recieved, the callback function will be called
    with a dictionary of key-value pairs as the parameter.
    message_type: in [0,255] , when a message is passed with this type, the corresponding
        function will be called
    callback: this is the function to be called when a message of type message_type is recieved,
        function should take as a parameter a hashTableChrInt pointer
    return Nothing
*/
StatusInt register_callback_float( uint16_t message_type, void ( *callback )( uint8_t sender, uint16_t id, float val ) )
{
    uint8_t i;
    StatusInt status = STATUSINT_ERROR_GENERAL;

    for( i = 0; i < N_MAX_FUNCTION_PER_CALLBACK; i++ )
    {
        if ( callbacks_float[ message_type ][ i ] == NULL )
        {
            callbacks_float[ message_type ][ i ] = *callback;
            status = STATUSINT_SUCCESS;
            break;
        }
    }

    return status;
}

StatusInt register_callback_int_all( void ( *callback )( uint16_t type, uint8_t sender, uint16_t id, uint32_t val ) )
{
    uint8_t i;
    StatusInt status = STATUSINT_ERROR_GENERAL;

    for( i = 0; i < N_MAX_FUNCTION_PER_CALLBACK; i++ )
    {
        if ( callbacks_int_all[ i ] == NULL )
        {
            callbacks_int_all[ i ] = *callback;
            status = STATUSINT_SUCCESS;
            break;
        }
    }

    return status;
}

StatusInt register_callback_float_all( void ( *callback )( uint16_t type, uint8_t sender, uint16_t id, float val ) )
{
    uint8_t i;
    StatusInt status = STATUSINT_ERROR_GENERAL;

    for( i = 0; i < N_MAX_FUNCTION_PER_CALLBACK; i++ )
    {
        if ( callbacks_float_all[ i ] == NULL )
        {
            callbacks_float_all[ i ] = *callback;
            status = STATUSINT_SUCCESS;
            break;
        }
    }

    return status;
}

/**
    Send a message with int value to another board.

    destination: in [0,N_DESTINATIONS] , determines which board we send the message to
    priority: in [0,N_PRIORITIES], more critical messages should be higher priority since
        they will be sent before low priority messages
    message_type: in [0,N_MESSAGE_TYPES] , callbacks are called based on what message_type is recieved
    returns nothing
*/
StatusInt send_int( uint8_t destination,
                    uint16_t priority,
                    uint16_t type,
                    uint16_t id,
                    uint32_t val )
{
    uint32_t header;
    union data_store data;
    CANTxStruct_t msg;
    StatusInt status = STATUSINT_SUCCESS;

    if ( status == STATUSINT_SUCCESS ) {
        if ((destination >= N_DESTINATIONS) ||
            (priority >= N_PRIORITIES)) {
            status = STATUSINT_ERROR_BADARG;
        }
    }

    if ( status == STATUSINT_SUCCESS ) {
        header = ( priority << HEAD_PRIORITY_SHIFT ) | ( MSG_S_SELF << HEAD_SENDER_SHIFT ) | ( destination & HEAD_DESTINATION_SHIFT );

        data.i[ 0 ] = ( id << MESSAGE_ID_SHIFT ) | ( type << MESSAGE_TYPE_SHIFT) | ( MSG_S_SELF << MESSAGE_SENDER_SHIFT );
        data.i[ 1 ] = val;

        /* Actually send the message */
        msg.ID = header;
        memcpy( msg.data, data.c, sizeof( data ) );
        msg.length = sizeof( data );
        status = CAN_SendMessage( &msg, CAN_CHANNEL_MSG );
    }

    return status;
}


/**
    Send a message with float value to another board.

    destination: in [0,N_DESTINATIONS] , determines which board we send the message to
    priority: in [0,N_PRIORITIES], more critical messages should be higher priority since
        they will be sent before low priority messages
    message_type: in [0,N_MESSAGE_TYPES] , callbacks are called based on what message_type is recieved
    returns nothing
*/
StatusInt send_float( uint8_t destination,
                      uint16_t priority,
                      uint16_t type,
                      uint16_t id,
                      float val )
{
    return send_int( destination,
                     priority,
                     type,
                     id,
                     *( (uint32_t *) &val ) );
}
