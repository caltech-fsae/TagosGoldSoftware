/*
 * I2C Driver
 * Caltech Racing 2016
 */

/* Standard libraries */
#include <string.h>
/* STM libraries */
#ifdef LINT
    #define _RTE_
    #include <stm32f4xx.h>
#endif
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_i2c.h>

/* RTX */
#include <cmsis_os.h>
/* FSAE Firmware */
#include "i2c.h"

/* Local types */
/* I2C Channel Information */
struct i2c_channel_s 
{
    GPIO_TypeDef* scl_group;    /* SCL GPIO pin group */
    uint16_t scl_pin;           /* SCL GPIO pin number */
    GPIO_TypeDef* sda_group;    /* SDA GPIO pin group */
    uint16_t sda_pin;           /* SDA GPIO pin number */
    uint8_t alternate;          /* GPIO alternate pin function */
    I2C_TypeDef* instance;      /* I2C HAL handle instance */
    I2C_HandleTypeDef handle;   /* I2C Hal handle */
    I2CInfoStruct_t info;       /* I2C channel info struct */
    I2CRxCallBack_t rx_callback;    /* event callback function */
    IRQn_Type EV_IRQ;               /* I2C EV IRQn */
    IRQn_Type ER_IRQ;               /* I2C ER IRQn */
};

/* Local constants */


/* Helper macros for CAN_IRQFlags */
#define I2C_TX_IRQ_FLAG_SET(val) ( I2C_TX_IRQFlags = ( I2C_TX_IRQFlags | ( 0x1 << val ) ) )
#define I2C_TX_IRQ_FLAG_CLR(val) ( I2C_TX_IRQFlags = ( I2C_TX_IRQFlags & ~( (uint8_t)0x1 << val ) ) )
#define I2C_RX_IRQ_FLAG_SET(val) ( I2C_RX_IRQFlags = ( I2C_RX_IRQFlags | ( 0x1 << val ) ) )
#define I2C_RX_IRQ_FLAG_CLR(val) ( I2C_RX_IRQFlags = ( I2C_RX_IRQFlags & ~( (uint8_t)0x1 << val ) ) )
#define I2C_ER_IRQ_FLAG_SET(val) ( I2C_ER_IRQFlags = ( I2C_ER_IRQFlags | ( 0x1 << val ) ) )
#define I2C_ER_IRQ_FLAG_CLR(val) ( I2C_Er_IRQFlags = ( I2C_ER_IRQFlags & ~( (uint8_t)0x1 << val ) ) )

/* extern declaration */
// Message Q ID for IRQ thread Handler
extern osMessageQId msgQ_id_IRQ;

/* Local variables */

/* IRQ flags for recording the current IRQ status */
static uint8_t I2C_TX_IRQFlags = 0;
static uint8_t I2C_RX_IRQFlags = 0;
static uint8_t I2C_ER_IRQFlags = 0;


static struct i2c_channel_s i2c_channels[ I2C_CHANNEL_N ] = 
{
    { I2C1_SCL_GPIO_GROUP, I2C1_SCL_GPIO_PIN, I2C1_SDA_GPIO_GROUP, I2C1_SDA_GPIO_PIN, GPIO_AF4_I2C1, I2C1, {0}, {0}, NULL, I2C1_EV_IRQn, I2C1_ER_IRQn},
    { I2C2_SCL_GPIO_GROUP, I2C2_SCL_GPIO_PIN, I2C2_SDA_GPIO_GROUP, I2C2_SDA_GPIO_PIN, GPIO_AF4_I2C2, I2C2, {0}, {0}, NULL, I2C2_EV_IRQn, I2C1_ER_IRQn},
    { I2C3_SCL_GPIO_GROUP, I2C3_SCL_GPIO_PIN, I2C3_SDA_GPIO_GROUP, I2C3_SDA_GPIO_PIN, GPIO_AF4_I2C3, I2C3, {0}, {0}, NULL, I2C3_EV_IRQn, I2C1_ER_IRQn}
};

static StatusInt init_error = STATUSINT_SUCCESS;



/* Local Function implementation */
int I2C_TX_IRQ_FLAG_Get(int i2c_num){
    return ((I2C_TX_IRQFlags & ( 0x1 << i2c_num )) >> i2c_num);
}


StatusInt I2C_ErrorHandle(int i2c_num){
    if(__HAL_I2C_GET_FLAG(&(i2c_channels[i2c_num].handle), I2C_FLAG_BERR)){
        __HAL_I2C_CLEAR_FLAG(&(i2c_channels[i2c_num].handle), I2C_FLAG_BERR);
    }
    if(__HAL_I2C_GET_FLAG(&(i2c_channels[i2c_num].handle), I2C_FLAG_BUSY)){
        i2c_channels[i2c_num].handle.Instance->CR1 |= I2C_CR1_SWRST ;
        i2c_channels[i2c_num].handle.Instance->CR1 &= ~I2C_CR1_SWRST;
    }
    return STATUSINT_SUCCESS;
}


/* 
 * Function implementations
 */

/*
 * @brief This function overwrite the default(which did nothing) MSP init function
 * that will be called at the start of HAL_I2C_Init
 */
void HAL_I2C_MspInit( I2C_HandleTypeDef *hi2c )
{
    uint8_t i2c;
    for ( i2c = 0; i2c < I2C_CHANNEL_N; i2c++ )
    {
        if ( hi2c == &i2c_channels[ i2c ].handle )
        {
            break;
        }
    }

    if (i2c >= I2C_CHANNEL_N) {
        init_error = STATUSINT_ERROR_BADARG;
        return;
    }

    PIO_Init( i2c_channels[ i2c ].sda_pin,
              i2c_channels[ i2c ].sda_group,
              GPIO_MODE_AF_OD,
              GPIO_PULLUP,
              i2c_channels[ i2c ].alternate );
    PIO_Init( i2c_channels[ i2c ].scl_pin,
              i2c_channels[ i2c ].scl_group,
              GPIO_MODE_AF_OD,
              GPIO_PULLUP,
              i2c_channels[ i2c ].alternate );

    /* IMPORTANT
     * RCC MUST BE INIT <b>AFTER</b> the PIOs are init.
     */
    switch ( i2c )
    {
        case I2C_1:
            __HAL_RCC_I2C1_CLK_ENABLE();
            break;
        case I2C_2:
            __HAL_RCC_I2C2_CLK_ENABLE();
            break;
        case I2C_3:
            __HAL_RCC_I2C3_CLK_ENABLE();
            break;
        default:
            init_error = STATUSINT_ERROR_BADARG;
            break;
    }
}

/**
  * @brief: initialize i2c channel specified by i2c channel number
  *     channel from 0 to 2 exists. 
  * @param[in] i2c_num: i2c channel number to initialize. (0 ~ 2)
  * @param[in] IT: indicate if using I2C operating mode. I2C_MODE_IT or I2C_MODE_POLLING
  * @return: StatusInt
  */
StatusInt I2C_Init( uint8_t i2c_num, uint8_t mode )
{
    StatusInt retVal = STATUSINT_ERROR_GENERAL;
    HAL_StatusTypeDef status;
    status = HAL_I2C_DeInit( &i2c_channels[ i2c_num ].handle );

    i2c_channels[ i2c_num ].handle.Instance = i2c_channels[ i2c_num ].instance;
    i2c_channels[ i2c_num ].handle.Init.ClockSpeed = I2C_CLOCKSPEED;
    i2c_channels[ i2c_num ].handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    i2c_channels[ i2c_num ].handle.Init.OwnAddress1 = 0;
    i2c_channels[ i2c_num ].handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2c_channels[ i2c_num ].handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c_channels[ i2c_num ].handle.Init.OwnAddress2 = 0;
    i2c_channels[ i2c_num ].handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c_channels[ i2c_num ].handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if ( status == HAL_OK ) {
        status = HAL_I2C_Init( &i2c_channels[ i2c_num ].handle );
    }

    if ((status == HAL_OK )&&(mode == I2C_MODE_IT)){
        // Enable NVIC interrupt if using I2C_MODE_IT
        HAL_NVIC_SetPriority(i2c_channels[i2c_num].EV_IRQ, 1,1);
        HAL_NVIC_EnableIRQ(i2c_channels[i2c_num].EV_IRQ);
        HAL_NVIC_SetPriority(i2c_channels[i2c_num].ER_IRQ, 1,1);
        HAL_NVIC_EnableIRQ(i2c_channels[i2c_num].ER_IRQ);
    }

    if ( status == HAL_OK )
    {
        if ( init_error == STATUSINT_SUCCESS ) {
            retVal = STATUSINT_SUCCESS;
        } else {
            retVal = init_error;
        }
    }
    return retVal;
}

/**
  * @brief: writes "size" number of bytes starting from "pData" pointer
  *     to "devAddress" through I2C channel specified by "i2c_num"
  * @param[in] i2c_num: i2c channel number to write to.
  * @param[in] devAddress: 7 bit device address to write to. (not including the
  * R/W bit)
  * @param[in] pData: pointer to buffer that we need to write to I2C.
  * @param[in] size: number of bytes to write to
  * @return: StatusInt
  */
StatusInt I2C_Write( uint8_t i2c_num, uint16_t devAddress, uint8_t *pData, uint16_t size )
{
    StatusInt retVal = STATUSINT_ERROR_GENERAL;
    HAL_StatusTypeDef status;


    if ( i2c_num < I2C_CHANNEL_N ) {
        status = HAL_I2C_Master_Transmit( &i2c_channels[ i2c_num ].handle,
                                          ( devAddress << 1 ),
                                          pData,
                                          size,
                                          I2C_TIMEOUT );

        if ( status == HAL_OK )
        {
            retVal = STATUSINT_SUCCESS;
        }
    } else {
        retVal = STATUSINT_ERROR_BADARG;
    }
    return retVal;
}

/**
  * @brief: reads "size" number of bytes from "devAddress" through I2C channel
  *     specified by "i2c_num" and further stores it in "pData"
  * @param[in] i2c_num: i2c channel number to read from.
  * @param[in] devAddress: 7 bit device address to read from. (not including the
  * R/W bit)
  * @param[in] pData: pointer to buffer to move data that we read from I2C.
  * @param[in] size: number of bytes to read
  * @return: StatusInt
  */
StatusInt I2C_Read( uint8_t i2c_num, uint16_t devAddress, uint8_t *pData, uint16_t size )
{
    StatusInt retVal = STATUSINT_ERROR_GENERAL;
    HAL_StatusTypeDef status;

    if ( i2c_num < I2C_CHANNEL_N ) {
        status = HAL_I2C_Master_Receive( &i2c_channels[ i2c_num ].handle,
                                         ( devAddress << 1 ),
                                         pData,
                                         size,
                                         I2C_TIMEOUT );

        if ( status == HAL_OK )
        {
            retVal = STATUSINT_SUCCESS;
        }
    } else {
        retVal = STATUSINT_ERROR_BADARG;
    }
    return retVal;
}

/*
  * @brief: reads "size" number of bytes from "devAddress" through I2C channel
  *     specified by "i2c_num" and further stores it in "pData" in interrupt mode.
  *     Note the function will try I2C_MAX_BUSY_TRY times before returning error
  *     in case I2C_CHANNEL was still occupied by previous Read/Write.
  *
  *     To use READ_IT, user need to install callback through I2C_RegisterCallback.
  *     The registered Callback will be called when the data is received along with
  *     the I2CInfoStruct containg the state as I2C_STATE_RECEIVE.
  * @param[in] i2c_num: i2c channel number to read from.
  * @param[in] devAddress: 7 bit device address to read from. (not including the
  * R/W bit)
  * @param[in] pData: pointer to buffer to move data that we read from I2C.
  * @param[in] size: number of bytes to read
  * @return: StatusInt
 */
StatusInt I2C_Read_IT( uint8_t i2c_num, uint16_t devAddress, uint8_t *pData, uint16_t size )
{
    StatusInt retVal = STATUSINT_ERROR_GENERAL;
    HAL_StatusTypeDef status;

    int count = 0;
    if ( i2c_num < I2C_CHANNEL_N ) {
        i2c_channels[i2c_num].info.address = devAddress;
        i2c_channels[i2c_num].info.length = size;
        do{
            // Keep trying if return HAL_BUSY. Try up till I2C_MAX_BUSY_TRY times
        status = HAL_I2C_Master_Receive_IT( &i2c_channels[ i2c_num ].handle,
                                         ( i2c_channels[i2c_num].info.address << 1 ),
                                         (i2c_channels[i2c_num].info.data),
                                         size);
        osDelay(100);
        count++;
        }while(status == HAL_BUSY && count < I2C_MAX_BUSY_TRY);
        if ( status == HAL_OK )
        {
            retVal = STATUSINT_SUCCESS;
        }
    } else {
        retVal = STATUSINT_ERROR_BADARG;
    }
    return retVal;
}


/**
  * @brief: reads "size" number of bytes from "devAddress" through I2C channel
  *     specified by "i2c_num" and further stores it in "pData"
  *
  *     Note the function will try I2C_MAX_BUSY_TRY times before returning error
  *     in case I2C_CHANNEL was still occupied by previous Read/Write.
  *
  *     To use READ_IT, user need to install callback through I2C_RegisterCallback.
  *     The registered Callback will be called when the data is received along with
  *     the I2CInfoStruct containg the state as I2C_STATE_TRANSMIT.
  * @param[in] i2c_num: i2c channel number to write to.
  * @param[in] i2c_num: i2c channel number to write to.
  * @param[in] devAddress: 7 bit device address to read from. (not including the
  * R/W bit)
  * @param[in] pData: pointer to buffer that stores the data to write to I2C.
  * @param[in] size: number of bytes to read
  * @return: StatusInt
  */
StatusInt I2C_Write_IT( uint8_t i2c_num, uint16_t devAddress, uint8_t *pData, uint16_t size )
{
    StatusInt retVal = STATUSINT_ERROR_GENERAL;
    HAL_StatusTypeDef status;
    int count = 0;
    
    if ( i2c_num < I2C_CHANNEL_N ) {
        i2c_channels[i2c_num].info.address = devAddress;
        i2c_channels[i2c_num].info.length = size;
        memcpy(i2c_channels[i2c_num].info.data, pData, size);
        // keep trying until I2C_MAX_BUSY_TRY time
        do {
            status = HAL_I2C_Master_Transmit_IT(&i2c_channels[i2c_num].handle,
                                                (i2c_channels[i2c_num].info.address << 1),
                                                (i2c_channels[i2c_num].info.data),
                                                size);
            count++;
        } while (status == HAL_BUSY && count < I2C_MAX_BUSY_TRY);

        if (status == HAL_OK) {
            retVal = STATUSINT_SUCCESS;
        }

    } else {
        retVal = STATUSINT_ERROR_BADARG;
    }

    return retVal;
}

/**
 * @brief register a event callback function for interrupt mode I/O operation on
 * i2c_channel
 * @param[in] i2c_channel that the callback should be installed
 * @param[in] f callback function to be registered
 */
StatusInt I2C_RegisterCallback( uint8_t i2c_channel, I2CRxCallBack_t f)
{
    /* Save callback */
    i2c_channels[ i2c_channel ].rx_callback = f;
    return STATUSINT_SUCCESS;
}

/**
 * @brief This functions handles all I2C_Rx interrupt. Should be called in
 * IRQ monitoring thread when IRQ_TYPE_I2C_RX message is received.
 * It identify the current receiving channel and call the user registered
 * callback function along with the I2CInfoStruct and I2C channel number.
 */
void I2C_HandleReceives(void){
    uint8_t i;
    uint8_t msk = 0x1;
    uint8_t IRQ_flags = I2C_RX_IRQFlags;
    I2CInfoStruct_t rx_info;

    // loop through all channels
    for (i = 0; i < (uint8_t)I2C_CHANNEL_N; i++){
        // if the IRQ flag is set, copy the message from channel handle
        // to rx_info and call callback function.
        if ((IRQ_flags & msk) != 0){
            rx_info.length = i2c_channels[i].info.length;
            rx_info.address = i2c_channels[i].info.address;
            memcpy(rx_info.data, (const uint8_t*)i2c_channels[i].info.data,rx_info.length);
            I2C_RX_IRQ_FLAG_CLR((int)i);
            rx_info.state = I2C_STATE_RECEIVE;
            i2c_channels[i].rx_callback(i,&rx_info);
        }
        msk = msk << 1;
    }
}

/**
 * @brief This functions handles all I2C_Tx interrupt. Should be called in
 * IRQ monitoring thread when IRQ_TYPE_I2C_TX message is received.
 * It identify the current receiving channel and call the user registered
 * callback function along with the I2CInfoStruct and I2C channel number.
 * Note the only "state" in I2CInfoStruct is valid.
 */
void I2C_HandleTransmit(void){
    uint8_t i;
    uint8_t msk = 0x1;
    uint8_t IRQ_flags = I2C_RX_IRQFlags;
    I2CInfoStruct_t rx_info;

    for (i = 0; i < (uint8_t)I2C_CHANNEL_N; i++){
        if ((IRQ_flags & msk) != 0){
            rx_info.state = I2C_STATE_TRANSMIT;
            i2c_channels[i].rx_callback(i,&rx_info);
        }
        msk = msk << 1;
    }
}

/**
 * @brief This functions handles all I2C_ER interrupt. Should be called in
 * IRQ monitoring thread when IRQ_TYPE_I2C_ER message is received.
 * It identify the current receiving channel and call the user registered
 * callback function along with the I2CInfoStruct and I2C channel number.
 * Note the only "state" in I2CInfoStruct is valid.
 */
void I2C_HandleError(void){
    uint8_t i;
    uint8_t msk = 0x1;
    uint8_t IRQ_flags = I2C_ER_IRQFlags;
    I2CInfoStruct_t rx_info;

    for (i = 0; i < (uint8_t)I2C_CHANNEL_N; i++){
        if ((IRQ_flags & msk) != 0){
            rx_info.state = I2C_STATE_ERROR;
            i2c_channels[i].rx_callback(i,&rx_info);
        }
        msk = msk << 1;
    }
}

/* System IRQ functions on IRQ vector table */
void I2C1_EV_IRQHandler( void ){
    HAL_I2C_EV_IRQHandler(&i2c_channels[I2C_1].handle);
}

void I2C2_EV_IRQHandler( void ){
    HAL_I2C_EV_IRQHandler(&i2c_channels[I2C_2].handle);
}

void I2C3_EV_IRQHandler( void ){
    HAL_I2C_EV_IRQHandler(&i2c_channels[I2C_3].handle);
}

void I2C1_ER_IRQHandler( void ){
    HAL_I2C_ER_IRQHandler(&i2c_channels[I2C_1].handle);
}

void I2C2_ER_IRQHandler( void ){
    HAL_I2C_ER_IRQHandler(&i2c_channels[I2C_2].handle);
}
void I2C3_ER_IRQHandler( void ){
    HAL_I2C_ER_IRQHandler(&i2c_channels[I2C_3].handle);

}

/**
 * @brief This overwrite the default HAL_I2C_MasterRxCpltCallback.
 * This function will be called at the end of HAL_I2C_EV_IRQHandler
 * if data is received.
 * This function send a os message to indicate the occurance of
 * a IRQ_TYPE_I2C_RX interrupt
 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){
    if(hi2c->Instance == I2C1){
        I2C_RX_IRQ_FLAG_SET((int)I2C_1);
    }
    else if(hi2c->Instance == I2C2){
        I2C_RX_IRQ_FLAG_SET((int)I2C_2);
        
    }else{
        I2C_RX_IRQ_FLAG_SET((int)I2C_3);
    }
    osMessagePut(msgQ_id_IRQ, IRQ_TYPE_I2C_RX, 0);
   
}


/**
 * @brief This overwrite the default HAL_I2C_MasterTxCpltCallback.
 * This function will be called at the end of HAL_I2C_EV_IRQHandler
 * if data is transmitted.
 * This function send a os message to indicate the occurance of
 * a IRQ_TYPE_I2C_TX interrupt
 */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c){
    if(hi2c->Instance == I2C1){
        I2C_TX_IRQ_FLAG_SET((int)I2C_1);
    }
    else if(hi2c->Instance == I2C2){
        I2C_TX_IRQ_FLAG_SET((int)I2C_2);
        
    }else{
        I2C_TX_IRQ_FLAG_SET((int)I2C_3);
    }
    osMessagePut(msgQ_id_IRQ, IRQ_TYPE_I2C_TX, 0);
}


/**
 * @brief This overwrite the default HAL_I2C_ErrorCallback.
 * This function will be called at the end of HAL_I2C_ER_IRQHandler
 * if data is transmitted.
 * This function send a os message to indicate the occurance of
 * a IRQ_TYPE_I2C_ER interrupt.
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c){
        if(hi2c->Instance == I2C1){
        I2C_ER_IRQ_FLAG_SET((int)I2C_1);
    }
    else if(hi2c->Instance == I2C2){
        I2C_ER_IRQ_FLAG_SET((int)I2C_2);
        
    }else{
        I2C_ER_IRQ_FLAG_SET((int)I2C_3);
    }
    osMessagePut(msgQ_id_IRQ, IRQ_TYPE_I2C_ER, 0);
}
