
#include <common.h>
#include <stm32f4xx_hal.h>

#include "relays.h"

#include <cmsis_os.h>

osMutexDef (relay_mutex);    // Declare mutex
osMutexId  (relay_mutex_id); // Mutex ID

// Init the mutex, and init the GPIOs
StatusInt Relays_Init( void ) {
    relay_mutex_id = osMutexCreate(osMutex(relay_mutex));
    if ( relay_mutex_id == NULL ) {
        // Check return values MB-112
    }

    PIO_Init(
        DCHG_GPIO_PIN,
        DCHG_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        PCHG_GPIO_PIN,
        PCHG_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        AIR1H_GPIO_PIN,
        AIR1H_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        AIR1L_GPIO_PIN,
        AIR1L_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        AIR2H_GPIO_PIN,
        AIR2H_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        AIR2L_GPIO_PIN,
        AIR2L_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        AIR3H_GPIO_PIN,
        AIR3H_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);
    PIO_Init(
        AIR3L_GPIO_PIN,
        AIR3L_GPIO_GROUP,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL,
        GPIO_SET_AF_NONE);

    return STATUSINT_SUCCESS;
}

// Turn on the relays in the correct sequence
void Enable_Relays( void ) {
    osStatus os_status;
    os_status = osMutexWait(relay_mutex_id, osWaitForever);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }

    HAL_GPIO_WritePin(
        DCHG_GPIO_GROUP,
        DCHG_GPIO_PIN,
        GPIO_PIN_SET);

    os_status = osDelay(DISCHARGE_WAIT_DELAY);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }

    HAL_GPIO_WritePin(
        AIR1H_GPIO_GROUP,
        AIR1H_GPIO_PIN,
        GPIO_PIN_SET);
    os_status = osDelay(50);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }
    HAL_GPIO_WritePin(
        AIR1L_GPIO_GROUP,
        AIR1L_GPIO_PIN,
        GPIO_PIN_SET);
    os_status = osDelay(50);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }
    HAL_GPIO_WritePin(
        AIR2H_GPIO_GROUP,
        AIR2H_GPIO_PIN,
        GPIO_PIN_SET);
    os_status = osDelay(50);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }
    HAL_GPIO_WritePin(
        AIR2L_GPIO_GROUP,
        AIR2L_GPIO_PIN,
        GPIO_PIN_SET);
    os_status = osDelay(50);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }
    HAL_GPIO_WritePin(
        AIR3H_GPIO_GROUP,
        AIR3H_GPIO_PIN,
        GPIO_PIN_SET);
    os_status = osDelay(50);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }
    HAL_GPIO_WritePin(
        AIR3L_GPIO_GROUP,
        AIR3L_GPIO_PIN,
        GPIO_PIN_SET);

    os_status = osDelay(PRECHARGE_WAIT_DELAY);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }

    HAL_GPIO_WritePin(
        PCHG_GPIO_GROUP,
        PCHG_GPIO_PIN,
        GPIO_PIN_SET);

    os_status = osMutexRelease(relay_mutex_id);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }
}

void Disable_Relays( void ) {
    osStatus os_status;
    os_status = osMutexWait(relay_mutex_id, osWaitForever);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }

    HAL_GPIO_WritePin(
        DCHG_GPIO_GROUP,
        DCHG_GPIO_PIN,
        GPIO_PIN_RESET);

    HAL_GPIO_WritePin(
        AIR1H_GPIO_GROUP,
        AIR1H_GPIO_PIN,
        GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        AIR1L_GPIO_GROUP,
        AIR1L_GPIO_PIN,
        GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        AIR2H_GPIO_GROUP,
        AIR2H_GPIO_PIN,
        GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        AIR2L_GPIO_GROUP,
        AIR2L_GPIO_PIN,
        GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        AIR3H_GPIO_GROUP,
        AIR3H_GPIO_PIN,
        GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        AIR3L_GPIO_GROUP,
        AIR3L_GPIO_PIN,
        GPIO_PIN_RESET);

    HAL_GPIO_WritePin(
        PCHG_GPIO_GROUP,
        PCHG_GPIO_PIN,
        GPIO_PIN_RESET);

    os_status = osMutexRelease(relay_mutex_id);
    if ( os_status != osOK ) {
        // Check return values MB-112
    }
}

