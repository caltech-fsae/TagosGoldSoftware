/*
 * ADC Driver
 * Caltech Racing 2016
 */

/* Standard libraries */

/* STM libraries */
#ifdef LINT
    #define _RTE_
#endif
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_adc.h>
#include <stm32f4xx_hal_gpio.h>

/* RTX */

/* FSAE Firmware */
#include "common.h"
#include "adc.h"

/* Local types */
struct adc_chan_s
{
    uint16_t pin;
    GPIO_TypeDef* group;
    uint32_t channel;
};

struct adc_s
{
    ADC_HandleTypeDef handle;
    DMA_HandleTypeDef dma;
    uint32_t channel1_value;
    uint32_t channel2_value;
    uint32_t buffer[ADC_BUFFER_LENGTH];
    uint8_t err_flag;
};

/* Local constants */

/* Local variables */
const struct adc_chan_s channel_info[ ADC_N ][ ADC_CHANNEL_N] =
{
    {
        { ADC1_C0_PIN,  ADC1_C0_GROUP,  ADC_CHANNEL_0  },
        { ADC1_C1_PIN,  ADC1_C1_GROUP,  ADC_CHANNEL_1  },
        { ADC1_C2_PIN,  ADC1_C2_GROUP,  ADC_CHANNEL_2  },
        { ADC1_C3_PIN,  ADC1_C3_GROUP,  ADC_CHANNEL_3  },
        { ADC1_C4_PIN,  ADC1_C4_GROUP,  ADC_CHANNEL_4  },
        { ADC1_C5_PIN,  ADC1_C5_GROUP,  ADC_CHANNEL_5  },
        { ADC1_C6_PIN,  ADC1_C6_GROUP,  ADC_CHANNEL_6  },
        { ADC1_C7_PIN,  ADC1_C7_GROUP,  ADC_CHANNEL_7  },
        { ADC1_C8_PIN,  ADC1_C8_GROUP,  ADC_CHANNEL_8  },
        { ADC1_C9_PIN,  ADC1_C9_GROUP,  ADC_CHANNEL_9  },
        { ADC1_C10_PIN,  ADC1_C10_GROUP,  ADC_CHANNEL_10  },
        { ADC1_C11_PIN,  ADC1_C11_GROUP,  ADC_CHANNEL_11  },
        { ADC1_C12_PIN,  ADC1_C12_GROUP,  ADC_CHANNEL_12  },
        { ADC1_C13_PIN,  ADC1_C13_GROUP,  ADC_CHANNEL_13  },
        { ADC1_C14_PIN,  ADC1_C14_GROUP,  ADC_CHANNEL_14  },
        { ADC1_C15_PIN,  ADC1_C15_GROUP,  ADC_CHANNEL_15  }
    },
    { 
        { ADC2_C0_PIN,  ADC2_C0_GROUP,  ADC_CHANNEL_0 },
        { ADC2_C1_PIN,  ADC2_C1_GROUP,  ADC_CHANNEL_1 },
        { ADC2_C2_PIN,  ADC2_C2_GROUP,  ADC_CHANNEL_2 },
        { ADC2_C3_PIN,  ADC2_C3_GROUP,  ADC_CHANNEL_3 },
        { ADC2_C4_PIN,  ADC2_C4_GROUP,  ADC_CHANNEL_4 },
        { ADC2_C5_PIN,  ADC2_C5_GROUP,  ADC_CHANNEL_5 },
        { ADC2_C6_PIN,  ADC2_C6_GROUP,  ADC_CHANNEL_6 },
        { ADC2_C7_PIN,  ADC2_C7_GROUP,  ADC_CHANNEL_7 },
        { ADC2_C8_PIN,  ADC2_C8_GROUP,  ADC_CHANNEL_8 },
        { ADC2_C9_PIN,  ADC2_C9_GROUP,  ADC_CHANNEL_9 },
        { ADC2_C10_PIN,  ADC2_C10_GROUP,  ADC_CHANNEL_10 },
        { ADC2_C11_PIN,  ADC2_C11_GROUP,  ADC_CHANNEL_11 },
        { ADC2_C12_PIN,  ADC2_C12_GROUP,  ADC_CHANNEL_12 },
        { ADC2_C13_PIN,  ADC2_C13_GROUP,  ADC_CHANNEL_13 },
        { ADC2_C14_PIN,  ADC2_C14_GROUP,  ADC_CHANNEL_14 },
        { ADC2_C15_PIN,  ADC2_C15_GROUP,  ADC_CHANNEL_15 }
    },
    { 
        { ADC3_C0_PIN,  ADC3_C0_GROUP,  ADC_CHANNEL_0  },
        { ADC3_C1_PIN,  ADC3_C1_GROUP,  ADC_CHANNEL_1  },
        { ADC3_C2_PIN,  ADC3_C2_GROUP,  ADC_CHANNEL_2  },
        { ADC3_C3_PIN,  ADC3_C3_GROUP,  ADC_CHANNEL_3  },
        { ADC3_C4_PIN,  ADC3_C4_GROUP,  ADC_CHANNEL_4  },
        { ADC3_C5_PIN,  ADC3_C5_GROUP,  ADC_CHANNEL_5  },
        { ADC3_C6_PIN,  ADC3_C6_GROUP,  ADC_CHANNEL_6  },
        { ADC3_C7_PIN,  ADC3_C7_GROUP,  ADC_CHANNEL_7  },
        { ADC3_C8_PIN,  ADC3_C8_GROUP,  ADC_CHANNEL_8  },
        { ADC3_C9_PIN,  ADC3_C9_GROUP,  ADC_CHANNEL_9  },
        { ADC3_C10_PIN,  ADC3_C10_GROUP,  ADC_CHANNEL_10 },
        { ADC3_C11_PIN,  ADC3_C11_GROUP,  ADC_CHANNEL_11 },
        { ADC3_C12_PIN,  ADC3_C12_GROUP,  ADC_CHANNEL_12 },
        { ADC3_C13_PIN,  ADC3_C13_GROUP,  ADC_CHANNEL_13 },
        { ADC3_C14_PIN,  ADC3_C14_GROUP,  ADC_CHANNEL_14 },
        { ADC3_C15_PIN,  ADC3_C15_GROUP,  ADC_CHANNEL_15 }
    }
};

struct adc_s adcs [ ADC_N ];

/* Local function prototypes */
// offset is where avg starts
// sample is how to increment loop
// len is how many elements to avg
// for odds in second half of array: average( 1, 2, len/4, array)
uint32_t average ( uint32_t offset, uint32_t sample, uint32_t len , uint32_t buffer[ADC_BUFFER_LENGTH])
{
    uint32_t i, avg = 0;
    
    for ( i = 0; i < len; i++ )
    {
        avg = avg + buffer[offset + i * sample];
    }
    return avg/len;
}

/* 
 * Function implementations
 */

StatusInt ADC_Init ( uint8_t adc, uint8_t channel1, uint8_t channel2, uint32_t sampling_time)
{
    StatusInt status = STATUSINT_ERROR_UNKNOWN;
    HAL_StatusTypeDef hal_status;
    ADC_InitTypeDef ADC_init_structure; //Structure for adc confguration
    ADC_ChannelConfTypeDef ADC_init_conf; //Structure for adc confguration
    ADC_ChannelConfTypeDef ADC_init_conf2; //Structure for adc confguration
    DMA_InitTypeDef DMA_init_structure;
    
    /* Initialize GPIO */
    PIO_Init( channel_info[ adc ][ channel1 ].pin,
              channel_info[ adc ][ channel1 ].group,
              GPIO_MODE_ANALOG, GPIO_NOPULL,
              GPIO_SET_AF_NONE );
     
    PIO_Init( channel_info[ adc ][ channel2 ].pin,
              channel_info[ adc ][ channel2 ].group,
              GPIO_MODE_ANALOG, GPIO_NOPULL,
              GPIO_SET_AF_NONE );
    
    /* Set default values to err */
    adcs[ adc ].channel1_value = ADC_MIN;
    adcs[ adc ].channel2_value = ADC_MIN;
    adcs[ adc ].err_flag = 0;
     
    /* Initialize ADC clock, set channel, set instances, NVIC priorities */
    switch ( adc )
    {
        case ADC_1:
            __HAL_RCC_ADC1_CLK_ENABLE();
            DMA_init_structure.Channel = DMA_CHANNEL_0;
            adcs[ adc ].dma.Instance = DMA2_Stream0;
            adcs[ adc ].handle.Instance = ADC1;
            HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0x1, 0x1);
            HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
            break;
        case ADC_2:
            __HAL_RCC_ADC2_CLK_ENABLE();
            DMA_init_structure.Channel = DMA_CHANNEL_1;
            adcs[ adc ].dma.Instance = DMA2_Stream2;
            adcs[ adc ].handle.Instance = ADC2;
            HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0x1, 0x1);
            HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
            break;
        case ADC_3:
            __HAL_RCC_ADC3_CLK_ENABLE();
            DMA_init_structure.Channel = DMA_CHANNEL_2;
            adcs[ adc ].dma.Instance = DMA2_Stream1;
            adcs[ adc ].handle.Instance = ADC3;
            HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0x1, 0x1);
            HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
            break;
        default:
            return STATUSINT_ERROR_BADARG;
    }
    
    /* DMA structure configuration */
    __HAL_RCC_DMA2_CLK_ENABLE();
   
    DMA_init_structure.Direction = DMA_PERIPH_TO_MEMORY;
    DMA_init_structure.PeriphInc = DMA_PINC_DISABLE;
    DMA_init_structure.MemInc = DMA_MINC_ENABLE;
    DMA_init_structure.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    DMA_init_structure.MemDataAlignment = DMA_MDATAALIGN_WORD;
    DMA_init_structure.Mode = DMA_CIRCULAR;
    DMA_init_structure.Priority = DMA_PRIORITY_HIGH;
    DMA_init_structure.FIFOMode = DMA_FIFOMODE_DISABLE;
    DMA_init_structure.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    DMA_init_structure.MemBurst = DMA_MBURST_SINGLE;
    DMA_init_structure.PeriphBurst = DMA_PBURST_SINGLE;
    
    adcs[ adc ].dma.Init = DMA_init_structure;
    hal_status = HAL_DMA_Init(&adcs[ adc ].dma);
    
     
    /* ADC structure configuration */
    HAL_ADC_DeInit( &adcs[ adc ].handle );
    ADC_init_structure.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    ADC_init_structure.Resolution = ADC_RESOLUTION_12B;
    ADC_init_structure.DataAlign = ADC_DATAALIGN_RIGHT;
    ADC_init_structure.ScanConvMode = ENABLE;
    ADC_init_structure.EOCSelection = DISABLE;
    ADC_init_structure.ContinuousConvMode = ENABLE;
    ADC_init_structure.DMAContinuousRequests = ENABLE;
    ADC_init_structure.NbrOfConversion = 2;
    ADC_init_structure.DiscontinuousConvMode = DISABLE;
    ADC_init_structure.NbrOfDiscConversion = 1;
    ADC_init_structure.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    ADC_init_structure.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    
    /* ADC Handle config */
    adcs[ adc ].handle.Init = ADC_init_structure;

    __HAL_LINKDMA( &adcs[ adc ].handle, DMA_Handle, adcs[ adc ].dma);

    /* NVIC Config */
    HAL_NVIC_SetPriority(ADC_IRQn, 0x4, 0x4);
    HAL_NVIC_EnableIRQ(ADC_IRQn);

    /* Initialize ADC with the above configuration */
    if ( hal_status == HAL_OK )
    {
        hal_status = HAL_ADC_Init( &adcs[ adc ].handle );
    }
    
    /* Enable Handle */
    __HAL_ADC_ENABLE( &adcs[ adc ].handle );
    
    /* Enable Channel1 if set */
    if ( hal_status == HAL_OK && channel1 != ADC_NO_CHANNEL)
    {          
        ADC_init_conf.Channel = channel_info[ adc ][ channel1 ].channel;
        ADC_init_conf.Offset = 0;
        ADC_init_conf.Rank = 1;
        ADC_init_conf.SamplingTime = sampling_time;
        hal_status = HAL_ADC_ConfigChannel( &adcs[ adc ].handle, &ADC_init_conf );
    }
    
    /* Enable Channel2 if set */
    if ( hal_status == HAL_OK && channel2 != ADC_NO_CHANNEL)
    {
        ADC_init_conf2.Channel = channel_info[ adc ][ channel2 ].channel;
        ADC_init_conf2.Offset = 0;
        ADC_init_conf2.Rank = 2;
        ADC_init_conf2.SamplingTime = sampling_time;
        hal_status = HAL_ADC_ConfigChannel( &adcs[ adc ].handle, &ADC_init_conf2 );
    }
   
    if ( hal_status == HAL_OK )
    {
        hal_status = HAL_ADC_Start_DMA( &adcs[ adc ].handle, adcs[ adc ].buffer , ADC_BUFFER_LENGTH );
    }
    
    if ( hal_status == HAL_OK )
    {
        status = STATUSINT_SUCCESS;
    }

    return status;
}

// This callback is called when last value in buffer filled
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
    if ( AdcHandle == &adcs[ ADC_1 ].handle )
    {
        adcs[ ADC_1 ].channel1_value  = average(ADC_BUFFER_LENGTH / 2, 2, ADC_BUFFER_LENGTH/4, adcs[ ADC_1 ].buffer);
        adcs[ ADC_1 ].channel2_value  = average(ADC_BUFFER_LENGTH / 2 + 1, 2, ADC_BUFFER_LENGTH/4, adcs[ ADC_1 ].buffer);
    }
    else if ( AdcHandle == &adcs[ ADC_2 ].handle )
    {
        adcs[ ADC_2 ].channel1_value  = average(ADC_BUFFER_LENGTH / 2, 2, ADC_BUFFER_LENGTH/4, adcs[ ADC_2 ].buffer);
        adcs[ ADC_2 ].channel2_value  = average(ADC_BUFFER_LENGTH / 2 + 1, 2, ADC_BUFFER_LENGTH/4, adcs[ ADC_2 ].buffer);
    }
    else if ( AdcHandle == &adcs[ ADC_3 ].handle )
    {
        adcs[ ADC_3 ].channel1_value  = average(ADC_BUFFER_LENGTH / 2, 2, ADC_BUFFER_LENGTH/4, adcs[ ADC_3 ].buffer);
        adcs[ ADC_3 ].channel2_value  = average(ADC_BUFFER_LENGTH / 2 + 1, 2, ADC_BUFFER_LENGTH/4, adcs[ ADC_3 ].buffer);
    }
}

void DMA2_Stream0_IRQHandler()
{
     HAL_DMA_IRQHandler(&adcs[ ADC_1 ].dma);
}

void DMA2_Stream2_IRQHandler()
{
     HAL_DMA_IRQHandler(&adcs[ ADC_2 ].dma);
} 

void DMA2_Stream1_IRQHandler()
{
     HAL_DMA_IRQHandler(&adcs[ ADC_3 ].dma);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *AdcHandle)
{
    if ( AdcHandle == &adcs[ ADC_1 ].handle )
    {
        adcs[ ADC_1 ].channel1_value  = ADC_MIN;
        adcs[ ADC_1 ].channel2_value  = ADC_MIN;
        adcs[ ADC_1 ].err_flag = 1;
    }
    else if ( AdcHandle == &adcs[ ADC_2 ].handle )
    {
        adcs[ ADC_2 ].channel1_value  = ADC_MIN;
        adcs[ ADC_2 ].channel2_value  = ADC_MIN;
        adcs[ ADC_2 ].err_flag = 1;
    }
    else if ( AdcHandle == &adcs[ ADC_3 ].handle )
    {
        adcs[ ADC_3 ].channel1_value  = ADC_MIN;
        adcs[ ADC_3 ].channel2_value  = ADC_MIN;
        adcs[ ADC_3 ].err_flag = 1;
    }
}

// note: if using one, just put the same ptr in both or a dummy ptr in one
StatusInt ADC_Read( uint32_t *value1, uint32_t *value2, uint8_t adc) 
{
    StatusInt retVal = STATUSINT_SUCCESS;

    // check if pointers valid
    if ( !value1 || !value2 )
    {
        retVal = STATUSINT_ERROR_BADARG;
    }

    // check if error callback happened on this adc
    if ( retVal == STATUSINT_SUCCESS )
    {
        if ( adcs[ adc ].err_flag ) {
            retVal = STATUSINT_ERROR_GENERAL;
        }
    }

    if ( retVal == STATUSINT_SUCCESS )
    {
        *value1 = adcs[ adc ].channel1_value;
        *value2 = adcs[ adc ].channel2_value;
    }

    // check if values within bounds
    if ( retVal == STATUSINT_SUCCESS )
    {
        if ((( *value1 <= ADC_MIN ) || ( *value2 <= ADC_MIN )) ||
            (( *value1 >= ADC_MAX ) || ( *value2 >= ADC_MAX )))
        {
            retVal = STATUSINT_ERROR_GENERAL;
        }
    }

    return retVal;
}
