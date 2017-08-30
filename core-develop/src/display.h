#include "common.h"

#define DISPLAY_TITLE_LENGTH    8
#define DISPLAY_NO_FAULT        0
#define DISPLAY_FAULT           1
#define DISPLAY_FAULT_HEIGHT    50

#define DISPLAY_COLOR_FAULT     LCD_COLOR_RED
#define DISPLAY_COLOR_LIMP      LCD_COLOR_YELLOW
#define DISPLAY_COLOR_OKAY      LCD_COLOR_GREEN
#define DISPLAY_COLOR_IDLE      LCD_COLOR_BLUE

// State IDs for display_set_state
enum {
    DISPLAY_STATE_DRIVING = 0,
    DISPLAY_STATE_LIMP,
    DISPLAY_STATE_IDLE,
};

enum {
    DISPLAY_FAULT_IMD = 1,
    DISPLAY_FAULT_BMS,
    DISPLAY_FAULT_BSPD,
    DISPLAY_FAULT_TEMP,
    DISPLAY_FAULT_CURR,
    DISPLAY_FAULT_HEART,
    DISPLAY_FAULT_SWR,
    DISPLAY_FAULT_SWNR,
    DISPLAY_FAULT_INIT,
    DISPLAY_FAULT_ALL,
};
#define N_FAULT_TYPES DISPLAY_FAULT_ALL

StatusInt Display_Init ( void );

StatusInt Display_SetFault ( uint8_t fault_id );

StatusInt Display_ClearFault ( uint8_t fault_id );

StatusInt Display_SetState ( uint8_t state_id );

StatusInt Display_SetSpeed( uint32_t speed );
StatusInt Display_SetSOC ( uint32_t soc );
StatusInt Display_SetTemperature ( uint32_t temp );
