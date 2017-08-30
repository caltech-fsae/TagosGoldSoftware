#include <stm32f4xx_hal.h>
#include "imu.h"
#include "cmsis_os.h"


/* 
 * Common function implementations
 */
StatusInt InitDevices( void )
{
    StatusInt status = STATUSINT_ERROR_UNKNOWN;

    osDelay(500);
    status = IMU_Init();

    return status;
}

void CheckIRQ( void const* arg )
{
    while( true )
    {
        osSignalWait( SIGNAL_CAN, osWaitForever );\
    }
}

int main(void)
{
	int16_t x, y, z;

	InitBoard();
 
    while (1) {
        if(IMU_getAcceleration(&x, &y, &z) != STATUSINT_SUCCESS){
            return -1;
        }
        osDelay(500);
    }
}
