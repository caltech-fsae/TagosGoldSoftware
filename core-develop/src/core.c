#include <cmsis_os.h>

#include <common.h>
#include <motors.h>
#include <boards/core.h>
#include <messages.h>
#include <messaging.h>
#include <imu.h>
#include <heartbeat.h>
#include <stm32f4xx_hal.h>
#include "core.h"
#include <adc.h>
#include <temperature.h>
#include <display.h>



static float throttleValue;
static uint32_t throttleAge;
static float brakeValue;
static uint32_t brakeAge;
static bool invalidInput;
static uint8_t buttonStatus;
static bool temperatureWarning;
static bool interlockConnected = true;
static bool permanentFault = false;
static uint32_t currentTime;
static uint32_t bootTime;
static uint32_t lastDisplayUpdate;
static uint32_t lastBrakeUpdate;

static osMutexId stateMutex;
osMutexDef( stateMutex );
static enum states currentState;

static void receive_acc_angle( uint8_t sender, uint16_t key, uint32_t value );
static void receive_brake_angle( uint8_t sender, uint16_t key, uint32_t value );
static void receive_hitemp( uint8_t sender, uint16_t key, uint32_t value );
static void receive_fault( uint8_t sender, uint16_t key, uint32_t value );
static void receive_temp( uint8_t sender, uint16_t key, uint32_t value );
static void receive_soc( uint8_t sender, uint16_t key, uint32_t value );
static void core_controlLoop( void const * arg );
static void core_monitoringLoop( void const * arg );
static void setBrakeLight( uint8_t on );
static StatusInt startDrive( void );
static StatusInt endDrive( void );
static void startRTDS( void );
static void endRTDS( void const * arg );
static StatusInt clearFaults( void );
static void triggerFault( bool permanent, bool broadcast );
static bool brakePressed( void );
static bool brakePressedHard( void );
static uint8_t getButtonStatus( void );
static bool throttlePressed( void );

void triggerHeartbeatFault();

static osTimerId rtdsTimer;
osTimerDef( rtdsTimer, endRTDS );

static osThreadId controlLoopThread;
osThreadDef (core_controlLoop, osPriorityRealtime, 1, 0);
static osThreadId monitoringLoopThread;
osThreadDef (core_monitoringLoop, osPriorityNormal, 1, 0);

StatusInt core_Init()
{
    StatusInt status;
    
    bootTime = osKernelSysTick();
    currentState = CORE_STATE_IDLE;
    temperatureWarning = false;
    Display_SetState( DISPLAY_STATE_IDLE );

    /* Register callbacks */
    status = register_callback_int( MSG_T_THROTTLE, receive_acc_angle );

    if ( status == STATUSINT_SUCCESS )
    {
        status = register_callback_int( MSG_T_BRAKE, receive_brake_angle );
    }

    if ( status == STATUSINT_SUCCESS )
    {
        status = register_callback_int( MSG_T_HITEMP, receive_hitemp );
    }
    
    if ( status == STATUSINT_SUCCESS )
    {
        status = register_callback_int( MSG_T_FAULT, receive_fault );
    }
    
    if ( status == STATUSINT_SUCCESS )
    {
        status = register_callback_int( MSG_T_SOC, receive_soc );
    }
    
    if ( status == STATUSINT_SUCCESS )
    {
        status = register_callback_int( MSG_T_TEMP_MAX, receive_temp );
    }


    /* Init threads */
    if ( status == STATUSINT_SUCCESS )
    {
        controlLoopThread = osThreadCreate( osThread( core_controlLoop ), NULL );
        if ( controlLoopThread == NULL )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    if ( status == STATUSINT_SUCCESS )
    {
        monitoringLoopThread = osThreadCreate( osThread( core_monitoringLoop ), NULL );
        if (monitoringLoopThread == NULL )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    /* Initialize mutex */
    if ( status == STATUSINT_SUCCESS ) 
    {
        stateMutex = osMutexCreate( osMutex( stateMutex ) );
        if ( stateMutex == NULL )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    /* Initialize timer */
    if ( status == STATUSINT_SUCCESS )
    {
        rtdsTimer = osTimerCreate( osTimer( rtdsTimer ), osTimerOnce, (void*) NULL );

        if ( rtdsTimer == NULL )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    /* Initialize GPIOs */
    if ( status == STATUSINT_SUCCESS )
    {
        PIO_Init( DRIVERBUTTON_GPIO_PIN,
                  DRIVERBUTTON_GPIO_GROUP,
                  GPIO_MODE_INPUT,
                  GPIO_PULLUP,
                  GPIO_SET_AF_NONE );

        PIO_Init( RTDS_GPIO_PIN,
                  RTDS_GPIO_GROUP,
                  GPIO_MODE_OUTPUT_PP,
                  GPIO_NOPULL,
                  GPIO_SET_AF_NONE );
    }

    return status;
}

/* This will be the function running in the main motor control thread. It will
   look at the angle of the pedals, and use a function decided by the control
   systems people to calculate desired speed, and send this to the motor.
     Go from 0 to 100 speed
 */
void core_controlLoop( void const * arg ) 
{
    uint32_t speed;
    StatusInt status = STATUSINT_SUCCESS;
    float desiredTorque;
    while ( 1 )
    {
        if ( osKernelSysTick() - lastBrakeUpdate > osKernelSysTickMicroSec( 200000 ) )
        {
            /* Deal with brake light */
            if ( brakePressed() == true )
            {
                setBrakeLight( BRAKE_ON );
            }
            else
            {
                setBrakeLight( BRAKE_OFF );
            }
            lastBrakeUpdate = osKernelSysTick();
        }


        /* Check for invalid input */
        if ( invalidInput == false )
        {
            if ( ( brakePressed() == true ) && ( throttlePressed() == true ) )
            {
                invalidInput = true;
            }
        }
        else
        {
            if ( ( brakePressed() == false ) && ( throttlePressed() == false ) )
            {
                invalidInput = false;
            }
        }

        /* Check age */
        currentTime = osKernelSysTick();
        if ( ( ( currentTime - brakeAge ) > osKernelSysTickMicroSec( IO_UPDATE_LIFE_EXPECTANCY ) )
             ||
             ( ( currentTime - throttleAge ) > osKernelSysTickMicroSec( IO_UPDATE_LIFE_EXPECTANCY ) ) )
        {
            (void) Motor_SetTorque( 0 );
            (void) Motor_Disable();
            if ( ( currentTime - bootTime )> osKernelSysTickMicroSec( STARTUP_TIME ) )
            {
                triggerFault( false, true );
                osDelay(200);
            }
        }


        /* Set motor speed */
        if ( ( currentState == CORE_STATE_DRIVING ) && ( invalidInput == false ) )
        {
            /* Actually do something useful */
            desiredTorque = throttleValue * MOTOR_MAX_TORQUE;
        }
        else if ( ( currentState == CORE_STATE_LIMP ) && ( invalidInput == false ) )
        {
            if ( throttleValue * MOTOR_MAX_TORQUE < MOTOR_LIMP_TORQUE )
            {
                desiredTorque = throttleValue * MOTOR_MAX_TORQUE;
            }
            else
            {
                desiredTorque = MOTOR_LIMP_TORQUE;
            }
        }
        else
        {
            /* Set torque to 0 */
            desiredTorque = 0;
        }
        //status = Motor_SetTorque( desiredTorque );

        if ( status != STATUSINT_SUCCESS )
        {
            (void) Motor_SetTorque( 0 );
            (void) Motor_Disable();
            triggerFault( true, true );
            return;
        }
        
        //status = Motor_GetSpeed( &speed );
        if ( status == STATUSINT_SUCCESS )
        {
            if ( osKernelSysTick() - lastDisplayUpdate > osKernelSysTickMicroSec( 1000000 ) )
            {
                lastDisplayUpdate = osKernelSysTick();
                Display_SetSpeed( speed );
            }
        }

        osDelay( MOTOR_UPDATE_SPEED );
    }
}

void core_monitoringLoop( void const * arg ) {
    StatusInt status;
    uint8_t newButtonStatus = BUTTON_RELEASED;

    while ( 1 ) {
        newButtonStatus = getButtonStatus();

        if ( newButtonStatus != buttonStatus )
        {
            buttonStatus = newButtonStatus;

            if ( buttonStatus == BUTTON_PRESSED )
            {
                switch ( currentState )
                {
                    case CORE_STATE_IDLE:
                        if ( brakePressedHard() == true )
                        {
                            status = startDrive();
                        }
                    break;

                    case CORE_STATE_LIMP:
                        if ( brakePressedHard() == true )
                        {
                            status = startDrive();
                        }
                    break;

                    case CORE_STATE_DRIVING:
                        status = endDrive();
                    break;

                    case CORE_STATE_FAULT:
                        status = clearFaults();
                    break;

                    default:
                        triggerFault( true, true );
                    break;
                }
            }
        }

        if ( status != STATUSINT_SUCCESS )
        {
            triggerFault( true, true );
            return;
        }
        osDelay( CORE_MONITORING_DELAY );
    }
}

void receive_acc_angle( uint8_t sender, uint16_t key, uint32_t value )
{
    throttleValue = (float)value / UINT32_MAX;
    throttleAge = osKernelSysTick();
}


void receive_brake_angle( uint8_t sender, uint16_t key, uint32_t value )
{
    brakeValue = (float)value / UINT32_MAX;
    brakeAge = osKernelSysTick();
}

void receive_temp( uint8_t sender, uint16_t key, uint32_t value )
{
    Display_SetTemperature( value );
}

void receive_soc( uint8_t sender, uint16_t key, uint32_t value )
{
    Display_SetSOC( value );
}

void receive_hitemp( uint8_t sender, uint16_t key, uint32_t value )
{
    osStatus os_status = osMutexWait( stateMutex, MUTEX_TIMEOUT );

    if ( os_status == osOK )
    {
        if ( value == MSG_V_HITEMP_GOOD )
        {
            temperatureWarning = false;
            if ( currentState == CORE_STATE_LIMP )
            {
                currentState = CORE_STATE_DRIVING;
                Display_SetState( DISPLAY_STATE_DRIVING );
            }
        }
        else
        {
            temperatureWarning = true;
            if ( currentState == CORE_STATE_DRIVING )
            {
                currentState = CORE_STATE_LIMP;
                Display_SetState( DISPLAY_STATE_LIMP );
            }
        }
        os_status = osMutexRelease( stateMutex );
    }
    else
    {
        if ( value != MSG_V_HITEMP_GOOD )
        {
            temperatureWarning = true;
        }
        triggerFault( true, true );
    }
}

void receive_fault( uint8_t sender, uint16_t key, uint32_t value )
{
    bool permanent;
    uint8_t fault;
    switch ( value )
    {
        case MSG_V_FAULT_SWR:
            permanent = false;
            fault = DISPLAY_FAULT_SWR;
            break;  
        
        case MSG_V_FAULT_IMD:   
            permanent = true;
            fault = DISPLAY_FAULT_IMD;
            break;
        case MSG_V_FAULT_BMS:
            permanent = true;
            fault = DISPLAY_FAULT_BMS;
            break;
        case MSG_V_FAULT_BSPD:
            permanent = true;
            fault = DISPLAY_FAULT_BSPD;
            break;
        case MSG_V_FAULT_TEMP:
            permanent = true;
            fault = DISPLAY_FAULT_TEMP;
            break;
        case MSG_V_FAULT_CURR:
            permanent = true;
            fault = DISPLAY_FAULT_CURR;
            break;
        case MSG_V_FAULT_HEART:
            permanent = true;
            fault = DISPLAY_FAULT_HEART;
            break;
        case MSG_V_FAULT_SWNR:
            permanent = true;
            fault = DISPLAY_FAULT_SWNR;
            break;
        case MSG_V_FAULT_INIT:
            permanent = true;
            fault = DISPLAY_FAULT_INIT;
            break;
        default:
            permanent = true;
            break;
    }
    Display_SetFault( fault );
    triggerFault( permanent, false );
}

void receive_interlock( uint8_t sender, uint16_t key, uint32_t value )
{
    if ( value == MSG_V_INTERLOCK_CONNECTED )
    {
        
    }
    else if ( value == MSG_V_INTERLOCK_DISCONNECTED )
    {
    }
}


void setBrakeLight( uint8_t on )
{
    StatusInt status;
    
    if ( on == BRAKE_ON )
    {
        status = send_int( ADDRESS_SHUTDOWN, MSG_P_BRAKELIGHT, MSG_T_BRAKELIGHT, MSG_I_NONE, MSG_V_BRAKELIGHT_ON );
    }
    else
    {
        status = send_int( ADDRESS_SHUTDOWN, MSG_P_BRAKELIGHT, MSG_T_BRAKELIGHT, MSG_I_NONE, MSG_V_BRAKELIGHT_OFF );
    }
}

StatusInt startDrive()
{
    StatusInt status = STATUSINT_SUCCESS;
    osStatus os_status;

    if ( interlockConnected == true )
    {
        status = send_int( ADDRESS_SHUTDOWN, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, MSG_V_STATUS_R2D );
        if ( status != STATUSINT_SUCCESS ) 
        {
            status = send_int( ADDRESS_SHUTDOWN, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, MSG_V_STATUS_R2D );
        }
        if ( status == STATUSINT_SUCCESS )
        {
            startRTDS();

            os_status = osMutexWait( stateMutex, MUTEX_TIMEOUT );

            if ( os_status == osOK )
            {
                if ( currentState == CORE_STATE_IDLE )
                {
                    if ( temperatureWarning == true )
                    {
                        currentState = CORE_STATE_LIMP;
                        Display_SetState( DISPLAY_STATE_DRIVING );
                    }
                    else
                    {
                        currentState = CORE_STATE_DRIVING;
                        Display_SetState( DISPLAY_STATE_DRIVING );
                    }
                    //status = Motor_Enable();
                }
                else
                {
                    endRTDS( (void*) NULL );
                }
            }
            else
            {
                status = STATUSINT_ERROR_GENERAL;
            }
        }

        if ( status == STATUSINT_SUCCESS )
        {
            os_status = osMutexRelease( stateMutex );
            if ( os_status != osOK )
            {
                status = STATUSINT_ERROR_GENERAL;
            }
        }
    }
    return status;
}

StatusInt endDrive()
{
    StatusInt status;
    osStatus os_status;

    status = send_int( ADDRESS_SHUTDOWN, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, MSG_V_STATUS_STOP );
    if ( status != STATUSINT_SUCCESS ) 
    {
        status = send_int( ADDRESS_SHUTDOWN, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, MSG_V_STATUS_STOP );
    }
    if ( status == STATUSINT_SUCCESS )
    {
        os_status = osMutexWait( stateMutex, MUTEX_TIMEOUT );

        if ( os_status == osOK )
        {
            if ( ( currentState == CORE_STATE_DRIVING ) || ( currentState == CORE_STATE_LIMP ) )
            {
                currentState = CORE_STATE_IDLE;
                Display_SetState( DISPLAY_STATE_IDLE );
                Motor_Disable();
            }
        }
        else
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    if ( status == STATUSINT_SUCCESS )
    {
        os_status = osMutexRelease( stateMutex );
        if ( os_status != osOK )
        {
            status = STATUSINT_ERROR_GENERAL;
        }
    }

    return status;
}

void startRTDS()
{
    HAL_GPIO_WritePin( RTDS_GPIO_GROUP,
                       RTDS_GPIO_PIN,
                       GPIO_PIN_SET );
    
    osTimerStart( rtdsTimer, RTDS_LENGTH );

}

void endRTDS( void const * arg )
{
    osTimerStop( rtdsTimer ); /* In case it was still running */

    HAL_GPIO_WritePin( RTDS_GPIO_GROUP,
                       RTDS_GPIO_PIN,
                       GPIO_PIN_RESET );
}

StatusInt clearFaults()
{
    StatusInt status = STATUSINT_SUCCESS;
    osStatus os_status;

    if ( ( interlockConnected == true ) && ( permanentFault == false ) )
    {
        status = send_int( ADDRESS_SHUTDOWN, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, MSG_V_STATUS_RESETD );
        if ( status != STATUSINT_SUCCESS )
        {
            status = send_int( ADDRESS_SHUTDOWN, PRIORITY_LOWEST, MSG_T_STATUS, MSG_I_NONE, MSG_V_STATUS_RESETD );
        }

        if ( status == STATUSINT_SUCCESS )
        {
            os_status = osMutexWait( stateMutex, MUTEX_TIMEOUT );

            if ( os_status == osOK )
            {
                Display_ClearFault( DISPLAY_FAULT_SWR );
                if ( currentState == CORE_STATE_FAULT )
                {
                    currentState = CORE_STATE_IDLE;
                    Display_SetState( DISPLAY_STATE_IDLE );
                }
            }
            else
            {
                status = STATUSINT_ERROR_GENERAL;
            }
        }

        if ( status == STATUSINT_SUCCESS )
        {
            os_status = osMutexRelease( stateMutex );
            if ( os_status != osOK )
            {
                status = STATUSINT_ERROR_GENERAL;
            }
        }
    }

    return status;
}

void triggerFault( bool permanent, bool broadcast )
{
    osStatus os_status;
    StatusInt status;
    bool sendForever = false;
    
    if ( permanent == true )
    {
        Display_SetFault( DISPLAY_FAULT_SWNR );
        permanentFault = true;
        if ( broadcast == true )
        {
            status = send_int( ADDRESS_SHUTDOWN,
                               MSG_P_FAULT,
                               MSG_T_FAULT,
                               MSG_I_NONE,
                               MSG_V_FAULT_SWNR );
            if ( status != STATUSINT_SUCCESS )
            {
                sendForever = true;
            }
        }
    }
    else
    {
        Display_SetFault( DISPLAY_FAULT_SWR );
        if ( broadcast == true )
        {
            //status = send_int( ADDRESS_SHUTDOWN,
              //                 MSG_P_FAULT,
                //               MSG_T_FAULT,
                  //             MSG_I_NONE,
                    //           MSG_V_FAULT_SWR );
            if ( status != STATUSINT_SUCCESS )
            {
                sendForever = true;
            }
        }
    }
    
    if ( permanent == true )
    {
        sendForever = true;
    }
    
    os_status = osMutexWait( stateMutex, osWaitForever /* no timeout because fault */ );

    if ( os_status == osOK )
    {
        currentState = CORE_STATE_FAULT;
    }
    else
    {
        (void)Heartbeat_Fault(); /* Don't really care if this fails */
        while ( 1 )
        {
            currentState = CORE_STATE_FAULT;
            if ( sendForever == true );
            {
                (void) send_int( ADDRESS_SHUTDOWN,
                                 MSG_P_FAULT,
                                 MSG_T_FAULT,
                                 MSG_I_NONE,
                                 MSG_V_FAULT_SWNR );
                osDelay( 100 );
            }
        }
    }

    if ( status == STATUSINT_SUCCESS )
    {
        os_status = osMutexRelease( stateMutex );
        if ( os_status != osOK )
        {
            (void)Heartbeat_Fault(); /* Don't really care if this fails */
            while ( 1 )
            {
                currentState = CORE_STATE_FAULT;
                if ( sendForever == true );
                {
                    (void) send_int( ADDRESS_SHUTDOWN,
                                     MSG_P_FAULT,
                                     MSG_T_FAULT,
                                     MSG_I_NONE,
                                     MSG_V_FAULT_SWNR );
                    osDelay( 100 );
                }
            }
        }
    }
    if ( sendForever == true )
    {
        while ( 1 )
        {
            (void) send_int( ADDRESS_SHUTDOWN,
                             MSG_P_FAULT,
                             MSG_T_FAULT,
                             MSG_I_NONE,
                             MSG_V_FAULT_SWNR );
            osDelay( 100 );
        }
    }
}

void triggerHeartbeatFault()
{
    osStatus os_status;
    StatusInt status;
    
    Display_SetFault( DISPLAY_FAULT_HEART );
    
    os_status = osMutexWait( stateMutex, osWaitForever /* no timeout because fault */ );

    if ( os_status == osOK )
    {
        currentState = CORE_STATE_FAULT;
    }
    else
    {
        while ( 1 )
        {
            currentState = CORE_STATE_FAULT;
        }
    }

    if ( status == STATUSINT_SUCCESS )
    {
        os_status = osMutexRelease( stateMutex );
        if ( os_status != osOK )
        {
            while ( 1 )
            {
                currentState = CORE_STATE_FAULT;
            }
        }
    }
}


bool brakePressed()
{
    bool pressed;

    if ( brakeValue > BRAKE_THRESHOLD_LIGHT )
    {
        pressed = true;
    }
    else
    {
        pressed = false;
    }

    return pressed;
}

bool brakePressedHard()
{
    bool pressed;

    if ( brakeValue > BRAKE_THRESHOLD_HARD )
    {
        pressed = true;
    }
    else
    {
        pressed = false;
    }

    return pressed;
}

uint8_t getButtonStatus()
{
    GPIO_PinState state;
    uint8_t pressed;

    state = HAL_GPIO_ReadPin( DRIVERBUTTON_GPIO_GROUP,
                               DRIVERBUTTON_GPIO_PIN );

    if ( state == GPIO_PIN_SET )
    {
        pressed = BUTTON_RELEASED;
    }
    else
    {
        pressed = BUTTON_PRESSED;
    }

    return pressed;
}

bool throttlePressed()
{
    bool pressed;

    if ( throttleValue > THROTTLE_THRESHOLD )
    {
        pressed = true;
    }
    else
    {
        pressed = false;
    }

    return pressed;
}
