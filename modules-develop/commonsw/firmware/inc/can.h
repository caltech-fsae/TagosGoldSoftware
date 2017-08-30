/*
 * CAN Driver
 * Caltech Racing 2016
 */
#ifndef CAN_H
#define CAN_H

#include "common.h" /* StatusInt */

/*
 * CAN Configuration values
 */
/* 250 kbps */
#define CAN_250K_PRESCALER 9
#define CAN_250K_BS1 CAN_BS1_12TQ
#define CAN_250K_BS2 CAN_BS2_5TQ
/* 125 kbps */
#define CAN_125K_PRESCALER 19
#define CAN_125K_BS1 CAN_BS1_12TQ
#define CAN_125K_BS2 CAN_BS2_4TQ
/* 50 kbps */
#define CAN_50K_PRESCALER 49
#define CAN_50K_BS1 CAN_BS1_11TQ
#define CAN_50K_BS2 CAN_BS2_4TQ

#define CAN_FILTER_SLAVE_BANK 14
#define CAN1_FILTER0_BANK 0
#define CAN1_FILTER1_BANK ( CAN1_FILTER0_BANK + 2 )
#define CAN2_FILTER0_BANK CAN_FILTER_SLAVE_BANK
#define CAN2_FILTER1_BANK ( CAN2_FILTER0_BANK + 2 )
#define CAN_FILTERS_PER_CHANNEL 2

/*
 * Actual CAN Configuration
 */
#if ( CAN1_SPEED == 125)
#define CAN1_PRESCALER (CAN_125K_PRESCALER)
#define CAN1_BS1 (CAN_125K_BS1)
#define CAN1_BS2 (CAN_125K_BS2)
#elif ( CAN1_SPEED == 250 )
#define CAN1_PRESCALER (CAN_250K_PRESCALER)
#define CAN1_BS1 (CAN_250K_BS1)
#define CAN1_BS2 (CAN_250K_BS2)
#elif ( CAN1_SPEED == 50 )
#define CAN1_PRESCALER (CAN_50K_PRESCALER)
#define CAN1_BS1 (CAN_50K_BS1)
#define CAN1_BS2 (CAN_50K_BS2)
#else
#define CAN1_PRESCALER (CAN_125K_PRESCALER)
#define CAN1_BS1 (CAN_125K_BS1)
#define CAN1_BS2 (CAN_125K_BS2)
#endif

#if ( CAN2_SPEED == 125 )
#define CAN2_PRESCALER (CAN_125K_PRESCALER)
#define CAN2_BS1 (CAN_125K_BS1)
#define CAN2_BS2 (CAN_125K_BS2)
#elif ( CAN2_SPEED == 250 )
#define CAN2_PRESCALER (CAN_250K_PRESCALER)
#define CAN2_BS1 (CAN_250K_BS1)
#define CAN2_BS2 (CAN_250K_BS2)
#elif ( CAN2_SPEED == 50 )
#define CAN2_PRESCALER (CAN_50K_PRESCALER)
#define CAN2_BS1 (CAN_50K_BS1)
#define CAN2_BS2 (CAN_50K_BS2)
#else
#define CAN2_PRESCALER (CAN_125K_PRESCALER)
#define CAN2_BS1 (CAN_125K_BS1)
#define CAN2_BS2 (CAN_125K_BS2)
#endif


#define CAN_MESSAGE_LENGTH 8

#define CAN_MAIL_QUEUE_SIZE 30
#define CAN_MAILING_Q_THREAD_PRIORITY (osPriorityRealtime)

#define CAN_TIMEOUT 20

/**
 * Received message type 
 */
typedef struct
{
    uint8_t data[ CAN_MESSAGE_LENGTH ];
    uint8_t length;
    uint32_t ID;
} CANRxStruct_t;

/**
 * Transceived message type 
 */
typedef struct
{
    uint8_t data[ CAN_MESSAGE_LENGTH ];
    uint8_t length;
    uint32_t ID;
} CANTxStruct_t;

/**
 * CAN Rx callback type 
 */
typedef void ( *CANRxCallBack_t )( CANRxStruct_t* rx_info );

/**
 * CAN Channels
 */
typedef enum
{
    CAN_CHANNEL_1 = 0,
    CAN_CHANNEL_2,
    CAN_CHANNEL_N
} CANChannel_t;


/**
 * @brief This function initializes the corresponding can controller
 * @param[in] CAN controller number (CANChannel_t)
 * @return: STATUSINT_SUCCESS if success, STATUSINT_ERROR_GENERAL otherwise
*/
StatusInt CAN_Init( CANChannel_t );


/**
 * @brief Register RxCallback to can controller fifo.
 * Note this function need to be called to enable rx interrupt
 * @param[in] CAN controller fifo number,
 * @param[in] Receive can message callback. This function will be called
 * when a corresponding msg is received at the fifo
 * @return: none.
*/
void CAN_RegisterCallback( CANChannel_t channel, CANRxCallBack_t f );

/**
 * @brief Send message from CAN cantroller with ID specify in CANTxStruct
 * @param[in] tx_info containing data, data length and destination ID
 * @param[in] can controller id (CANChannel_t)
*/
StatusInt CAN_SendMessage( const CANTxStruct_t* tx_info, CANChannel_t channel );


/**
 * @brief This Signal Handler should be called when received
 * a signal from CANx_RXx_IRQ_Handler.
*/
void CAN_HandleMessages( void );


#endif /* CAN_H */
