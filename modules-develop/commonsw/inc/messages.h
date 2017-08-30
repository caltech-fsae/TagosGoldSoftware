/*
 * messages.h
 *
 * Message types, IDs, and values
 * Caltech Racing 2016
 *
 * This file contains a complete list of all the message types and unique
 * identifiers for the messages flowing around the vehicle, as well as the
 * value identifiers for special messages and descriptions of the messages'
 * contents.
 */
 
#ifndef MESSAGES_H
#define MESSAGES_H

#include <addresses.h>

/*
 * Message processing masks
 */
#define MESSAGE_TYPE_MASK    0xFFFF0000  /* Type is top 16 bits of uint32 identifier */
#define MESSAGE_TYPE_SHIFT   16
#define MESSAGE_TYPE_BITS    16
#define MESSAGE_SENDER_MASK  0x0000F000  /* Sender is middle 4 bits */
#define MESSAGE_SENDER_SHIFT 12
#define MESSAGE_SENDER_BITS  4
#define MESSAGE_ID_MASK      0x00000FFF  /* Unique ID is bottom 12 bits */
#define MESSAGE_ID_SHIFT     0
#define MESSAGE_ID_BITS      12

/*
 * Message types 
 */
#define MSG_T_FAULT         1   /* Shutdown fault triggered. ID: none. Value: fault id (int). */
#define MSG_T_WARNING       2   /* Warning. ID: none. Value: warning id (int). */
#define MSG_T_TEMP_CELL     3   /* Cell temperature. ID: cell (sequential). Value: degrees C (float). */
#define MSG_T_TEMP_GEN      4   /* General temperature. ID: sensor. Value: degrees C (float). */
#define MSG_T_VOLTAGE_MAX   5   /* Maximum cell voltage. ID: none. Value: degrees C (float). */
#define MSG_T_VOLTAGE_MIN   6   /* Minimum cell voltage. ID: none. Value: degrees C (float). */
#define MSG_T_VOLTAGE_NMAX  7   /* # cells @ max voltage. ID: none. Value: # (int). */
#define MSG_T_VOLTAGE_NMIN  8   /* # cells @ min voltage. ID: none. Value: # (int). */
#define MSG_T_VOLTAGE_AVG   9   /* Average cell voltage. ID: none. Value: degrees C (float). */
#define MSG_T_THROTTLE      10  /* Throttle position. ID: none. Value: 00xFF00 (int). */
#define MSG_T_BRAKE         11  /* Brake position. ID: none. Value: 00xFF00 (int). */
#define MSG_T_STEER         12  /* Steering wheel position. ID: none. Value: 0-200 (int). */
#define MSG_T_IMU_X         13  /* X-axis acceleration. ID: none. Value: g (float). */
#define MSG_T_IMU_Y         14  /* Y-axis acceleration. ID: none. Value: g (float). */
#define MSG_T_IMU_Z         15  /* Z-axis acceleration. ID: none. Value: g (float). */
#define MSG_T_IMU_ROLL      16  /* Gyroscope roll. ID: none. Value: degrees (float). */
#define MSG_T_IMU_PITCH     17  /* Gyroscope pitch. ID: none. Value: degrees (float). */
#define MSG_T_IMU_YAW       18  /* Gyroscope yaw. ID: none. Value: degrees (float). */
#define MSG_T_MOTOR_RPM     19  /* Motor RPM. ID: none. Value: # (int). */
#define MSG_T_MOTOR_TORQUE  20  /* Motor torque. ID: none. Value: Nm (float). */
#define MSG_T_MOTOR_VOLT    21  /* Motor voltage ID: none. Value: V (float). */
#define MSG_T_TS_VOLT       22  /* Tractive system voltage. ID: none. Value: V (float). */
#define MSG_T_TS_CURRENT    23  /* Tractive system current. ID: none. Value: Nm (float). */
#define MSG_T_HEART         24  /* Heartbeat. ID: none. Value: none. */
#define MSG_T_STATUS        25  /* Status info. ID: none. Value: status id (int). */
#define MSG_T_TIME          26  /* Time synchronization. ID: none. Value: time_t (int). */
#define MSG_T_CONFIG        27  /* Parameter configuration. ID: parameter. Value: varies. */
#define MSG_T_FAULTN        28  /* Non-resettable fault triggered. ID: none. Value: fault id (int). */
#define MSG_T_INTERLOCK     29  /* Tells the core that the current fault can't be cleared 
                                   until interlock is closed. ID: none. Value: bool (int). */

#define N_MESSAGE_TYPES     30

/*
 * Message senders
 */
#define MSG_S_CORE          ADDRESS_CORE         /* Core board */
#define MSG_S_COMM          ADDRESS_COMM         /* Communications board */
#define MSG_S_SD            ADDRESS_SHUTDOWN     /* Shutdown board */
#define MSG_S_IO_PEDALS_1   ADDRESS_IO_PEDALS_1  /* Pedals IO board 1 */
#define MSG_S_IO_PEDALS_2   ADDRESS_IO_PEDALS_2  /* Pedals IO board 1 */
#define MSG_S_IO_ACCUM      ADDRESS_IO_ACCUM     /* Accumulator IO board */

#define MSG_S_SELF          ADDRESS_SELF

/*
 * Message unique identifiers
 */
#define MSG_I_NONE          0   /* ID not required */

/* General temperature sensors */
#define MSG_I_TEMP_CORE_EXT 1   /* Core board, external (board) */
#define MSG_I_TEMP_CORE_INT 2   /* Core board, internal (die) */
#define MSG_I_TEMP_COMM_EXT 3   /* Communications board */
#define MSG_I_TEMP_COMM_INT 4
#define MSG_I_TEMP_SD_EXT   5   /* Shutdown board */
#define MSG_I_TEMP_SD_INT   6
#define MSG_I_TEMP_IO1_EXT  7   /* Pedals IO board */
#define MSG_I_TEMP_IO1_INT  8
#define MSG_I_TEMP_IO2_EXT  9   /* Accumulator IO board */
#define MSG_I_TEMP_IO2_INT  10
#define MSG_I_TEMP_MOTOR    11  /* Motor controller */

/* Configurable parameters */
#define MSG_I_PARAM_REGEN   1   /* Regenerative breaking. Value: on/off (bool). */

/*
 * Message values
 */

/* Faults */
#define MSG_V_FAULT_IMD      1   /* Insulation Monitoring Device fault */
#define MSG_V_FAULT_BMS      2   /* Accumulator Management System fault */
#define MSG_V_FAULT_BSPD     3   /* Brake Sensor Plausibility Device fault */
#define MSG_V_FAULT_TEMP     4   /* Overtemperature fault */
#define MSG_V_FAULT_CURR     5   /* Overcurrent fault */
#define MSG_V_FAULT_HEART    6	 /* Missed heartbeat fault */
#define MSG_V_FAULT_SWR      7   /* Software resettable fault */
#define MSG_V_FAULT_SWNR     8   /* Software non-resettable fault */
#define MSG_V_FAULT_INIT     9   /* Initialization fault */

/* Interlock */

#define MSG_V_INTERLOCK_CONNECTED    0 /* Interlock reconnected */
#define MSG_V_INTERLOCK_DISCONNECTED 1 /* Interlock disconnected */

/* Warnings */
#define MSG_V_WARN_HITEMP    1   /* High temperature warning */
#define MSG_V_WARN_LOBATT    2   /* Low battery state of charge warning */
#define MSG_V_WARN_SLOWHEART 3   /* Missed heartbeat warning */
#define MSG_V_WARN_BADHEART  4   /* Bad heartbeat value warning */
#define MSG_V_WARN_BADCELL   5   /* Bad temperature sensor warning */

/* Status */
#define MSG_V_STATUS_R2D     1   /* Ready to drive! */
#define MSG_V_STATUS_STOP    2   /* Driving stopped */
#define MSG_V_STATUS_RESETD  3   /* Fault reset by driver */
#define MSG_V_STATUS_RESETM  4   /* Fault reset manually */

#define MSG_V_MAGIC_HEART   0xC0FFEEEE /* Checksum for heartbeat */

/*
 * Message priorities
 * 0-PRIORITY_LOWEST: 0 is highest priority, PRIORITY_LOWEST is lowest
 * Currently assigned for PRIORITY_LOWEST = 15
 */
#define MSG_P_FAULT         0
#define MSG_P_HEART         5
#define MSG_P_WARNING       10
#define MSG_P_INFO          12


#endif /* MESSAGES_H */
