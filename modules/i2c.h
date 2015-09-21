/*
 *   AFCIPMI
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * @file i2c.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date August 2015
 *
 * @brief Definitions used in I2C Driver
 */

#ifndef I2C_H_
#define I2C_H_

#include "FreeRTOS.h"
#include "task.h"

/*! @brief Max message length (in bits) used in I2C */
#define i2cMAX_MSG_LENGTH               32

/*! @brief Size of #IPMBL_TABLE
 */
#define IPMBL_TABLE_SIZE                27

/*! @brief GA pins definition */
typedef enum {
    GROUNDED = 0,
    POWERED,
    UNCONNECTED
}GA_Pin_state;

/*! @brief I2C Modes definition
 * @warning Slave Transmit (Slave write) mode is not implemented
 */
typedef enum {
    I2C_Mode_Local_Master = 0,              /*!< <b>Master Write</b> and <b>Master Read</b> modes only */
    I2C_Mode_IPMB                           /*!< <b>Master Write</b> and <b>Slave Read</b> modes only */
} I2C_Mode;

/*! @brief I2C driver error enumeration */
typedef enum {
    i2c_err_SUCCESS = 0,
    i2c_err_FAILURE,                        /*!< General Failure in I2C driver */
    i2c_err_MAX_LENGTH,                     /*!< Message trying to be sent is higher than I2C buffer limit */
    i2c_err_SLA_R_SENT_NACK,                /*!< SLA+R address transmitted, but no response has been received.
                                             *  @see #I2C_STAT_SLA_R_SENT_NACK  */
    i2c_err_SLA_DATA_RECV_NACK,             /*!< DATA byte has been received, NACK has been returned.
                                             *  This usually means that the master will only be reading one
                                             *  more byte from the slave.
                                             *  @see #I2C_STAT_DATA_RECV_NACK  */
    i2c_err_SLA_W_SENT_NACK,                /*!< SLA+R address transmitted, but no response has been received.
                                             *  Slave is either busy or unreachable.
                                             *  @see #I2C_STAT_SLA_W_SENT_NACK  */
    i2c_err_DATA_SENT_NACK                  /*!< DATA byte has been transmitted, but NACK has returned.
                                             *  Slave is either busy or unreachable.
                                             *  @see #I2C_STAT_DATA_SENT_NACK  */
} i2c_err;

/*! @brief I2C transaction parameter structure */
typedef struct xI2C_msg
{
    I2C_ID_T i2c_id;                        /*!< I2C interface number (0, 1 or 2) */
    uint8_t addr;                           /*!< Slave address of I2C device */
    uint8_t tx_data[i2cMAX_MSG_LENGTH];     /*!< Buffer cointaning bytes to transmit, limitted to #i2cMAX_MSG_LENGTH */
    uint8_t tx_len;                         /*!< Number of bytes to transmit */
    uint8_t rx_data[i2cMAX_MSG_LENGTH];     /*!< Buffer cointaning received bytes, limitted to #i2cMAX_MSG_LENGTH */
    uint8_t rx_len;                         /*!< Number of bytes to receive */
    i2c_err error;                          /*!< Error value from I2C driver
                                             *  @see #i2c_err
                                             */
} xI2C_msg;

extern uint8_t ipmb_addr;

/***********************/
/* Function Prototypes */
/***********************/

/*! @todo Update i2c function comments */

/*! @brief I2C Interface Initialization
 *
 * Initialize I2C corresponding pins with the following characteristics:
 *  - Open Drain
 *  - Function #3 (If I2C0, it's function #1)
 *  - No pull-up nor pull-down
 *
 * Configure and init the I2C interruption, with its priority set to one
 * level below the maximum FreeRTOS priority, so the interruption service
 * can access the API and manage the semaphore.
 * @param i2c_id: Interface ID ( I2C0, I2C1, I2C2 )
 * @param mode: Operating mode for the specified I2C interface
 */
void vI2CInit( I2C_ID_T i2c_id, uint32_t speed, I2C_Mode mode );

/*! @todo Document this function */
//void vI2CConfig( I2C_ID_T id, uint32_t speed );

/*! @brief Enter Master Write mode and transmit a buffer
 *
 * Bytes are transmitted in crescent order, incrementing the buffer index.
 *
 * @note This function blocks until its completion, in other words, it'll only
 * return when the whole buffer has been transmitted or an error occurs in I2C Driver.
 *
 * Example:
 * @code
 * uint8_t tx_buffer = { 0xAA, 0x55, 0x00, 0x01 };
 * uint8_t slave_address = 0x72;
 *
 * if( xI2CWrite ( I2C1, slave_address, tx_buffer, sizeof(tx_buffer)/sizeof(tx_buffer[0])) == i2c_err_SUCCESS ) {
 *
 *     // Send next message, for example
 *
 * } else {
 *
 *     // Retry sending this message or handle error
 *
 * }
 * @endcode
 * @param i2c_id: Interface ID ( I2C0, I2C1, I2C2 ).
 * @param addr: Destination of the message (7 bit address).
 * @param tx_data: Pointer to buffer of bytes to be transmitted.
 * @param tx_len: Length of tx_data buffer (must be lower than #i2cMAX_MSG_LENGTH, or an error will return)
 * @return I2C Driver error
 * @see #xI2CRead
 * @see #xI2CSlaveTransfer
 */
//i2c_err xI2CMasterWrite( I2C_ID_T id, uint8_t addr, uint8_t * tx_buff, uint8_t tx_len );

/*! @brief Enter Master Read mode and receive a buffer from slave
 *
 * @note This function blocks until its completion, in other words, it'll only
 * return when the whole buffer has been read or an error occurs in I2C Driver.
 *
 * @warning @p rx_data must be previously allocated to prevent invalid memory access by functions inside this routine.
 *
 * Example:
 * @code
 * uint8_t rx_data[i2cMAX_MSG_LENGTH];
 * uint8_t slave_address = 0x72;
 * uint8_t bytes_to_receive = 5;
 *
 * if( xI2CRead ( I2C1, slave_address, rx_data, bytes_to_receive)) == i2c_err_SUCCESS ) {
 *
 *     //Read message and blink a LED
 *     if ( rx_data[1] == 0x0A ) {
 *         blink_led(green);
 *     } else {
 *         blink_led(red);
 *     }
 *
 * } else {
 *
 *     // Retry sending this command or handle error
 *
 * }
 * @endcode
 * @param i2c_id: Interface ID ( I2C0, I2C1, I2C2 ).
 * @param addr: Destination of the message (7 bit address).
 * @param rx_data: Pointer to buffer in which the received bytes will be copied.
 * @param rx_len: Number of bytes that will be read from slave
 * @return I2C Driver error
 * @see #xI2CWrite
 * @see #xI2CSlaveTransfer
 */
//i2c_err xI2CMasterRead( I2C_ID_T id, uint8_t addr, uint8_t * rx_buff, uint8_t rx_len );

/*! @todo Document this function */
//i2c_err xI2CMasterWriteRead( I2C_ID_T id, uint8_t addr, uint8_t cmd, uint8_t * rx_buff, uint8_t rx_len );

/*! @brief Enter Slave Receiver mode and waits a data transmission
 *
 *     This function forces the I2C interface switch to Slave Listen (Receiver) mode
 * and waits another Master on the bus transmit any data.
 *     A timeout can be specified for this function, so your functions only block here
 * as long as they want.
 *
 * @warning @p rx_data must be previously allocated to prevent invalid memory access by functions inside this routine.
 *
 * Example:
 * @code
 * uint8_t rx_data[i2cMAX_MSG_LENGTH];
 * uint32_t timeout =  1000/portTICK_PERIOD_MS; // Specified in ticks ( you can convert to ms dividing the desired value by portTICK_PERIOD_MS macro, defined in portmacro.h file )
 * uint8_t data_len;
 *
 * data_len = xI2CSlaveTransfer( I2C0, rx_data, timeout );
 *
 * //Read message and blink a LED
 * if ( (data_len > 2) && (rx_data[1] == 0x0A) ) {
 *     blink_led(green);
 * } else {
 *     blink_led(red);
 * }
 * @endcode
 * @param i2c_id: Interface ID ( I2C0, I2C1, I2C2 ).
 * @param rx_data: Pointer to buffer in which the received bytes will be copied.
 * @param timeout: Amount of time to remain blocked until a message arrives (32-bit value)
 * @return Length of message received
 * @see #xI2CWrite
 * @see #xI2CRead
 */
//uint8_t xI2CSlaveReceive( I2C_ID_T id, uint8_t * rx_buff, uint8_t buff_len, TickType_t timeout );

/*! @todo Document this function */
//void vI2CSlaveSetup ( I2C_ID_T id, uint8_t slave_addr );

/*! @brief Reads own I2C slave address using GA pins
 *
 * Based on coreipm/coreipm/mmc.c
 * @author Gokhan Sozmen
 * Reads the GA pins, performing an unconnection checking, to define the device I2C slave address, as specified by MicroTCA documentation.
 *
 * @return 7-bit Slave Address
 *
 * @todo Develop a function to discover the Geographic Address once (checking the GA pins)
 * and store it into a global variable, since everytime a IPMI message is built
 * (request or response) the MMC has to check its own  address to fill the rs/rqSA field,
 * and it takes some time to go through all this function.
 */
uint8_t get_ipmb_addr( void );

#endif /*I2C_H_*/
