/*
 * Common Firmware Components
 * Caltech Racing 2016
 */

/* Standard libraries */

/* STM libraries */
#ifdef LINT
    #define _RTE_
    #include <stm32f4xx.h>
#endif
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal.h>              // Keil::Device:STM32Cube HAL:Common

/* RTX */
#include <cmsis_os.h>

/* FSAE Firmware */
#include "common.h"

/* Local types */

/* Local constants */

/* Local variables */

/* Local function prototypes */
static StatusInt initThreadIRQ( void );
static StatusInt InitClocks( void );

/* Extern'd */
osThreadId tid_Thread_IRQ;
osThreadDef( CheckIRQ, osPriorityRealtime, 1, 0 );
osMessageQDef(msgQ_IRQ, 10, uint32_t);
osMessageQId msgQ_id_IRQ;

extern void CheckIRQ( void const* arg );

#ifdef RTE_CMSIS_RTOS_RTX

extern uint32_t os_time;

uint32_t HAL_GetTick(void) {
    return os_time;
}
#endif

/* 
 * Function implementations
 */
 
StatusInt InitBoard( void )
{
    StatusInt status = STATUSINT_SUCCESS;
    HAL_StatusTypeDef hal_status;
    osStatus os_status;

    if ( status == STATUSINT_SUCCESS )
    {
        /* General HAL initialization */
        hal_status = HAL_Init();
        if ( hal_status != HAL_OK ) {
            status = STATUSINT_ERROR_UNKNOWN;
        }
    }

    if ( status == STATUSINT_SUCCESS )
    {
        /* Clock configuration */
        status = InitClocks();
    }

    if ( status == STATUSINT_SUCCESS )
    {
        /* General RTX initialization */
        os_status = osKernelInitialize();
        if ( os_status != osOK ) {
            status = STATUSINT_ERROR_UNKNOWN;
        }
    }

    if ( status == STATUSINT_SUCCESS )
    {
        /* Firmware IRQ processing thread */
        status = initThreadIRQ();

    }

    if ( status == STATUSINT_SUCCESS )
    {
        /* Board-specific initialization */
        status = InitDevices();
    }

    if ( status == STATUSINT_SUCCESS )
    {
        /* And get the party started */
        os_status = osKernelStart();
        if ( os_status != osOK ) {
            status = STATUSINT_ERROR_UNKNOWN;
        }
    }

    return status;
}

static StatusInt InitClocks( void )
{
    HAL_StatusTypeDef status;
    StatusInt retVal = STATUSINT_ERROR_UNKNOWN;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON; /* Use external oscillator */

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON; /* Turn the PLL on */
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE; /* Run the external osc. through the PLL */
    RCC_OscInitStruct.PLL.PLLM = PLLM_VALUE; /* Multiply */
    RCC_OscInitStruct.PLL.PLLN = PLLN_VALUE; /* and divide */
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; /* as necessary */
    RCC_OscInitStruct.PLL.PLLQ = 7; /* This is for USB, etc. Not needed really */
    status = HAL_RCC_OscConfig( &RCC_OscInitStruct );

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
    if ( status == HAL_OK )
    {
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; /* Everything runs off the PLL system clock */
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; /* Don't divide it for AHB */
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4; /* Divide by 4 for APB1 */
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; /* Divide by 2 for APB2 */
        status = HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_5 );
    }
    if ( status == HAL_OK )
    {
        retVal = STATUSINT_SUCCESS;
    }
    
    return retVal;
}

/**
 * Initializes GPIO at the given pin. 
 */
void PIO_Init( uint32_t pin, 
               GPIO_TypeDef * port, 
               uint32_t in_out, 
               uint32_t pull, 
               uint32_t alternate ) 
{
    GPIO_InitTypeDef GPIO_InitDef;
     
    // Initialize RCC. Hal functions have the argument inconveniently in the function name. 
    switch ( (uint32_t) port ) 
    {
        case (uint32_t)GPIOA:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
        case (uint32_t)GPIOB:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        case (uint32_t)GPIOC:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
        case (uint32_t)GPIOD:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            break;
        case (uint32_t)GPIOE:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            break;
        case (uint32_t)GPIOF:
            __HAL_RCC_GPIOF_CLK_ENABLE();
            break;
        case (uint32_t)GPIOG:
            __HAL_RCC_GPIOG_CLK_ENABLE();
            break;
        case (uint32_t)GPIOH:
            __HAL_RCC_GPIOH_CLK_ENABLE();
            break;
        case (uint32_t)GPIOI:
            __HAL_RCC_GPIOI_CLK_ENABLE();
            break;
#ifdef STM32f429xx
        case (uint32_t)GPIOJ:
            __HAL_RCC_GPIOJ_CLK_ENABLE();           
            break;
        case (uint32_t)GPIOK:
            __HAL_RCC_GPIOK_CLK_ENABLE();           
            break;
#endif
        default:
            break;
    }
 
    GPIO_InitDef.Pin = pin;
    GPIO_InitDef.Mode = in_out;
    GPIO_InitDef.Pull = pull;
    GPIO_InitDef.Speed = GPIO_SPEED_HIGH;
    if ( alternate != GPIO_SET_AF_NONE )
    {
        GPIO_InitDef.Alternate = alternate;
    }
    
    HAL_GPIO_Init( port, &GPIO_InitDef );
}

static StatusInt initThreadIRQ () 
{
    StatusInt retVal;
    tid_Thread_IRQ = osThreadCreate( osThread( CheckIRQ ), NULL );
    if ( tid_Thread_IRQ == NULL) 
    {
        retVal = STATUSINT_ERROR_GENERAL;
    }else{
        msgQ_id_IRQ = osMessageCreate(osMessageQ(msgQ_IRQ), NULL);
        if (msgQ_id_IRQ == NULL){
            retVal = STATUSINT_ERROR_GENERAL;
        }else{
            retVal = STATUSINT_SUCCESS;
        }
    }
    return retVal;
}
