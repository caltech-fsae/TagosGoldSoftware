/*
 * Core Board Specific Firmware
 * Caltech Racing 2016
 */
#ifndef BOARDS_CORE_H
#define BOARDS_CORE_H

#include "common.h" /* StatusInt */
 
/**********************************************************************
 *                  Other Core Functions                              *
 **********************************************************************/
StatusInt writeDisplay( uint32_t *displayValues, uint32_t size );

#endif /* BOARDS_CORE_H */
