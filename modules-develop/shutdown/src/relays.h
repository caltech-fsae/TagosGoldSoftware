
#ifndef RELAYS_H
#define RELAYS_H

#include <common.h>

#define DISCHARGE_WAIT_DELAY 500
#define PRECHARGE_WAIT_DELAY 500

StatusInt Relays_Init( void );
void Enable_Relays( void );
void Disable_Relays( void );

#endif // RELAYS_H
