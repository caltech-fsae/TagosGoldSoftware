/*
 * Fault Management
 * Caltech Racing 2016
 */
#ifndef FAULT_H
#define FAULT_H

#include "common.h" /* StatusInt */

#define NUM_PERMANENT_FAULTS 10

#define FAULT_SIGNAL  0x1
#define FAULTN_SIGNAL 0x2

typedef struct FaultId {
    int index;
    // Is this fault currently triggered?
    bool triggered;
    // Does this fault clear when fault_clear_all is called?
    bool permanent;
    // Does this trigger a resettable fault or a non-resettable fault?
    bool resettable;
} FaultId;

StatusInt fault_add( FaultId *fault_id, bool permanent, bool resettable  );
void fault_trigger( FaultId *fault_id );
void fault_clear( FaultId *fault_id );
void fault_trigger_resettable( void );
void fault_trigger_nonresettable( void );

#endif
