#include <stm32f4xx_hal.h>
#include "common.h"
#include "temperature.h"
#include "i2c.h"
#include "cmsis_os.h"
#include "can.h"

extern osMessageQId msgQ_id_IRQ;

enum pin_state{
	LOW = 0, HIGH = 1, FLOAT = 2
};

struct temp_sensor{
	enum pin_state A0;
	enum pin_state A1;
	enum pin_state A2;
};

void temp_callback_check(uint8_t channel, uint16_t sensor_id, uint16_t temp){
	static int i;
	if ((temp != 0)&&(temp != TEMP_I2C_ERROR)){
		i++;
	}
}

StatusInt InitDevices() {
    temperature_InitExtUnit(&(temp_callback_check));
    return STATUSINT_SUCCESS;
}


void CheckIRQ( void const* arg )
{
  osEvent ev;
    uint32_t msg;
	while(1){
        ev = osMessageGet(msgQ_id_IRQ, osWaitForever);
        if(ev.status == osEventMessage){
			msg = (uint32_t)ev.value.p;
            switch(msg){
                case IRQ_TYPE_CAN:
                    CAN_HandleMessages();
                    break;
                case IRQ_TYPE_I2C_RX:
                    I2C_HandleReceives();
                    break;
                case IRQ_TYPE_I2C_TX:
                    I2C_HandleTransmit();
                    break;
                default:
                    // unhandle IRQ type
                    for(;;){}
            }
		}
    }
}

static int sensor_id_to_test[2] = {27*6 + 10 - 1, 27*7 + 27 - 1, };

// Note sensor_id should range from 0 to NUM_CHANNEL_PER_MUX*NUM_SENSOR_PER_CHANNEL
int main(void)
{	
	uint16_t temp;
    float temp_int;
    int i = 0;
    StatusInt status;
    InitBoard();
    for(i = 0; i < 2; i++){
       status = temperature_InitExt(I2C_3, 0, sensor_id_to_test[i]); 
        osDelay(100);
       if (status == STATUSINT_SUCCESS) {
            status = temperature_GetExtInteger(I2C_3, 0, sensor_id_to_test[i]);
       }
   }
	return (status == STATUSINT_SUCCESS);
}
