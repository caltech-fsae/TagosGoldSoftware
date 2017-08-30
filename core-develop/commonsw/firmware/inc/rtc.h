
/*
 * RTC Driver
 * Caltech Racing 2016
 */
#ifndef RTC_H
#define RTC_H

#include <time.h> /* time_t */
#include "common.h" /* StatusInt */

StatusInt RTC_Init( void );
StatusInt RTC_SetTime( time_t time );
StatusInt RTC_getTimeStr( char* str );
StatusInt RTC_getTimeT( time_t* time );
StatusInt getTime( struct tm *time_info );

#endif /* RTC_H */
