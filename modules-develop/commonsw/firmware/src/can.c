/*
 * CAN Driver
 * Caltech Racing 2016
 */

/* Standard libraries */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* STM libraries */
#ifdef LINT
#define _RTE_
#endif
#include <stm32f4xx_hal_can.h>
#include <stm32f4xx_hal_gpio.h>
#include <stm32f4xx_hal_rcc.h>
#include <stm32f4xx_hal_cortex.h>

/* RTX */
#include <cmsis_os.h>

/* FSAE firmwrae */
#include "common.h"
#include "can.h"
#include "fault.h"

/* Local function prototypes */
static void CAN1_mail_q_send_thread(void const* arg);
static void CAN2_mail_q_send_thread(void const* arg);

static StatusInt CAN_SendMessageExt(const CANTxStruct_t *tx_info, CANChannel_t channel);

/* Mail Queues Declaraton */
osMailQDef (CAN1MailQId, CAN_MAIL_QUEUE_SIZE, CANTxStruct_t);
osMailQId CAN1MailQId;
osMailQDef (CAN2MailQId, CAN_MAIL_QUEUE_SIZE, CANTxStruct_t);
osMailQId CAN2MailQId;

/* Thread Declaration for Mailing Q monitor*/
osThreadId tid_can1_mail_q;
osThreadId tid_can2_mail_q;
osThreadDef(CAN1_mail_q_send_thread, CAN_MAILING_Q_THREAD_PRIORITY, 1, 0);
osThreadDef(CAN2_mail_q_send_thread, CAN_MAILING_Q_THREAD_PRIORITY, 1, 0);

static osMailQId can_MailQIds[2];


/* Local types */
/* Timing data structure */
struct can_timing_s
{
    uint32_t prescaler;
    uint32_t bs1;
    uint32_t bs2;
};
struct can_filter_s
{
    uint8_t num;
    uint32_t mask;
    uint32_t id;
};
struct can_channel_s
{
    uint32_t pin_tx;
    GPIO_TypeDef* group_tx;
    uint32_t pin_rx;
    GPIO_TypeDef* group_rx;
    uint32_t alternate;
    CAN_TypeDef* instance;
    CAN_HandleTypeDef handle;
    CanTxMsgTypeDef txMsg;
    CanRxMsgTypeDef rxMsg;
    uint32_t fifo;
    IRQn_Type txIRQ;
    IRQn_Type rxIRQ;
    CANRxCallBack_t callback;
    struct can_filter_s filters[ CAN_FILTERS_PER_CHANNEL ];
    struct can_timing_s timing;
};

/* Local constants */
/* Helper macros for CAN_IRQFlags */
#define CAN_IRQ_FLAG_SET(val) ( CAN_IRQFlags = ( CAN_IRQFlags | ( 0x1 << val ) ) )
#define CAN_IRQ_FLAG_CLR(val) ( CAN_IRQFlags = ( CAN_IRQFlags & ~( (uint8_t)0x1 << val ) ) )

/* Local variables */
static uint8_t CAN_IRQFlags = 0;
static FaultId CAN_fault;

// Thread ID for IRQ signal handler
extern osMessageQId msgQ_id_IRQ;

static struct can_channel_s can_channels[ CAN_CHANNEL_N ] = {
        { CAN1_TX_GPIO_PINS, /* pins */
                CAN1_TX_GPIO_GROUP, /* group */
                CAN1_RX_GPIO_PINS, /* pins */
                CAN1_RX_GPIO_GROUP, /* group */
                GPIO_AF9_CAN1, /* alternate */
                CAN1, /* instance */
                {0}, /* handle */
                {0}, /* txMsg */
                {0}, /* rxMsg */
                CAN_FILTER_FIFO0, /* fifo */
                CAN1_TX_IRQn, /* txirq */
                CAN1_RX0_IRQn, /* rxirq */
                NULL,
                { { CAN1_FILTER0_BANK, CAN1_FILTER0_MASK, CAN1_FILTER0_ID },   /* filter[0] */
                        { CAN1_FILTER1_BANK, CAN1_FILTER1_MASK, CAN1_FILTER1_ID } }, /* filter[1] */
                { CAN1_PRESCALER, CAN1_BS1, CAN1_BS2 } /* timing */
        },
        { CAN2_TX_GPIO_PINS, /* pins */
                CAN2_TX_GPIO_GROUP, /* group */
                CAN2_RX_GPIO_PINS, /* pins */
                CAN2_RX_GPIO_GROUP, /* group */
                GPIO_AF9_CAN2,  /* alternate */
                CAN2, /* instance */
                {0}, /* handle */
                {0}, /* txMsg */
                {0}, /* rxMsg */
                CAN_FILTER_FIFO0, /* fifo */
                CAN2_TX_IRQn, /* txirq */
                CAN2_RX0_IRQn, /* rxirq */
                NULL,
                { { CAN2_FILTER0_BANK, CAN2_FILTER0_MASK, CAN2_FILTER0_ID },   /* filter[0] */
                        { CAN2_FILTER1_BANK, CAN2_FILTER1_MASK, CAN2_FILTER1_ID } }, /* filter[1] */
                { CAN2_PRESCALER, CAN2_BS1, CAN2_BS2 } /* timing */
        }
};



/*
 * Function implementations
 */

StatusInt CAN_Init( CANChannel_t channel )
{
    CAN_FilterConfTypeDef CAN_FilterStruct;
    HAL_StatusTypeDef status;
    int i;
    StatusInt retVal = STATUSINT_ERROR_GENERAL;
#ifdef CAN_RS_CONTROL
    PIO_Init( CAN1_RS_GPIO_PINS,
              CAN1_RS_GPIO_GROUP,
              GPIO_MODE_OUTPUT_OD,
              GPIO_NOPULL,
              GPIO_SET_AF_NONE);
    PIO_Init( CAN2_RS_GPIO_PINS,
              CAN2_RS_GPIO_GROUP,
              GPIO_MODE_OUTPUT_OD,
              GPIO_NOPULL,
              GPIO_SET_AF_NONE);
    HAL_GPIO_WritePin( CAN1_RS_GPIO_GROUP,
                           CAN1_RS_GPIO_PINS,
                           GPIO_PIN_SET );
    HAL_GPIO_WritePin( CAN2_RS_GPIO_GROUP,
                           CAN2_RS_GPIO_PINS,
                           GPIO_PIN_SET );
#endif

#ifdef CAN_SHDN_CONTROL
    PIO_Init( CAN1_SHDN_GPIO_PIN,
              CAN1_SHDN_GPIO_GROUP,
              GPIO_MODE_OUTPUT_PP,
              GPIO_NOPULL,
              GPIO_SET_AF_NONE);
    HAL_GPIO_WritePin( CAN1_SHDN_GPIO_GROUP,
               CAN1_SHDN_GPIO_PIN,
               GPIO_PIN_RESET );
#endif

    /* Initialize GPIO for Tx */
    PIO_Init( can_channels[ channel ].pin_tx,
              can_channels[ channel ].group_tx,
              GPIO_MODE_AF_PP,
              GPIO_NOPULL,
              can_channels[ channel ].alternate );
    /* Initialize GPIO for Rx*/
    PIO_Init( can_channels[ channel ].pin_rx,
              can_channels[ channel ].group_rx,
              GPIO_MODE_AF_PP,
              GPIO_NOPULL,
              can_channels[ channel ].alternate );


    /* Enable CAN clocks */
    if ( channel == CAN_CHANNEL_2 )
    {
        __HAL_RCC_CAN2_CLK_ENABLE();
    }
    __HAL_RCC_CAN1_CLK_ENABLE();    /* Both channels require CAN1 clock */


    /* Initialize CAN Controller */
    can_channels[ channel ].handle.Instance = can_channels[ channel ].instance;
    can_channels[ channel ].handle.pTxMsg = &( can_channels[ channel ].txMsg );
    can_channels[ channel ].handle.pRxMsg = &( can_channels[ channel ].rxMsg );
    can_channels[ channel ].handle.pTxMsg->RTR = CAN_RTR_DATA;
    can_channels[ channel ].handle.pTxMsg->IDE = CAN_ID_STD;
#ifdef CAN_LOOPACK
    can_channels[ channel ].handle.Init.Mode = CAN_MODE_SILENT_LOOPBACK;
#else
    can_channels[ channel ].handle.Init.Mode = CAN_MODE_NORMAL;
#endif

    /* Set bit timing. Sampling point = BS1/(BS2 + BS1) */
    can_channels[ channel ].handle.Init.Prescaler = can_channels[ channel ].timing.prescaler;
    can_channels[ channel ].handle.Init.SJW = CAN_SJW_2TQ;
    can_channels[ channel ].handle.Init.BS1 = can_channels[ channel ].timing.bs1;
    can_channels[ channel ].handle.Init.BS2 = can_channels[ channel ].timing.bs2;
    can_channels[ channel ].handle.Init.TTCM = (int)DISABLE; /* Time-triggered communication */
    can_channels[ channel ].handle.Init.ABOM = (int)ENABLE;  /* Automatic bus-off management */
    can_channels[ channel ].handle.Init.AWUM = (int)DISABLE; /* Automatic wake-up mode */
    can_channels[ channel ].handle.Init.NART = (int)DISABLE; /* Non-automatic retransmission mode */
    can_channels[ channel ].handle.Init.RFLM = (int)DISABLE; /* Receive FIFO locked mode */
    can_channels[ channel ].handle.Init.TXFP = (int)DISABLE; /* Transmit FIFO priority */

    status = HAL_CAN_DeInit( &can_channels[ channel ].handle );

    if ( status == HAL_OK )
    {
        status = HAL_CAN_Init( &can_channels[ channel ].handle ); /* Init CAN */
    }

    if ( status == HAL_OK )
    {
        for ( i = 0; i < CAN_FILTERS_PER_CHANNEL; i++ ){
            /* Configure filter */
            CAN_FilterStruct.FilterIdHigh = hiword( can_channels[ channel ].filters[ i ].id );
            CAN_FilterStruct.FilterIdLow = loword( can_channels[ channel ].filters[ i ].id );
            CAN_FilterStruct.FilterMaskIdHigh =  hiword( can_channels[ channel ].filters[ i ].mask );
            CAN_FilterStruct.FilterMaskIdLow = loword( can_channels[ channel ].filters[ i ].mask );
            CAN_FilterStruct.FilterFIFOAssignment = can_channels[ channel ].fifo;
            CAN_FilterStruct.FilterNumber = can_channels[ channel ].filters[ i ].num;
            CAN_FilterStruct.FilterMode = CAN_FILTERMODE_IDMASK;  /* Identifier mask mode */
            CAN_FilterStruct.FilterScale = CAN_FILTERSCALE_32BIT; /* 32bit ID filter */
            CAN_FilterStruct.FilterActivation = (int)ENABLE;           /* Enable this filter */
            CAN_FilterStruct.BankNumber = CAN_FILTER_SLAVE_BANK;

            /* Initialize filter */
            status = HAL_CAN_ConfigFilter( &can_channels[ channel ].handle, &CAN_FilterStruct ); /* Initialize filter */
            if ( status != HAL_OK )
            {
                break;
            }
        }
    }

    if ( status == HAL_OK )
    {
        /* Enable CAN RX interrupt */
        HAL_NVIC_SetPriority( can_channels[ channel ].rxIRQ, 1, 1 );
        HAL_NVIC_EnableIRQ( can_channels[ channel ].rxIRQ );

        /* And TX in debug mode */
        HAL_NVIC_SetPriority( can_channels[ channel ].txIRQ, 1, 1 );
        HAL_NVIC_EnableIRQ( can_channels[ channel ].txIRQ );
        retVal = STATUSINT_SUCCESS;
    }

    // initialize can message management thread and mailing queue
    if ( channel == CAN_CHANNEL_1 )
    {
        can_MailQIds[CAN_CHANNEL_1]=osMailCreate(osMailQ(CAN1MailQId), NULL);
        if (can_MailQIds[CAN_CHANNEL_1] != NULL)
        {
            tid_can1_mail_q = osThreadCreate(osThread(CAN1_mail_q_send_thread), NULL);
            retVal = STATUSINT_SUCCESS;
        }else{
            retVal = STATUSINT_ERROR_GENERAL;
        }
    }
    else if ( channel == CAN_CHANNEL_2 )
    {
        can_MailQIds[CAN_CHANNEL_2]=osMailCreate(osMailQ(CAN2MailQId), NULL);
        if (can_MailQIds[CAN_CHANNEL_2] != NULL){
            tid_can2_mail_q = osThreadCreate(osThread(CAN2_mail_q_send_thread), NULL);
            retVal = STATUSINT_SUCCESS;
        }else{
            retVal = STATUSINT_ERROR_GENERAL;
        }
    }

#ifndef FIRMWARE_ONLY
    if (retVal == STATUSINT_SUCCESS) {
        retVal = fault_add(&CAN_fault, false, false);
    }
#endif
    return retVal;
}

void CAN_RegisterCallback( CANChannel_t channel, CANRxCallBack_t f )
{
    /* Save callback */
    can_channels[ channel ].callback = f;

    __HAL_CAN_ENABLE_IT( &can_channels[ channel ].handle, CAN_IT_FMP0 );
}

StatusInt CAN_SendMessage(const CANTxStruct_t* tx_info, CANChannel_t channel){
    CANTxStruct_t *mptr;
    StatusInt status;
    osStatus os_status;
    mptr = osMailAlloc(can_MailQIds[channel], osWaitForever);

    memcpy( (uint8_t*)( mptr->data ),
            tx_info->data,
            tx_info->length );
    mptr->length = tx_info->length;
    mptr->ID = tx_info->ID;
    os_status = osMailPut(can_MailQIds[channel], mptr);
    if ( os_status == osOK ) {
        status = STATUSINT_SUCCESS;
    } else {
        status = STATUSINT_ERROR_GENERAL;
    }
    return status;
}


/*
 * Private
 */
void CAN1_RX0_IRQHandler( void )
{
    /* Handle the IRQ */
    HAL_CAN_IRQHandler( &can_channels[ CAN_CHANNEL_1 ].handle );
    CAN_IRQ_FLAG_SET( (int)CAN_CHANNEL_1 );

    /* Deal with callback */
    osMessagePut(msgQ_id_IRQ, IRQ_TYPE_CAN, 0);

    /* Re-enable interrupt */
    __HAL_CAN_ENABLE_IT( &can_channels[ CAN_CHANNEL_1 ].handle, CAN_IT_FMP0 );
}

void CAN2_RX0_IRQHandler( void )
{
    /* Handle the IRQ */
    HAL_CAN_IRQHandler( &can_channels[ CAN_CHANNEL_2 ].handle );
    CAN_IRQ_FLAG_SET( (int)CAN_CHANNEL_2 );

    /* Deal with callback */
    osMessagePut(msgQ_id_IRQ, IRQ_TYPE_CAN, 0);

    /* Re-enable interrupt */
    __HAL_CAN_ENABLE_IT( &can_channels[ CAN_CHANNEL_2 ].handle, CAN_IT_FMP0 );
}

void CAN_HandleMessages( void )
{
    uint8_t IRQ_flags = CAN_IRQFlags;
    CANRxStruct_t rx_info;
    const CanRxMsgTypeDef* msg;

    uint8_t msk = 0x1;
    uint8_t c;

    for ( c = 0; c < (uint8_t)CAN_CHANNEL_N; c++ )
    {
        if ( ( IRQ_flags & msk ) != 0 )
        {
            msg = &can_channels[ c ].rxMsg;
            memcpy( rx_info.data,
                    (const uint8_t*) msg->Data,
                    msg->DLC );
            rx_info.length = msg->DLC;
            rx_info.ID = msg->StdId;
            CAN_IRQ_FLAG_CLR( ( c * (uint8_t)CAN_FILTERS_PER_CHANNEL ) );
            ( can_channels[ c ].callback )( &( rx_info ) );
            break;
        }
        msk = msk << 1;
    }
}

static StatusInt CAN_SendMessageExt(const CANTxStruct_t *tx_info, CANChannel_t channel)
{
    HAL_StatusTypeDef status;
    StatusInt retVal = STATUSINT_SUCCESS;
    can_channels[ channel ].handle.pTxMsg->DLC = tx_info->length;
    can_channels[ channel ].handle.pTxMsg->StdId = tx_info->ID;

    /* Copy transmit data to can_handle's TxMsg data field */
    memcpy( (uint8_t*)( can_channels[ channel ].handle.pTxMsg->Data ),
            tx_info->data,
            tx_info->length );

    /* And send it */
    status = HAL_CAN_Transmit( &can_channels[ channel ].handle, CAN_TIMEOUT );
    if ( status != HAL_OK )
    {
        retVal = STATUSINT_ERROR_GENERAL;
    }

    return retVal;
}

static void CAN1_mail_q_send_thread(void const* arg){
    CANTxStruct_t* tx_q_info;
    osEvent evt;
    osStatus os_status;
    StatusInt status;
    CANChannel_t channel = CAN_CHANNEL_1;
    for(;;){
        evt = osMailGet(can_MailQIds[channel], osWaitForever);
        if (evt.status == osEventMail){
            tx_q_info = (CANTxStruct_t *)evt.value.p;
            status = CAN_SendMessageExt(tx_q_info,channel);
#ifndef FIRMWARE_ONLY
            if (status != STATUSINT_SUCCESS){
                fault_trigger( &CAN_fault );
            }
#endif
            os_status = osMailFree(can_MailQIds[channel], tx_q_info);
            if (os_status != osOK) {
                // TODO handle osErrorValue or osErrorParameter
            }
        }
    }
}

static void CAN2_mail_q_send_thread(void const* arg){
    CANTxStruct_t* tx_q_info;
    osEvent evt;
    osStatus os_status;
    StatusInt status;
    CANChannel_t channel = CAN_CHANNEL_2;
    for(;;){
        evt = osMailGet(can_MailQIds[channel], osWaitForever);
        if (evt.status == osEventMail){
            tx_q_info = (CANTxStruct_t *)evt.value.p;
            status = CAN_SendMessageExt(tx_q_info,channel);
#ifndef FIRMWARE_ONLY
            if (status != STATUSINT_SUCCESS){
                fault_trigger( &CAN_fault );
            }
#endif
            os_status = osMailFree(can_MailQIds[channel], tx_q_info);
            if (os_status != osOK) {
                // TODO handle osErrorValue or osErrorParameter
            }
        }
    }
}

void CAN1_TX_IRQHandler( void )
{
    HAL_CAN_IRQHandler( &can_channels[ CAN_CHANNEL_1 ].handle );
}

void CAN2_TX_IRQHandler( void )
{
    HAL_CAN_IRQHandler( &can_channels[ CAN_CHANNEL_2 ].handle );
}
