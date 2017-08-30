/* 
 * File: display.c
 * Author: Megan Keehan
 * Purpose: This uses the TM drivers to update the display for FSAE
 */

#include "cmsis_os.h"
#include "tm_stm32_lcd.h"
#include "stm32f4xx_hal.h"
#include "messages.h"
#include "display.h"
#include <assert.h>

/*
#define MSG_V_FAULT_IMD      1   
#define MSG_V_FAULT_BMS      2   
#define MSG_V_FAULT_BSPD     3   
#define MSG_V_FAULT_TEMP     4   
#define MSG_V_FAULT_CURR     5   
#define MSG_V_FAULT_HEART    6	
#define MSG_V_FAULT_SWR      7   
#define MSG_V_FAULT_SWNR     8  
#define MSG_V_FAULT_INIT     9    
*/

struct fault_s
{
    uint8_t tripped;
    char title[DISPLAY_TITLE_LENGTH];
};

struct fault_s display_faults [N_FAULT_TYPES] = 
{
    { 0, "IMD" },
    { 0, "BMS" },
    { 0, "BSPD" },
    { 0, "TEMP" },
    { 0, "CURR" },
    { 0, "HEART" },
    { 0, "SWR" },
    { 0, "SWNR" },
    { 0, "INIT" }
};

static uint8_t display_state;

static uint32_t display_speed = 0;
static uint32_t display_soc = 0;
static uint32_t display_temp = 0;

static osMutexId displayMutex;
osMutexDef( displayMutex );

static void Display_DrawRectangle( uint16_t x, uint16_t y, uint16_t w, uint16_t h);

StatusInt display_redraw ( void ) 
{
    uint8_t i;
    char output[DISPLAY_TITLE_LENGTH];
    
    TM_LCD_Fill( LCD_COLOR_WHITE );
    // shows fault boxes and text
    TM_LCD_SetFont(&TM_Font_11x18);
    
    for ( i = 0; i < N_FAULT_TYPES; i++ )
    {
        if ( display_faults[i].tripped )
        {
            TM_LCD_DrawFilledRectangle( i * ( LCD_PIXEL_WIDTH / N_FAULT_TYPES), 0,  LCD_PIXEL_WIDTH / N_FAULT_TYPES, 50, 
                DISPLAY_COLOR_FAULT );
        }
        else
        {
            TM_LCD_DrawFilledRectangle( i * ( LCD_PIXEL_WIDTH / N_FAULT_TYPES), 0,  LCD_PIXEL_WIDTH / N_FAULT_TYPES, 50, 
                DISPLAY_COLOR_OKAY );
        }
        TM_LCD_SetXY(i * LCD_PIXEL_WIDTH / N_FAULT_TYPES + 5, 10);
        sprintf(output, "%s", display_faults[i].title);
        TM_LCD_Puts(output);
    }
    
    // sets big color
    Display_DrawRectangle( 0, DISPLAY_FAULT_HEIGHT, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT - DISPLAY_FAULT_HEIGHT );
    
    // Displays other info
    TM_LCD_SetXY( LCD_PIXEL_WIDTH/4, LCD_PIXEL_HEIGHT / 2 - 30);
    TM_LCD_Puts("SPEED");
    TM_LCD_SetXY( 2*LCD_PIXEL_WIDTH/4, LCD_PIXEL_HEIGHT / 2 - 30);
    TM_LCD_Puts("SOC");
    TM_LCD_SetXY( 3*LCD_PIXEL_WIDTH/4, LCD_PIXEL_HEIGHT / 2 - 30);
    TM_LCD_Puts("TEMP");
    
    TM_LCD_SetFont(&TM_Font_16x26);
    
    TM_LCD_SetXY( LCD_PIXEL_WIDTH/4, LCD_PIXEL_HEIGHT / 2);
    sprintf( output, "%d", display_speed );
    TM_LCD_Puts( output );
    
    TM_LCD_SetXY( 2*LCD_PIXEL_WIDTH/4, LCD_PIXEL_HEIGHT / 2);
    sprintf( output, "%d", 100 );
    TM_LCD_Puts( output );
    
    TM_LCD_SetXY( 3*LCD_PIXEL_WIDTH/4, LCD_PIXEL_HEIGHT / 2);
    sprintf( output, "%d", 40 );
    TM_LCD_Puts( output );
    
    return STATUSINT_SUCCESS;
}

StatusInt Display_Init ( void ) 
{
    TM_LCD_Init();
    TM_LCD_SetOrientation(1); // sets horizontal orientation
    
    display_state = DISPLAY_STATE_IDLE;
    
    /* Initialize mutex */
    displayMutex = osMutexCreate( osMutex( displayMutex ) );
    if ( displayMutex == NULL )
    {
        return STATUSINT_ERROR_GENERAL;
    }
    else
    {
        return display_redraw();
    }
}

StatusInt Display_SetFault ( uint8_t fault_id )
{
    osStatus os_status;
    StatusInt status = STATUSINT_SUCCESS;
    
    assert( fault_id >= 1 && fault_id <= N_FAULT_TYPES );
    
    os_status = osMutexWait( displayMutex, MUTEX_TIMEOUT );
    if ( os_status == osOK )
    {
        if ( display_faults[ fault_id - 1 ].tripped != DISPLAY_FAULT )
        {
            // since fault ids are 1-indexed... 
            display_faults[fault_id - 1].tripped = DISPLAY_FAULT;
            
            status = display_redraw();
        }
        
        os_status = osMutexRelease( displayMutex );
        if ( os_status != osOK )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }
    return status;
}

StatusInt Display_ClearFault ( uint8_t fault_id )
{
    int i;
    osStatus os_status;
    StatusInt status = STATUSINT_SUCCESS;
    
    assert( fault_id >= 1 && fault_id <= N_FAULT_TYPES );
    
    os_status = osMutexWait( displayMutex, MUTEX_TIMEOUT );
    if ( os_status == osOK )
    {
        // since fault ids are 1-indexed... 
        if ( fault_id == DISPLAY_FAULT_ALL )
        {
            for ( i = 0; i < DISPLAY_FAULT_ALL; i++ )
            {
                display_faults[i - 1].tripped = DISPLAY_NO_FAULT;
            }
        }
        else
        {
            display_faults[fault_id - 1].tripped = DISPLAY_NO_FAULT;
        }
        
        os_status = osMutexRelease( displayMutex );
        if ( os_status != osOK )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }
    
    if ( status == STATUSINT_SUCCESS )
    {
        status = display_redraw();
    }
    
    return status;
}

// sets state for redraw
StatusInt Display_SetState ( uint8_t state_id )
{
    osStatus os_status;
    StatusInt status = STATUSINT_SUCCESS;
    
    os_status = osMutexWait( displayMutex, MUTEX_TIMEOUT );
    if ( os_status == osOK )
    {
        display_state = state_id; 
        
        os_status = osMutexRelease( displayMutex );
        if ( os_status != osOK )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }
    if ( status == STATUSINT_SUCCESS )
    {
        status = display_redraw();
    }
    
    return status;
}

// sets speed for redraw
StatusInt Display_SetSpeed ( uint32_t speed )
{
    osStatus os_status;
    StatusInt status = STATUSINT_SUCCESS;
    char output[ 10 ];
    
    os_status = osMutexWait( displayMutex, MUTEX_TIMEOUT );
    if ( os_status == osOK )
    {
        display_speed = speed;  
        
        TM_LCD_SetFont(&TM_Font_16x26);
        
        Display_DrawRectangle( LCD_PIXEL_WIDTH/4,
                               LCD_PIXEL_HEIGHT/2,
                               16,
                               26*3);
    
        TM_LCD_SetXY( LCD_PIXEL_WIDTH/4, LCD_PIXEL_HEIGHT / 2);
        sprintf( output, "%d", display_speed );
        TM_LCD_Puts( output );
        
        os_status = osMutexRelease( displayMutex );
        if ( os_status != osOK )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }
    
    return status;
}

StatusInt Display_SetSOC ( uint32_t soc )
{
    osStatus os_status;
    StatusInt status = STATUSINT_SUCCESS;
    char output[ 10 ];
    
    os_status = osMutexWait( displayMutex, MUTEX_TIMEOUT );
    if ( os_status == osOK )
    {
        display_soc = soc;  
        
        TM_LCD_SetFont(&TM_Font_16x26);
        
        Display_DrawRectangle( 2*LCD_PIXEL_WIDTH/4,
                               LCD_PIXEL_HEIGHT/2,
                               16,
                               26*3);
    
        TM_LCD_SetXY( 2*LCD_PIXEL_WIDTH/4, LCD_PIXEL_HEIGHT / 2);
        sprintf( output, "%d", display_soc );
        TM_LCD_Puts( output );
        
        os_status = osMutexRelease( displayMutex );
        if ( os_status != osOK )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }
    
    return status;
}

StatusInt Display_SetTemperature ( uint32_t temp )
{
    osStatus os_status;
    StatusInt status = STATUSINT_SUCCESS;
    char output[ 10 ];
    
    os_status = osMutexWait( displayMutex, MUTEX_TIMEOUT );
    if ( os_status == osOK )
    {
        display_temp = temp;  
        
        TM_LCD_SetFont(&TM_Font_16x26);
        
        Display_DrawRectangle( 3*LCD_PIXEL_WIDTH/4,
                               LCD_PIXEL_HEIGHT/2,
                               16,
                               26*3);
    
        TM_LCD_SetXY( 3*LCD_PIXEL_WIDTH/4, LCD_PIXEL_HEIGHT / 2);
        sprintf( output, "%d", display_temp );
        TM_LCD_Puts( output );
        
        os_status = osMutexRelease( displayMutex );
        if ( os_status != osOK )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }
    
    return status;
}

void Display_DrawRectangle( uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    uint8_t is_fault = 0;
    uint8_t i;
    
    for ( i = 0; i < N_FAULT_TYPES; i++ )
    {
        if ( display_faults[i].tripped )
        {
            is_fault = 1;
        }
    }
    
    if ( is_fault )
    {
        TM_LCD_DrawFilledRectangle( x, y, w, h, DISPLAY_COLOR_FAULT );
    }
    else 
    {
        switch ( display_state )
        {
            case DISPLAY_STATE_DRIVING: 
                TM_LCD_DrawFilledRectangle( x, y, w, h, DISPLAY_COLOR_OKAY );
                break;
            case DISPLAY_STATE_LIMP: 
                TM_LCD_DrawFilledRectangle( x, y, w, h, DISPLAY_COLOR_LIMP );
                break;
            case DISPLAY_STATE_IDLE: 
                TM_LCD_DrawFilledRectangle( x, y, w, h, DISPLAY_COLOR_IDLE );
                break;
            default:
                break;
        }
    }
}
