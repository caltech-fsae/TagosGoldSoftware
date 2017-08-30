/*
 * Heartbeat thread
 * Caltech Racing 2016
 */
#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <common.h>

#define MAX_MISSES            4
#define HB_WARNING_THRESHOLD  1
#define HEARTBEAT_SEND_DELAY  100      /* in ms */

StatusInt Init_Heartbeat( void );
StatusInt Heartbeat_Fault( void );
StatusInt Heartbeat_ClearFault( void );

#endif /* HEARTBEAT_H */
