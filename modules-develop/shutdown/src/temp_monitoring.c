/*
 * Temperature Monitoring thread
 * Caltech Racing 2016
 */

// branch off MB-95 later
// check CS-79 for latest temp driver

/* Standard libraries */

/* RTX */
#include <cmsis_os.h>

/* FSAE Firmware */
#include <addresses.h>
#include <common.h>
#include <temperature.h>
#include <i2c.h>

/* Software */
#include <messaging.h>
#include <messages.h>

#include "temp_monitoring.h"

/* Local constants */
const int totalSensors = sizeof(temp_sensors)/sizeof(struct tempSensor); 

/* Local variables */
static uint8_t num_reads[ totalSensors ];
static uint8_t alive_sensors[ totalSensors ];
static uint16_t ext_temp[ totalSensors ];
static int aliveSensors;
static uint8_t cell_number;

static osThreadId tempMonitorThread;

/* Local function prototypes */
static void temp_monitor ( void const * );
osThreadDef( temp_monitor, osPriorityNormal, 1, 0 );



/*
 * Function implementations
 */
 
/* 
 * Initializes all temperature sensors across the battery pack. Tracks whether
 * each cell was properly initialized in an array. Starts a thread to monitor
 * the temperatures.
 */ 
StatusInt Init_TempMonitor( void )
{
    StatusInt status;
    status = I2C_Init( I2C_1 );
    if ( status == STATUSINT_SUCCESS )
    {
        int i;
        aliveSensors = totalSensors;
        for (i = 0; i < totalSensors; i++ ) {
            // Skip conflict temp sensor address
            if ( temp_sensors[ i ].address == I2C_CONFLICT_SENSOR_ADDRESS )
            {
                continue;
            }
            if ( temperature_InitExt(temp_sensors[ i ].i2c, temp_sensors[ i ].address, temp_sensors[ i ].mux ) == STATUSINT_SUCCESS ) {
                alive_sensors [ i ] = 1;
                num_reads[ i ] = 0;
            }
            else {
                return STATUSINT_ERROR_GENERAL;
            }
        }
    }
    if ( status == STATUSINT_SUCCESS ) {
        cell_number = 0;
        tempMonitorThread = osThreadCreate( osThread( temp_monitor ), NULL );
        if ( tempMonitorThread == NULL) {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    return status;
}


/* 
 * Monitors all temperature sensors across the battery pack. Computes some 
 * statistical data and sends it over CAN for logging. When the maximum 
 * temperature exceeds a low threshold, a message is sent to "limp" the 
 * car until the temperature returns to a safe level. If the maximum 
 * temperature exceeds a high threshold, a fault is triggered and the car is 
 * shut down.
 */
static void temp_monitor( void const * arg )
{
    StatusInt status;
    uint8_t cell_max = 0;
    uint16_t temp_max = 0;

    while ( 1 ) {
        if ( alive_sensors [ cell_number ] ) {
            // Read from a temperature sensor
            status = temperature_GetExtInteger( &ext_temp[ cell_number ], temp_sensors[ cell_number ].i2c, temp_sensors[ cell_number ].address, temp_sensors[ cell_number ].mux );
            if (status == STATUSINT_SUCCESS ) {
                num_reads[ cell_number ]++;
                // Check if the current temp exceeds the high threshold            
                if ( ext_temp[ cell_number ] >= TEMP_FAULT_THRESHOLD ) {
                    status = send_int( ADDRESS_CORE,
                                       MSG_P_FAULT,
                                       MSG_T_FAULT,
                                       MSG_I_NONE,
                                       MSG_V_FAULT_TEMP );
                    killSwitch( true );
                }
                if (status == STATUSINT_SUCCESS) {
                    // Check if the current temp exceeds the low threshold
                    if ( ext_temp[ cell_number ] >= TEMP_LIMP_THRESHOLD ) {
                        status = send_int( ADDRESS_CORE,
                                           MSG_P_WARNING,
                                           MSG_T_WARNING,
                                           MSG_I_NONE,
                                           MSG_V_WARN_HITEMP );
                    }
                }
                if ( ext_temp [ cell_number ] > temp_max ) {
                    cell_max = cell_number;
                    temp_max = ext_temp [ cell_number ];
                }
                if ( status == STATUSINT_SUCCESS ) {
                    // Send every 10th temp reading of the cell
                    if ( num_reads[ cell_number ] >= DATA_READ_PER_OUT ) {
                        status = send_int( ADDRESS_COMM,
                                             MSG_P_INFO,
                                             MSG_T_TEMP_CELL,
                                             cell_number,
                                             ext_temp[ cell_number ] ); 
                        num_reads[ cell_number ] = 0;            
                    }
                }
                if ( status == STATUSINT_SUCCESS ) {
                    if ( (int)cell_number == totalSensors - 1 ) {
                        // Send the maximum temp reading after cycling through all cells
                        status = send_int( ADDRESS_COMM,
                                             MSG_P_INFO,
                                             MSG_T_TEMP_CELL,
                                             cell_max,
                                             temp_max );
                        // Check if too many sensors are dead
                        if ((float)aliveSensors / totalSensors <= 0.3f) {
                            status = send_int( ADDRESS_CORE,
                                               MSG_P_FAULT,
                                               MSG_T_FAULT,
                                               MSG_I_NONE,
                                               MSG_V_FAULT_SWR );
                        }
                    }
                }
            }
            else {
                // Send warning that temp could not be read
                status = send_int( ADDRESS_CORE,
                                   MSG_P_WARNING,
                                   MSG_T_WARNING,
                                   cell_number,
                                   MSG_V_WARN_BADCELL );
                if ( status == STATUSINT_SUCCESS ) {
                    // Perform a I2C software reset
                    status = I2C_ErrorHandle( I2C_1 );
                }
                if ( status != STATUSINT_SUCCESS ) {
                    // Send fault if I2C reset failed
                    status = send_int( ADDRESS_CORE,
                                       MSG_P_FAULT,
                                       MSG_T_FAULT,
                                       MSG_I_NONE,
                                       MSG_V_FAULT_SWR );
                }                    
            }
        }
        cell_number = ( cell_number + 1 ) % totalSensors;
    }
}

