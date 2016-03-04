#ifdef USE_GENERIC_I2C_DRIVER
#ifndef I2C_DRIVER_H_
#define I2C_DRIVER_H_

/*!
 * @file i2c_driver.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date September 2015
 *
 * @brief Definitions for generic implementation of a I2C Driver
 */

/*! @name I2STAT common values for all states
 * @{
 */
#define I2C_STAT_BUS_ERROR               0x00        /*!< @brief Bus error due to an illegal START or STOP condition.
                                                      *
                                                      *  This state can also occur when interference causes the I2C block to enter
                                                      *  an undefined state.
                                                      *  To recover from a bus error, the STO flag must be set and SI must be cleared.
                                                      *  This causes the I2C block to enter the “not addressed” slave mode and to clear
                                                      *  the STO flag. The SDA and SCL lines are then released ( STOP condition isn't transmitted).
                                                      */
#define I2C_STAT_START                   0x08        /*!< @brief START condition has been transmitted
                                                      */
#define I2C_STAT_REPEATED_START          0x10        /*!< @brief Repeated START condition has been transmitted
                                                      */
#define I2C_STAT_ARB_LOST                0x38        /*!< @brief Arbitration lost in SLA+R/W or Data bytes.
                                                      */
/*@}*/

/*! @name I2STAT values for Master Transmit Mode
 * @{
 */

#define I2C_STAT_SLA_W_SENT_ACK          0x18        /*!< @brief SLA+W has been transmitted and ACK has been received.
                                                      */
#define I2C_STAT_SLA_W_SENT_NACK         0x20        /*!< @brief SLA+W has been transmitted and NACK has been received.
                                                      */
#define I2C_STAT_DATA_SENT_ACK           0x28        /*!< @brief DATA byte has been transmitted and ACK has been received.
                                                      */
#define I2C_STAT_DATA_SENT_NACK          0x30        /*!< @brief DATA byte has been transmitted and NACK has been received.
                                                      */
/*@}*/

/*! @name I2STAT values for Master Receiver Mode
 * @{
 */
#define I2C_STAT_SLA_R_SENT_ACK          0x40        /*!< @brief SLA+R has been transmitted and NACK has been received.
                                                      */
#define I2C_STAT_SLA_R_SENT_NACK         0x48        /*!< @brief SLA+R has been transmitted and NACK has been received.
                                                      */
#define I2C_STAT_DATA_RECV_ACK           0x50        /*!< @brief DATA byte has been received and NACK has been returned.
                                                      */
#define I2C_STAT_DATA_RECV_NACK          0x58        /*!< @brief DATA byte has been received and NACK has been returned.
                                                      */
/*@}*/

/*! @name I2STAT values for Slave Receiver Mode
 * @{
 */

#define I2C_STAT_SLA_W_RECV_ACK          0x60        /*!< @brief Own SLA+W has been received ACK has been returned.
                                                      */
#define I2C_STAT_ARB_LOST_SLA_W_RECV_ACK 0x68        /*!< @brief Arbitration lost in SLA+R/W as master.
                                                      *
                                                      *    Own SLA+W has been received, ACK returned.
                                                      */
#define I2C_STAT_GEN_CALL_ACK            0x70        /*!< @brief General Call address has been received. ACK has been returned.
                                                      */
#define I2C_STAT_ARB_LOST_GEN_CALL_ACK   0x78        /*!< @brief Arbitration lost in SLA+R/W as master.
                                                      *
                                                      *    General Call address has been received, ACK has been returned.
                                                      */
#define I2C_STAT_SLA_DATA_RECV_ACK       0x80        /*!< @brief DATA has been received and ACK has been returned. (Slave Addressed mode)
                                                      */
#define I2C_STAT_SLA_DATA_RECV_NACK      0x88        /*!< @brief DATA has been received and NACK has been returned. (Slave Addressed mode)
                                                      */
#define I2C_STAT_GEN_CALL_DATA_RECV_ACK  0x90        /*!< @brief DATA has been received and ACK has been returned. (General Call mode)
                                                      */
#define I2C_STAT_GEN_CALL_DATA_RECV_NACK 0x98        /*!< @brief DATA has been received and NACK has been returned. (General Call mode)
                                                      */
#define I2C_STAT_SLA_STOP_REP_START      0xA0        /*!< @brief A STOP condition or repeated START condition received in Slave mode.
                                                      */

#define I2C_STAT_BUSERR                  0x00        /*!< @brief General bus error. */

/*@}*/

/*! @name I2STAT values for Slave Transmitter Mode
 * @warning Slave Transmitter mode is not implemented in this I2C driver
 * @{
 */
#define I2C_STAT_SLA_R_RECV_ACK          0xA8
#define I2C_STAT_ARB_LOST_SLA_R_RECV_ACK 0xB0
#define I2C_STAT_SLA_DATA_SENT_ACK       0xB8
#define I2C_STAT_SLA_DATA_SENT_NACK      0xC0
#define I2C_STAT_SLA_LAST_DATA_SENT_ACK  0xC8
/*@}*/

/*! Macro to obtain the I2C base address by its number */
#define LPC_I2Cx(x)      ((i2c_cfg[x].reg))

/*! @brief Pin definition struct for I2C interface
 *
 * (Port number, Pin number, Pin Function)
 * @note It's assumed here that both SDA and SCL pins are on the same port in the microcontroller */
typedef struct xI2C_pins {
    uint8_t sda_port;
    uint8_t sda_pin;
    uint8_t scl_port;
    uint8_t scl_pin;
    uint8_t pin_func;
} xI2C_pins_t;

/*! @brief I2C common interface structure */
typedef struct xI2C_Config {
    void *reg;                     /*!< I2C Control Register Address */
    xI2C_pins_t pins;              /*!< Pin configuration struct */
    IRQn_Type irq;                 /*!< Interruption table index */
    I2C_Mode mode;                 /*!< Mode of operation*/
    xI2C_msg msg;                  /*!< Message body (tx and rx buffers) */
} xI2C_Config;

/*! Global I2C Configuration struct array (1 item for each interface) */
extern struct xI2C_Config i2c_cfg[];

#endif
#endif
