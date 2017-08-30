/*
 * BMS Driver
 * Caltech Racing 2016
 */
#ifndef BMS_H
#define BMS_H

#include "common.h"



#define BMS_GET_STATE_TIMEOUT 0 // no wait 
#define BMS_IRQ_TIMEOUT 0 // no wait 
#define BMS_MSG_LENGTH (40)
#define BMS_INVALID_CAN_ID_OFFSET (-1)
#define MAX_BMS_CAN_ID_OFFSET 7


/*
* Define correpsonding bit fault for state in bme_state_t
* Each bit correspond to a type of fault, where 0 is no fault and 
* 1 is fault. 
*/
typedef enum{
    BMS_MASTER_FAULT = 1,
    BMS_CONTACTOR_1,
    BMS_CONTACTOR_2,
    BMS_CONTACTOR_3,
    BMS_RELAY
}bms_state_bit_t;

// bms_stat_t matches the elithlion bms can message.
// ID_offset indicates the the CAN ID offset from the ADDRESS_BMS
// and length specifies the size of can message
typedef struct __attribute__((__packed__)){
// ID_offset = 2, length = 7
    uint8_t state;
    uint16_t timer;
    uint8_t flag;
    uint8_t fault_code;
    uint8_t level_faults;
    uint8_t warnings;
// ID = 3  , length = 6  
    uint16_t pack_vtg;
    uint8_t min_vtg;
    uint8_t min_vtg_id;
    uint8_t max_vtg;
    uint8_t max_vtg_id;
// ID = 4, length = 6
    uint16_t current;
    uint16_t charge_limit;
    uint16_t discharge_limit;
// ID = 5, length = 8
    uint32_t batt_energy_in;
    uint32_t batt_energy_out;
// ID = 6, length = 7
    uint8_t SOC;
    uint16_t DOD;
    uint16_t capacity;
    uint8_t  reserved1; 
    uint8_t  SOH;
// ID = 7, length = 6
    uint8_t temp;
    uint8_t  reserved2;
    uint8_t  min_temp;
    uint8_t  min_temp_id;
    uint8_t  max_temp;
    uint8_t  max_temp_id;
} bms_stat_t;




/**
 * BMS_Init()
 * @brief this function initialize the BMS CAN channel and register
 * callback for can. NOTE CAN_Init function must be called before
 * calling BMS_Init()
 * @return STATUSINT_SUCCESS if initialization succeed, STATUSINT_ERROR_GENERAL if fail.
 */
StatusInt BMS_Init( void );

/**
* BMS_GetStatus()
* @brief this function retrieve the most recent battery stat if
* data is available. Note that this function may be blocked
 * maximum BMS_GET_STATE_TIMEOUT if the firmware is currently
 * handling CAN message.
 * @return STATUSINT_SUCCESS if succeed,
 *         STATUSINT_ERROR_BMS_INIT_FAIL if bms was not initialized.
 *         TATUSINT_BMS_BUSY if bms is in can interrupt now.
*/
StatusInt BMS_GetStatus( bms_stat_t* );



#endif /* BMS_H */
