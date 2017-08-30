/*
 * IO Board Specific Firmware
 * Caltech Racing 2016
 */

#ifndef BOARDS_IO_H
#define BOARDS_IO_H

#include "common.h" /* StatusInt */

/* Initializes readers */
StatusInt IO_InitBrake( void );
StatusInt IO_InitThrottle( void );
StatusInt IO_InitSteering( void );
 
/**********************************************************************
 *                  Driver Control Functions                          *
 **********************************************************************/
StatusInt IO_GetThrottle( uint32_t *value1, uint32_t *value2 );
StatusInt IO_GetBrake( uint32_t *value1, uint32_t *value2 );
StatusInt IO_GetSteering( uint32_t *value1, uint32_t *value2 );

/**********************************************************************
 *                  Light Indicator Functions                         *
 **********************************************************************/
StatusInt setBrakeLight(int lightStatus);
StatusInt getTSActiveLightStatus(int *lightStatus);
StatusInt getBrakeLightStatus(int *lightStatus);

#endif /* BOARDS_IO_H */
