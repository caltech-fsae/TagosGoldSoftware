#ifndef CORE_H
#define CORE_H

#include <common.h>

/* States */
enum states {
    CORE_STATE_IDLE,
    CORE_STATE_DRIVING,
    CORE_STATE_FAULT,
    CORE_STATE_LIMP,
};

#define MOTOR_UPDATE_SPEED  10  /* ms per loop */
#define MOTOR_MAX_TORQUE    100
#define MOTOR_LIMP_TORQUE   10

#define BRAKE_ON 0
#define BRAKE_OFF !BRAKE_ON
#define BUTTON_RELEASED 0
#define BUTTON_PRESSED !BUTTON_RELEASED

#define IO_UPDATE_LIFE_EXPECTANCY 500000 /* maximum age in microseconds */
#define CORE_MONITORING_DELAY 50

#define RTDS_LENGTH 2000

#define BRAKE_THRESHOLD_LIGHT 0.2 /* TODO */
#define BRAKE_THRESHOLD_HARD 0.8 /* TODO */
#define THROTTLE_THRESHOLD 0.3 /* TODO */


StatusInt core_Init( void );

#endif

