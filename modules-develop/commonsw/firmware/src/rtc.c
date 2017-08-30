/*
 * RTC Driver
 * Caltech Racing 2016
 */

/* Standard libraries */
#include <string.h>
#include "common.h"
/* STM libraries */
#ifdef LINT
#define _RTE_
    #include <stm32f4xx.h>
#endif
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_rtc.h>

/* RTX */

/* FSAE Firmware */
#include "rtc.h"

/* Local types */

/* Local constants */
static const uint8_t daysOfWeek[] = { RTC_WEEKDAY_SUNDAY,
                                      RTC_WEEKDAY_MONDAY,
                                      RTC_WEEKDAY_TUESDAY,
                                      RTC_WEEKDAY_WEDNESDAY,
                                      RTC_WEEKDAY_THURSDAY,
                                      RTC_WEEKDAY_FRIDAY,
                                      RTC_WEEKDAY_SATURDAY };
static const uint8_t monthsOfYear[] = { RTC_MONTH_JANUARY,
                                        RTC_MONTH_FEBRUARY,
                                        RTC_MONTH_MARCH,
                                        RTC_MONTH_APRIL,
                                        RTC_MONTH_MAY,
                                        RTC_MONTH_JUNE,
                                        RTC_MONTH_JULY,
                                        RTC_MONTH_AUGUST,
                                        RTC_MONTH_SEPTEMBER,
                                        RTC_MONTH_OCTOBER,
                                        RTC_MONTH_NOVEMBER,
                                        RTC_MONTH_DECEMBER };

/* Local variables */
static RTC_HandleTypeDef rtc_handle;

/* Local function prototypes */



/* 
 * Function implementations
 */

/**
  * @brief: initialize rtc 
  * @return: StatusInt
  */
StatusInt RTC_Init( void )
{
    HAL_StatusTypeDef status;
    StatusInt retVal = STATUSINT_ERROR_GENERAL;
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_RTC_CONFIG( RCC_RTCCLKSOURCE_LSI );
    __HAL_RCC_RTC_ENABLE();

    rtc_handle.Instance = RTC;
    rtc_handle.Init.HourFormat = RTC_HOURFORMAT_24;
    rtc_handle.Init.AsynchPrediv = 0x7F;
    rtc_handle.Init.SynchPrediv = 0xFF;
    rtc_handle.Init.OutPut = RTC_OUTPUT_DISABLE;

    status = HAL_RTC_Init( &rtc_handle );

    /* TODO: Install messaging callback (CS-64). */

    if ( status == HAL_OK )
    {
        retVal = STATUSINT_SUCCESS;
    }

    return retVal;
}

StatusInt RTC_SetTime( time_t time_value )
{
    StatusInt retVal = STATUSINT_ERROR_GENERAL;
    HAL_StatusTypeDef status;
    RTC_DateTypeDef cur_date;
    RTC_TimeTypeDef cur_time;
    struct tm *time_info;

    time_info = localtime( &time_value );

    cur_date.Year = ( time_info->tm_year % 100 ); /* tm years since 1900; hal 0-99 */
    cur_date.Month = monthsOfYear[ time_info->tm_mon ];
    cur_date.Date = time_info->tm_mday;
    cur_date.WeekDay = daysOfWeek[ time_info->tm_wday ];

    status = HAL_RTC_SetDate( &rtc_handle, &cur_date, RTC_FORMAT_BIN );

    if ( status == HAL_OK )
    {
        cur_time.Hours = time_info->tm_hour;
        cur_time.Minutes = time_info->tm_min;
        cur_time.Seconds = time_info->tm_sec;

        if ( time_info->tm_isdst == 0 )
        {
            cur_time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        }
        else
        {
            cur_time.DayLightSaving = RTC_DAYLIGHTSAVING_ADD1H;
        }

        status = HAL_RTC_SetTime( &rtc_handle, &cur_time, RTC_FORMAT_BIN );
    }

    if ( status == HAL_OK )
    {
        retVal = STATUSINT_SUCCESS;
    }

    return retVal;
}


/**
 * @biref: sync up the RTC time with the received rtc message value
 * @return: StatusInt
 */
StatusInt RTC_sync_callback(uint8_t sender, uint16_t id, uint32_t val){
    // TODO decide if this is necessary
    // assert(sender == MSG_S_COMM);
    time_t current_t;
    StatusInt status = STATUSINT_SUCCESS;
    status = RTC_getTimeT(&current_t);
    /*
    if ((status == STATUSINT_SUCCESS)&(time != (time_t)val)){
        status = RTC_SetTime(val);
    } */
    // TODO CS-64 fix type casting error
    return status;
}

/**
  * @brief: gets time in string
  * @return: StatusInt
  */
StatusInt RTC_getTimeStr( char* str )
{
    StatusInt status;
    char* time_in_str;
    struct tm time_info;

    status = getTime( &time_info );
    if ( status == STATUSINT_SUCCESS )
    {
        time_in_str = asctime( &time_info );
        strncpy( str, time_in_str, 24 );
    }

    return status;
}

/**
  * @brief: gets time in epoch
  * @return: StatusInt
  */
StatusInt RTC_getTimeT( time_t* time_value )
{
    StatusInt status;
    struct tm time_info;

    status = getTime( &time_info );
    if ( status == STATUSINT_SUCCESS )
    {
        *time_value = mktime( &time_info );
    }

    return status;
}

StatusInt getTime( struct tm *time_info )
{
    HAL_StatusTypeDef status;
    StatusInt retVal = STATUSINT_ERROR_GENERAL;
    RTC_DateTypeDef date;
    RTC_TimeTypeDef cur_time = {0};

    status = HAL_RTC_GetDate( &rtc_handle, &date, RTC_FORMAT_BIN );
    if ( status == HAL_OK )
    {
        status = HAL_RTC_GetTime( &rtc_handle, &cur_time, RTC_FORMAT_BIN );
    }

    if ( status == HAL_OK )
    {
        time_info->tm_sec = cur_time.Seconds;
        time_info->tm_min = cur_time.Minutes;
        time_info->tm_hour = cur_time.Hours;
        time_info->tm_mday = date.Date;
        switch ( date.WeekDay )
        {
            case RTC_WEEKDAY_MONDAY:
                time_info->tm_mon = 1;
                break;
            case RTC_WEEKDAY_TUESDAY:
                time_info->tm_mon = 2;
                break;
            case RTC_WEEKDAY_WEDNESDAY:
                time_info->tm_mon = 3;
                break;
            case RTC_WEEKDAY_THURSDAY:
                time_info->tm_mon = 4;
                break;
            case RTC_WEEKDAY_FRIDAY:
                time_info->tm_mon = 5;
                break;
            case RTC_WEEKDAY_SATURDAY:
                time_info->tm_mon = 6;
                break;
            case RTC_WEEKDAY_SUNDAY:
                time_info->tm_mon = 0;
                break;
            default:
                time_info->tm_mon = 0;
        }
        switch ( date.Month )
        {
            case RTC_MONTH_JANUARY:
                time_info->tm_mon = 0;
                break;
            case RTC_MONTH_FEBRUARY:
                time_info->tm_mon = 1;
                break;
            case RTC_MONTH_MARCH:
                time_info->tm_mon = 2;
                break;
            case RTC_MONTH_APRIL:
                time_info->tm_mon = 3;
                break;
            case RTC_MONTH_MAY:
                time_info->tm_mon = 4;
                break;
            case RTC_MONTH_JUNE:
                time_info->tm_mon = 5;
                break;
            case RTC_MONTH_JULY:
                time_info->tm_mon = 6;
                break;
            case RTC_MONTH_AUGUST:
                time_info->tm_mon = 7;
                break;
            case RTC_MONTH_SEPTEMBER:
                time_info->tm_mon = 8;
                break;
            case RTC_MONTH_OCTOBER:
                time_info->tm_mon = 9;
                break;
            case RTC_MONTH_NOVEMBER:
                time_info->tm_mon = 10;
                break;
            case RTC_MONTH_DECEMBER:
                time_info->tm_mon = 11;
                break;
            default:
                time_info->tm_mon = 0;
        }
        time_info->tm_year = date.Year + 100;
        if ( cur_time.DayLightSaving == RTC_DAYLIGHTSAVING_NONE )
        {
            time_info->tm_isdst = 0;
        }
        else
        {
            time_info->tm_isdst = 1;
        }

        retVal = STATUSINT_SUCCESS;
    }

    return retVal;
}
