
#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <common.h> // StatusInt

#define CORE_START_SIGNAL  0x0001
#define FAULT_SIGNAL       0x0002
#define FAULTN_SIGNAL      0x0004
#define AMS_SIGNAL         0x0008
#define IMD_SIGNAL         0x0010
#define INTERLOCK_SIGNAL   0x0020
#define BMS_SIGNAL         0x0040
#define FAULT_RESET_SIGNAL 0x0080


#define ALL_FAULTN_SIGNAL  (FAULTN_SIGNAL | AMS_SIGNAL | IMD_SIGNAL | INTERLOCK_SIGNAL)

void Shutdown_Fault( uint32_t fault_signals );
StatusInt Shutdown_Init( void );
StatusInt Shutdown_MonitoringInit( void );
uint32_t Transition_BMS_Faults(uint32_t fault_signals, uint32_t current_faults);

#endif //SHUTDOWN_SRC_H
