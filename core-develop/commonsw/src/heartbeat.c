/*
 * Heartbeat thread
 * Caltech Racing 2016
 */
 /* TODO: Make heartbeats run on a single thread. (CS-67) */

/* Standard libraries */
#include <string.h>
#ifdef DEBUG
#include <stdio.h>
#endif

/* RTX */
#include <cmsis_os.h>

/* FSAE Firmware */
#include <common.h>
#include <addresses.h>

/* Software */
#include "messaging.h"
#include "messages.h"
#include "heartbeat.h"
#ifdef DEBUG
#include <retarget.h>
#endif

extern void triggerHeartbeatFault();

/* Local types */

/* Local constants */

/* Local variables */
static uint8_t heartbeats[ N_CAN_DEVICES ];
static uint8_t beating[ N_CAN_DEVICES ];
static bool running;


static osMutexId heartbeatMutex;
osMutexDef( heartbeatMutex );
static osThreadId heartbeatDecrementThread;

/* Local function prototypes */
static void heartbeat_decrement ( void const * );
bool currently_missed_heartbeats( void );
static void on_Message( uint8_t sender, uint16_t key, uint32_t value );

osThreadDef( heartbeat_decrement, osPriorityNormal, 1, 0 );


/*
 * Function implementations
 */
 
StatusInt Init_Heartbeat( void )
{
    uint8_t i;
    StatusInt status = STATUSINT_SUCCESS;
    // Set each value in heartbeats to 3. The value should always be
    // between 0 and 3. If possible, recieving a message from a system
    // will increase their heartbeat count by 1. Every 1/9th of a second,
    // the heartbeat count of all systems will decrease by 1. This means
    // that if 2-3 heartbeat messages are skipped in a row, the value will
    // hit 0 and the system will shut down
    for ( i = 0; i < N_CAN_DEVICES; i++ )
    {
        heartbeats[ i ] = MAX_MISSES;
    }

    memset( beating, (uint8_t)false, N_CAN_DEVICES );

    if ( status == STATUSINT_SUCCESS ) {
        heartbeatMutex = osMutexCreate( osMutex( heartbeatMutex ) );
        if ( heartbeatMutex == NULL ) {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    if ( status == STATUSINT_SUCCESS ) {
        running = true;
        status = register_callback_int( MSG_T_HEART, on_Message );
    }


    if ( status == STATUSINT_SUCCESS ) {
        heartbeatDecrementThread = osThreadCreate( osThread( heartbeat_decrement ), NULL );
        if ( heartbeatDecrementThread == NULL) {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    return status;
}

StatusInt Heartbeat_Fault() {
    StatusInt retVal = STATUSINT_SUCCESS;
    int32_t signal_status;

    signal_status = osSignalSet( heartbeatDecrementThread, (int)SIGNAL_FAULT );
    if ( signal_status != SIGNAL_SET_FAIL ) {
        retVal = STATUSINT_ERROR_BADARG;
    }

    return retVal;
}

StatusInt Heartbeat_ClearFault() {
    StatusInt retVal = STATUSINT_SUCCESS;
    int32_t signal_status;

    signal_status = osSignalClear( heartbeatDecrementThread, (int)SIGNAL_FAULT );
    if ( signal_status == SIGNAL_SET_FAIL ) {
        retVal = STATUSINT_ERROR_BADARG;
    }

    signal_status = osSignalSet( heartbeatDecrementThread, (int)SIGNAL_CLEAR_FAULT );
    if ( signal_status != SIGNAL_SET_FAIL ) {
        retVal = STATUSINT_ERROR_BADARG;
    }

    return retVal;
}

// Decrements the heartbeat count every 1/9th of a second
// Kills the system if the heartbeat count of anything reaches 0.
static void heartbeat_decrement( void const * arg )
{
    StatusInt status;
    uint8_t i;
    osEvent signal_event;
    osStatus os_status;
    while( running == true )
    {
        while ( (signal_event = osSignalWait((int)SIGNAL_FAULT, 0)).status == osOK) {
            os_status = osMutexWait( heartbeatMutex, 0 );
            if ( os_status != osOK ) {
                return; /* This stops sending heartbeats. Great */
            }

            for ( i = 0; i < N_CAN_DEVICES; i++ )
            {
                if ( beating[ i ] == true )
                {
                    heartbeats[ i ]--;
#ifdef DEBUG
                    fputc( '-', stddebug );
#endif
                    if ( heartbeats[ i ] == HB_WARNING_THRESHOLD )
                    {
                        status = send_int( ADDRESS_BROADCAST,
                                           MSG_P_WARNING,
                                           MSG_T_WARNING,
                                           MSG_I_NONE,
                                           MSG_V_WARN_SLOWHEART );
#ifdef DEBUG
                        fprintf( stddebug, "Slow hearbeat warning: %d", i );
#endif
                        if ( status != STATUSINT_SUCCESS )
                        {
                            return; /* This stops sending heartbeats. Great */
                        }
                    }
                    else if ( heartbeats[ i ] == 0 )
                    {
                        status = send_int( ADDRESS_BROADCAST,
                                           MSG_P_FAULT,
                                           MSG_T_FAULT,
                                           MSG_I_NONE,
                                           MSG_V_FAULT_HEART );
                        running = false;
                        triggerHeartbeatFault();
#ifdef DEBUG
                        fprintf( stddebug, "Died due to heartbeat %d\n", i );
#endif
                    }
                }
            }
            os_status = osMutexRelease( heartbeatMutex );
            if ( os_status != osOK ) {
                return; /* This stops sending heartbeats. Great */
            }

            status = send_int( ADDRESS_BROADCAST,
                      MSG_P_HEART,
                      MSG_T_HEART,
                      MSG_I_NONE,
                      MSG_V_MAGIC_HEART );
            if ( status != STATUSINT_SUCCESS ) {
                return; /* This stops sending heartbeats. Great */
            }

            os_status = osDelay( HEARTBEAT_SEND_DELAY );
            if ( os_status != osEventTimeout ) {
                return; /* This stops sending heartbeats. Great */
            }
        }

        if (signal_event.status == osEventSignal) {
            signal_event = osSignalWait( (int)SIGNAL_CLEAR_FAULT, osWaitForever );
            if ( signal_event.status != osOK ) {
                return; /* This stops sending heartbeats. Great */
            }
        } else if (signal_event.status == osErrorValue) {
            // Occurs if signal is > (1<<8)
            // We can assume that this never happens
            // TODO: Process error (CS-98)
            return;
        } else if (signal_event.status == osErrorISR) {
            // Occurs if we are inside of an interrupt service routine
            // We can assume that this never happens
            // TODO: Process error (CS-98)
            return;
        }
    }
}


// When any message is recieved (heartbeat or not)
// add one to the heartbeat count for the system sending
// the message
static void on_Message( uint8_t sender, uint16_t key, uint32_t value )
{
    StatusInt status;
    osStatus os_status;
    if ( sender > N_CAN_DEVICES ) {
        return; /* This stops sending heartbeats. Great */
    }

    if ( value != MSG_V_MAGIC_HEART )
    {
        status = send_int( ADDRESS_BROADCAST,
                  MSG_P_WARNING,
                  MSG_T_WARNING,
                  MSG_I_NONE,
                  MSG_V_WARN_BADHEART );

        if ( status != STATUSINT_SUCCESS ) {
            return; /* This stops sending heartbeats. Great */
        }
    }

    os_status = osMutexWait( heartbeatMutex, 0 );
    if ( os_status != osOK ) {
        return; /* This stops sending heartbeats. Great */
    }

    beating[ sender - 1 ] = true;
    heartbeats[ sender - 1 ] = MAX_MISSES;

    os_status = osMutexRelease( heartbeatMutex );
    if ( os_status != osOK ) {
        return; /* This stops sending heartbeats. Great */
    }

#ifdef DEBUG
    fputc( '+', stddebug );
#endif
}

bool currently_missed_heartbeats() {
    uint8_t i;
    for ( i = 0; i < N_CAN_DEVICES; i++ ) {
        if ( beating[ i ] == true && heartbeats[ i ] == 0 ) {
            return true;
        }
    }
    return false;
}

