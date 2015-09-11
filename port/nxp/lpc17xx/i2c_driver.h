#ifdef PORT_I2C_DRIVER_H_
#undef PORT_I2C_DRIVER_H_
#endif
#define PORT_I2C_DRIVER_H_

/*! @name I2C Control Register (I2CCON) bit values
 */
#define I2C_AA                           (1 << 2)        /*!< @brief <b> Assert Acknowledge Flag </b>
                                                          *
                                                          *  Set this flag to Acknowledge the next incoming byte.
                                                          */
#define I2C_SI                           (1 << 3)        /*!< @brief <b> I2C interrupt Flag </b>
                                                          *
                                                          *  This bit is set when the I2C state changes.
                                                          *  It must be cleared after handling the interruption.
                                                          */
#define I2C_STO                          (1 << 4)        /*!< @brief <b> STOP Flag </b>
                                                          *
                                                          *  Setting this bit causes the I2C interface to transmit a STOP condition in master mode,
                                                          *  or recover from an error condition in slave mode.
                                                          *  When the bus detects the STOP condition, STO flag is cleared automatically.
                                                          *  @warning STO bit may not be set in I2CCONCLR (for LPC17xx chips)
                                                          */
#define I2C_STA                          (1 << 5)        /*!< @brief <b> START Flag </b>
                                                          *
                                                          *  Setting this bit causes the I2C interface to enter master mode and transmit a START condition
                                                          *  or transmit a repeated START condition if it is already in master mode.
                                                          *  @note If STA and STO are both set, then a STOP condition is transmitted on the I2C bus if the
                                                          *  interface is in master mode, and transmits a START condition thereafter. If the I2C interface
                                                          *  is in slave mode, an internal STOP condition is generated, but is not transmitted on the bus.
                                                          */
#define I2C_I2EN                         (1 << 6)        /*!< @brief <b> I2C Interface Enable Flag </b>
                                                          *
                                                          *  When I2EN is 1, the I2C interface is enabled.
                                                          *  When I2EN is “0”, the SDA and SCL input signals are ignored, the I2C block is in the “not
                                                          *  addressed” slave state, and the STO bit is forced to “0”.
                                                          *  @note I2EN should not be used to temporarily release the I2C bus since, when I2EN is reset,
                                                          *  the I2C-bus status is lost. The #I2C_AA flag should be used instead.
                                                          */

/* I2C interface functions for LPC 17xx */
i2c_err port_I2C_Master_Write( I2C_ID_T id, uint8_t addr, uint8_t * tx_buff, uint8_t tx_len );
i2c_err port_I2C_Master_Read( I2C_ID_T id, uint8_t addr, uint8_t * rx_buff, uint8_t rx_len );
i2c_err port_I2C_Master_Write_Read( I2C_ID_T id, uint8_t addr, uint8_t cmd, uint8_t * rx_buff, uint8_t rx_len );
uint8_t port_I2C_Slave_Receive( I2C_ID_T id, uint8_t * rx_buff, TickType_t timeout );
void port_I2C_Slave_Setup ( I2C_ID_T id, uint8_t slave_addr, uint8_t * rx_buff, uint8_t buff_len );
