#include <messaging.h>
#include <messages.h>
#include <bms.h>
#include "shutdown.h"
#include "bms_monitoring.h"

#include <cmsis_os.h>

#define DELAY_TIME 1000

uint32_t current_signals = BMS_SIGNAL;
static osThreadId monitoringThread;
osThreadDef( BMS_Monitoring, osPriorityNormal, 1, 0 );


/*Monitors BMS during shutdown*/
StatusInt BMS_MonitoringInit(){
     
    StatusInt bms_status;

    bms_status = BMS_Init();
    if (bms_status != STATUSINT_SUCCESS){
            return bms_status;
    }

        monitoringThread = osThreadCreate(osThread(BMS_Monitoring), NULL);
        
        
    return STATUSINT_SUCCESS;
}

void BMS_Monitoring(const void * args){

    StatusInt bms_status;
    bms_stat_t bms_stat;
    uint32_t fault_signals;
    fault_signals = BMS_SIGNAL;

    while (1) {
        bms_status = BMS_GetStatus(&bms_stat);
        if (bms_status != STATUSINT_SUCCESS){
            bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, bms_status);
            if (bms_status != STATUSINT_SUCCESS) {
                killSwitch(true);
                return;
            }
            killSwitch(true);
            return;
        
        }

        // send bms status to the comm
        bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_VOLTAGE_MAX, MSG_I_NONE, bms_stat.max_vtg);
        if (bms_status != STATUSINT_SUCCESS){
            bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_VOLTAGE_MAX, MSG_I_NONE, bms_stat.max_vtg);
            if ( bms_status != STATUSINT_SUCCESS )
            {
                fault_signals = BMS_SIGNAL;
            }
        }

        bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_VOLTAGE_MIN, MSG_I_NONE, bms_stat.min_vtg);
        if (bms_status != STATUSINT_SUCCESS){
            bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_VOLTAGE_MIN, MSG_I_NONE, bms_stat.min_vtg);
            if ( bms_status != STATUSINT_SUCCESS )
            {
                fault_signals = BMS_SIGNAL;
            }
        }
                
        bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_VOLTAGE_NMAX, MSG_I_NONE, bms_stat.max_vtg_id);
        if (bms_status != STATUSINT_SUCCESS){
            bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_VOLTAGE_NMAX, MSG_I_NONE, bms_stat.max_vtg_id);
            if ( bms_status != STATUSINT_SUCCESS )
            {
                fault_signals = BMS_SIGNAL;
            }
        }
                
        bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_VOLTAGE_NMIN, MSG_I_NONE, bms_stat.min_vtg_id);
        if (bms_status != STATUSINT_SUCCESS){
            bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_VOLTAGE_NMIN, MSG_I_NONE, bms_stat.min_vtg_id);
            if ( bms_status != STATUSINT_SUCCESS )
            {
                fault_signals = BMS_SIGNAL;
            }
        }

        bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_TS_VOLT, MSG_I_NONE, bms_stat.pack_vtg);
        if (bms_status != STATUSINT_SUCCESS){
            bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_TS_VOLT, MSG_I_NONE, bms_stat.pack_vtg);
            if ( bms_status != STATUSINT_SUCCESS )
            {
                fault_signals = BMS_SIGNAL;
            }
        }


        bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, bms_stat.discharge_limit);
        if (bms_status != STATUSINT_SUCCESS){
            bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, bms_stat.discharge_limit);
            {
                fault_signals = BMS_SIGNAL;
            }
        }

        bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, bms_stat.warnings);
        if (bms_status != STATUSINT_SUCCESS){
            bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, bms_stat.warnings);
            if ( bms_status != STATUSINT_SUCCESS )
            {
                fault_signals = BMS_SIGNAL;
            }
        }
         
          // send and handle faults
        bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, bms_stat.fault_code);
        if (bms_status != STATUSINT_SUCCESS){
            bms_status = send_int(ADDRESS_COMM, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, bms_stat.fault_code);
            if ( bms_status != STATUSINT_SUCCESS )
            {
                fault_signals = BMS_SIGNAL;
            }
        }
        
        if (fault_signals != current_signals) {
            current_signals = Transition_BMS_Faults(fault_signals, current_signals);
        }
        osDelay(DELAY_TIME);
    }


}

