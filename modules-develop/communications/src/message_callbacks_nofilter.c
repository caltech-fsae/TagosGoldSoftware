
#include <common.h>
#include <stm32f4xx_hal.h>

#include <cmsis_os.h>
#include <stdio.h>
#include <messages.h>
#include "message_callbacks_nofilter.h"
#include <messaging.h>
#include <retarget.h>
#include <uart.h>
#include <rtc.h>

#define MAX_WAIT_TIME 10000 // number of miliseconds mutex will wait to be available

uint8_t radioArray[13] = {0xff};

osMutexDef (uart_mutex);    // Declare mutex
osMutexId  (uart_mutex_id); // Mutex ID

/* 
 * All code must correctly process errors:
 * MB-109 on JIRA
 */

StatusInt init_messages_nofilter() {
    StatusInt status = STATUSINT_SUCCESS;
    if ( status == STATUSINT_SUCCESS ) {
        status = register_callback_float_all(float_message_nofilter_cb);
    }
    if ( status == STATUSINT_SUCCESS ) {
        status = register_callback_int_all(int_message_nofilter_cb);
    }

    if ( status == STATUSINT_SUCCESS ) {
        uart_mutex_id = osMutexCreate(osMutex(uart_mutex));
        if ( uart_mutex_id == NULL ) {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    return status;
}

void logFloat(uint32_t id, float payload) {
    struct tm time_info;
    if ( getTime(&time_info) != 0 )
    {
        // Process error (MB-109)
    }
    fprintf(stdlog, "[%d/%d/%d %d:%d:%d]%x:%f\r\n", time_info.tm_year, time_info.tm_mon, time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec, id, payload);
}

void logInt(uint32_t id, int payload) {
    struct tm time_info;
    if ( getTime(&time_info) != 0 )
    {
        // Process error (MB-109)
    }
    fprintf(stdlog, "[%d/%d/%d %d:%d:%d]%x:%d\r\n", time_info.tm_year, time_info.tm_mon, time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec, id, payload);
}

void radioFloat(int id, float payload) {
    // first byte is already the start token
    time_t time_value;
    if ( RTC_getTimeT(&time_value) != 0 )
    {
        // Process error (MB-109)
    }

    *((int *)&radioArray[1]) = time_value;
    *((int *)&radioArray[5]) = id;
    *((int *)&radioArray[9]) = payload;
    ////// payload should be (float *) but we get a hard fault. weird.
    // can define a custom HardFault_Handler

    UART_Write(UART_WIRELESS, radioArray, 13);
}

void radioInt(uint16_t id, int payload, uint16_t type, uint8_t sender) {
    // first byte is already the start token

    time_t time_value;
    if ( RTC_getTimeT(&time_value) != 0 )
    {
        // Process error (MB-109)
    }

    *((int *)&radioArray[1]) = time_value;
    *((int *)&radioArray[5]) = (type << MESSAGE_TYPE_SHIFT) | (sender << MESSAGE_SENDER_SHIFT) | (id << MESSAGE_ID_SHIFT);
    *((int *)&radioArray[9]) = payload;

    UART_Write(UART_WIRELESS, radioArray, 13); //Process Error (MB-109)
}

void float_message_nofilter_cb(uint16_t type, uint8_t sender, uint16_t id, float payload) {
    logFloat(id, payload);

    osMutexWait(uart_mutex_id, MAX_WAIT_TIME); //(MB-109)
    radioFloat(id, payload);
    osMutexRelease(uart_mutex_id); //(MB-109)

    // To correctly handle mutex errors, see examples in this link:
    // https://www.keil.com/pack/doc/CMSIS/RTOS/html/group___c_m_s_i_s___r_t_o_s___mutex_mgmt.html
}

void int_message_nofilter_cb(uint16_t type, uint8_t sender, uint16_t id, uint32_t payload) {
    logInt(id, payload);

    osMutexWait(uart_mutex_id, MAX_WAIT_TIME); //(MB-109)
    radioInt(id, payload, type, sender);
    osMutexRelease(uart_mutex_id); //(MB-109)
}
