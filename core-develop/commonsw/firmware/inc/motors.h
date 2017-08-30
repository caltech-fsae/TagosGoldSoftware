/*
 * Motor Controller Driver
 * Caltech Racing 2016
 */
#ifndef MOTORS_H
#define MOTORS_H

#include "common.h" /* StatusInt */
 
/* Configuration constants */
#define MOTOR_SPEED_CMD_MAX         32767
#define MOTOR_SPEED_CMD_MIN         0
#define MOTOR_TORQUE_CMD_MAX        32767
#define MOTOR_TORQUE_CMD_MIN        0
#define MOTOR_N_MAX                 5000 // max RPM as configured in motor controller TODO: Query motor controller for this param on startup

/* Vehicle Constants */
#define WHEEL_RADIUS            282.25 // mm
#define WHEEL_CIRCUMFRENCE      6.28318 * WHEEL_RADIUS // mm
#define MMPM_TO_MPH             0.00003728
#define GEAR_RATIO              5
#define DIRECTION_REVERSE       1     // This should only be 1 or -1, depending on installation on car


/* CAN Transmit and receive addresses */
#define TRANSMIT_ADDRESS        ((uint32_t) 0x210)
#define RECIEVE_ADDRESS         ((uint32_t) 0x190)

/* Register locations within the motor controller */
#define REG_SPEED_CMD           ((uint8_t) 0x31)
#define REG_TORQUE_CMD          ((uint8_t) 0x71)
#define REG_STATE               ((uint8_t) 0x40)
#define REG_MODE                ((uint8_t) 0x51)
#define REG_ERROR               ((uint8_t) 0x8F)
#define REG_MOTOR_SPEED         ((uint8_t) 0x30)
#define REG_MOTOR_TEMP          ((uint8_t) 0x49)
#define REG_MOTOR_CURRENT       ((uint8_t) 0x20)

#define REG_TRANSMIT            ((uint8_t) 0x3d)

/* Error codes */
#define ERROR_NONE              (0)
#define ERROR_BAD_PARAM         (1 << 0)
#define ERROR_POWER_FAULT       (1 << 1)
#define ERROR_CAN_TIMEOUT       (1 << 3)
#define ERROR_RESOLVER_SIGNAL   (1 << 4)
#define ERROR_POWER_VOLTAGE     (1 << 5)
#define ERROR_MOTOR_TEMP        (1 << 6)
#define ERROR_CURRENT_HIGH      (1 << 7)
#define ERROR_CURRENT_TOL       (1 << 8)
#define ERROR_CURRENT_OVER      (1 << 9)
#define ERROR_RACEAWAY          (1 << 10)
#define ERROR_CAN_INIT          (1 << 11)
#define ERROR_ADC_INIT          (1 << 12)
#define ERROR_ENCODER           (1 << 13)
#define ERROR_SOFTWARE          (1 << 14)
#define ERROR_BALLAST           (1 << 15)

/** 
 * Get the motor temperature in tenths of degree C.
 */
StatusInt motor_GetTemperature( uint32_t *temperature );

/** 
 * Get the motor current in units of tenths of A.
 */
StatusInt Motor_GetCurrent( uint32_t *current );

/** 
 * Get the motor speed in RPMs.
 */
StatusInt Motor_GetSpeed( uint32_t *speed );

/** 
 * Get the vehicle speed in miles per hour.
 */
StatusInt Motor_GetVehicleSpeed( uint32_t *speed );

/** 
 * Get the motor error value. This value can be ERROR_NONE, or it can
 * be the bitwise AND of any of the constants in Motors.h
 */
StatusInt Motor_GetError( uint32_t *error );

/** 
 * Get the amount of time that has passed since the system received
 * a CAN message from the motor controller. This can generally be 
 * used to check if the controller is alive and communicating.
 */
StatusInt Motor_GetReceiveTime( uint32_t *time );

/**
 * Sets the motor speed to the given value. 
 * MOTOR_SPEED_MAX is max speed and MOTO_SPEED_STOP is min speed.
 */
StatusInt Motor_SetSpeed( uint32_t setting );

/**
 *
 *
 */
StatusInt Motor_SetTorque( uint32_t setting );

/**
 * Initializes communication with the motor controller. Call this function 
 * before any other motors function is called. Also causes motor speed, 
 * temperature, current, error, and receive time to be refreshed every
 * interval milliseconds. 
 */
StatusInt Motor_Init( uint8_t interval );

/**
 *
 */
StatusInt Motor_Enable( void );

/**
 *
 */
StatusInt Motor_Disable( void );

#endif //MOTORS_H
