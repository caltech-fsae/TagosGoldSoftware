/*
 * BMS Driver
 * Caltech Racing 2016
 */

/* Standard libraries */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* STM libraries */

/* RTX */
#include <cmsis_os.h>

/* FSAE firmware */
#include "bms.h"
#include "can.h"
#include "common.h"
#include "addresses.h"


/* mutex initialize */
osMutexDef(BMSMutex);
osMutexId BMSMutexId;


/* Local variables */

// BMS message buffer
static bms_stat_t BMSMsgBuffer;
// boolean value indicates if bms is initialized
static bool bms_init_success = false;

/* Local function prototypes */
static void bms_can_cb( CANRxStruct_t* stat );



const ptrdiff_t BMS_ID2offset_map[8] = {
    BMS_INVALID_CAN_ID_OFFSET,
    BMS_INVALID_CAN_ID_OFFSET,
    0,
    7,
    13,
    19, 
    27,
    34};


    
    
/*
 * Function implementations
 */
StatusInt BMS_Init()
{
    StatusInt status = STATUSINT_SUCCESS;
    if ( status == STATUSINT_SUCCESS ) {
        // initialize mutex
        BMSMutexId = osMutexCreate(osMutex(BMSMutex));
        if (BMSMutexId == NULL){
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    if ( status == STATUSINT_SUCCESS ) {
        CAN_RegisterCallback( CAN_CHANNEL_BMS, bms_can_cb );
        memset((void*)&BMSMsgBuffer, 0x0, sizeof(BMSMsgBuffer));
        bms_init_success = true;
    }

    return status;
}


StatusInt BMS_GetStatus( bms_stat_t* stat )
{
    if (!bms_init_success){
        return STATUSINT_BMS_INIT_FAIL;
    }
    if (osMutexWait(BMSMutexId, BMS_GET_STATE_TIMEOUT) == osOK) {
        memcpy(stat,(void*)&BMSMsgBuffer, BMS_MSG_LENGTH);
        osMutexRelease(BMSMutexId);
        return STATUSINT_SUCCESS;
    }else{
        return STATUSINT_BMS_BUSY;
    }

}


static void bms_can_cb( CANRxStruct_t* stat )
{
    ptrdiff_t bms_buffer_offset;
    if (osMutexWait(BMSMutexId, BMS_IRQ_TIMEOUT) == osOK) {
        if (stat->ID - ADDRESS_BMS <= MAX_BMS_CAN_ID_OFFSET) {
            bms_buffer_offset = BMS_ID2offset_map[stat->ID - ADDRESS_BMS];
            if (bms_buffer_offset != BMS_INVALID_CAN_ID_OFFSET){
                memcpy((void *)(((uint8_t *)(&BMSMsgBuffer))+ bms_buffer_offset),(const void *)stat->data,stat->length);
            }
        }
        osMutexRelease(BMSMutexId);
    }
}
