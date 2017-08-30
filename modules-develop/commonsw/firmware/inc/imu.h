/*
 * IMU Driver
 * Caltech Racing 2016
 */
#ifndef IMU_H
#define IMU_H

#include "common.h" /* StatusInt */


#define IMU_SDA0            1
#define IMU_ADDRESS         ( 0x6A + IMU_SDA0 )

/* 
 * IMU Values 
 */
/* IMU's device ID */
#define IMU_DEVICE_ID       0x69

#define IMU_ACCEL_CONFIG    0xA0

#define IMU_ACCEL_X_LO      0
#define IMU_ACCEL_X_HI      1
#define IMU_ACCEL_Y_LO      2
#define IMU_ACCEL_Y_HI      3
#define IMU_ACCEL_Z_LO      4
#define IMU_ACCEL_Z_HI      5

/*
 * IMU Registers
 */
/* WHO_AM_I register holds the ID of the device. we will use this
 * to verify that communication with device is solid. */
#define IMU_REG_ID          0x0F

/* CTRL1_XL register lets us configure settings for acceleration
 * measurement. */
#define IMU_REG_CTRL1_XL    0x10

/* From below register, we can read lower byte of X-acceleration info.
 * Next five consecutive registers hold higher byte of X, and then 
 * Y and Z and so on. */
#define IMU_REG_OUTX_L_XL   0x28


StatusInt IMU_Init( void );
StatusInt IMU_getAcceleration( int16_t *x, int16_t *y, int16_t *z );

#endif
