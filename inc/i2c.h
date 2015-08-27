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

/*! @brief Max message length (in bits) used in I2C */
#define i2cMAX_MSG_LENGTH           32

/*! @name I2C Control Register (I2CCON) bit values
 */
#define I2C_AA      (1 << 2)        /*!< @brief <b> Assert Acknowledge Flag </b>
                                     *
                                     *  Set this flag to Acknowledge the next incoming byte.
                                     */
#define I2C_SI      (1 << 3)        /*!< @brief <b> I2C interrupt Flag </b>
                                     *
                                     *  This bit is set when the I2C state changes.
                                     *  It must be cleared after handling the interruption.
                                     */
#define I2C_STO     (1 << 4)        /*!< @brief <b> STOP Flag </b>
                                     *
                                     *  Setting this bit causes the I2C interface to transmit a STOP condition in master mode,
                                     *  or recover from an error condition in slave mode.
                                     *  When the bus detects the STOP condition, STO flag is cleared automatically.
                                     *  @warning STO bit may not be set in I2CCONCLR (for LPC17xx chips)
                                     */
#define I2C_STA     (1 << 5)        /*!< @brief <b> START Flag </b>
                                     *
                                     *  Setting this bit causes the I2C interface to enter master mode and transmit a START condition
                                     *  or transmit a repeated START condition if it is already in master mode.
                                     *  @note If STA and STO are both set, then a STOP condition is transmitted on the I2C bus if the
                                     *  interface is in master mode, and transmits a START condition thereafter. If the I2C interface
                                     *  is in slave mode, an internal STOP condition is generated, but is not transmitted on the bus.
                                     */
#define I2C_I2EN    (1 << 6)        /*!< @brief <b> I2C Interface Enable Flag </b>
                                     *
                                     *  When I2EN is 1, the I2C interface is enabled.
                                     *  When I2EN is “0”, the SDA and SCL input signals are ignored, the I2C block is in the “not
                                     *  addressed” slave state, and the STO bit is forced to “0”.
                                     *  @note I2EN should not be used to temporarily release the I2C bus since, when I2EN is reset,
                                     *  the I2C-bus status is lost. The #I2C_AA flag should be used instead.
                                     */

/*! @name I2STAT common values for all states
 * @{
 */
#define I2C_STAT_BUS_ERROR                  0x00    /*!< @brief Bus error due to an illegal START or STOP condition.
                                                     *
                                                     *  This state can also occur when interference causes the I2C block to enter
                                                     *  an undefined state.
                                                     *  To recover from a bus error, the STO flag must be set and SI must be cleared.
                                                     *  This causes the I2C block to enter the “not addressed” slave mode and to clear
                                                     *  the STO flag. The SDA and SCL lines are then released ( STOP condition isn't transmitted).
                                                     */
#define I2C_STAT_START                      0x08    /*!< @brief START condition has been transmitted
                                                     */
#define I2C_STAT_REPEATED_START             0x10    /*!< @brief Repeated START condition has been transmitted
                                                     */
#define I2C_STAT_ARB_LOST                   0x38    /*!< @brief Arbitration lost in SLA+R/W or Data bytes.
                                                     */
/*@}*/

/*! @name I2STAT values for Master Transmit Mode
 * @{
 */

#define I2C_STAT_SLA_W_SENT_ACK             0x18    /*!< @brief SLA+W has been transmitted and ACK has been received.
                                                     */
#define I2C_STAT_SLA_W_SENT_NACK            0x20    /*!< @brief SLA+W has been transmitted and NACK has been received.
                                                     */
#define I2C_STAT_DATA_SENT_ACK              0x28    /*!< @brief DATA byte has been transmitted and ACK has been received.
                                                     */
#define I2C_STAT_DATA_SENT_NACK             0x30    /*!< @brief DATA byte has been transmitted and NACK has been received.
                                                     */
/*@}*/

/*! @name I2STAT values for Master Receiver Mode
 * @{
 */
#define I2C_STAT_SLA_R_SENT_ACK             0x40    /*!< @brief SLA+R has been transmitted and NACK has been received.
                                                     */
#define I2C_STAT_SLA_R_SENT_NACK            0x48    /*!< @brief SLA+R has been transmitted and NACK has been received.
                                                     */
#define I2C_STAT_DATA_RECV_ACK              0x50    /*!< @brief DATA byte has been received and NACK has been returned.
                                                     */
#define I2C_STAT_DATA_RECV_NACK             0x58    /*!< @brief DATA byte has been received and NACK has been returned.
                                                     */
/*@}*/

/*! @name I2STAT values for Slave Receiver Mode
 * @{
 */

#define I2C_STAT_SLA_W_RECV_ACK             0x60    /*!< @brief Own SLA+W has been received ACK has been returned.
                                                     */
#define I2C_STAT_ARB_LOST_SLA_W_RECV_ACK    0x68    /*!< @brief Arbitration lost in SLA+R/W as master.
                                                     *
                                                     *    Own SLA+W has been received, ACK returned.
                                                     */
#define I2C_STAT_GEN_CALL_ACK               0x70    /*!< @brief General Call address has been received. ACK has been returned.
                                                     */
#define I2C_STAT_ARB_LOST_GEN_CALL_ACK      0x78    /*!< @brief Arbitration lost in SLA+R/W as master.
                                                     *
                                                     *    General Call address has been received, ACK has been returned.
                                                     */
#define I2C_STAT_SLA_DATA_RECV_ACK          0x80    /*!< @brief DATA has been received and ACK has been returned. (Slave Addressed mode)
                                                     */
#define I2C_STAT_SLA_DATA_RECV_NACK         0x88    /*!< @brief DATA has been received and NACK has been returned. (Slave Addressed mode)
                                                     */
#define I2C_STAT_GEN_CALL_DATA_RECV_ACK     0x90    /*!< @brief DATA has been received and ACK has been returned. (General Call mode)
                                                     */
#define I2C_STAT_GEN_CALL_DATA_RECV_NACK    0x98    /*!< @brief DATA has been received and NACK has been returned. (General Call mode)
                                                     */
#define I2C_STAT_SLA_STOP_REP_START         0xA0    /*!< @brief A STOP condition or repeated START condition received in Slave mode.
                                                     */
/*@}*/

/*! @name I2STAT values for Slave Transmitter Mode
 * @warning Slave Transmitter mode is not implemented in this I2C driver
 * @{
 */
#define I2C_STAT_SLA_R_RECV_ACK             0xA8
#define I2C_STAT_ARB_LOST_SLA_R_RECV_ACK    0xB0
#define I2C_STAT_SLA_DATA_SENT_ACK          0xB8
#define I2C_STAT_SLA_DATA_SENT_NACK         0xC0
#define I2C_STAT_SLA_LAST_DATA_SENT_ACK     0xC8
/*@}*/

/*! @brief Size of #IPMBL_TABLE 
 */
#define IPMBL_TABLE_SIZE 27

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
                                             *  This usually means that we, while in master receiver mode,
                                             *  will only be reading one more byte from the slave.
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

/*! @brief Pin definition struct for I2C interface
 *
 * (Port number, Pin number, Pin Function)
 * @note We assume here that both SDA and SCL pins are on the same port in the microcontroller */
typedef struct xI2C_pins {
    uint8_t sda_port;
    uint8_t sda_pin;
    uint8_t scl_port;
    uint8_t scl_pin;
    uint8_t pin_func;
} xI2C_pins_t;

/*! @brief I2C common interface structure */
typedef struct xI2C_Config {
    LPC_I2C_T *reg;                /*!< Control Register Address */
    xI2C_pins_t pins;              /*!< Pin configuration struct */
    IRQn_Type irq;                 /*!< Interruption table index */
    I2C_Mode mode;                 /*!< Mode of operation*/
    TaskHandle_t master_task_id;   /*!< Handler of caller task in
                                    * I2C master mode */
    TaskHandle_t slave_task_id;    /*!< Handler of caller task in
                                    * slave mode (will be notified
                                    * when a full message is received
                                    * (bytes from START to STOP) or
                                    * an error happens in the I2C
                                    * interruption service )*/
    uint8_t rx_cnt;                /*!< Received bytes counter */
    uint8_t tx_cnt;                /*!< Transmitted bytes counter */
    xI2C_msg msg;                  /*!< Message body (tx and rx buffers) */
} xI2C_Config;

/*! Global I2C Configuration struct array (1 item for each interface) */
extern struct xI2C_Config i2c_cfg[];

/*! Macro to obtain the I2C base address by its number */
#define LPC_I2Cx(x)      ((i2c_cfg[x].reg))

/***********************/
/* Function Prototypes */
/***********************/

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
void vI2CInit( I2C_ID_T i2c_id, I2C_Mode mode );

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
i2c_err xI2CWrite( I2C_ID_T i2c_id, uint8_t addr, uint8_t * tx_data, uint8_t tx_len );

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
i2c_err xI2CRead( I2C_ID_T i2c_id, uint8_t addr, uint8_t * rx_data, uint8_t rx_len );

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
uint8_t xI2CSlaveTransfer ( I2C_ID_T i2c_id, uint8_t * rx_data, uint32_t timeout );

/*! @brief Reads own I2C slave address using GA pins
 *
 * Reads the GA pins, performing an unconnection checking, to define the device I2C slave address, as specified by MicroTCA documentation. 
 *
 * @return 7-bit Slave Address
 *
 * @todo Develop a function to discover the Geographic Address once (checking the GA pins) 
 * and store it into a global variable, since everytime we build a IPMI message 
 * (request or response) we need to check our address to fill the rs/rqSA field, 
 * and it takes some time to go through all this function.
 */
uint8_t get_ipmb_addr( void );

#endif /*I2C_H_*/
