/*
 * i2c.h
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva <henrique.silva@lnls.br>
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
/* TODO: insert reference to CoreIPM code (geographic address find) */

#ifndef I2C_H_
#define I2C_H_

#define i2cMAX_MSG_LENGTH           32

/* I2C Control Register (I2CCON) bit values */
#define I2C_AA      (1 << 2)
#define I2C_SI      (1 << 3)
#define I2C_STO     (1 << 4)        /* Note: STO may not be set in I2CCONCLR */
#define I2C_STA     (1 << 5)
#define I2C_I2EN    (1 << 6)

/* I2C status (I2STAT register) possible values */
/* Common states */
#define I2C_STAT_BUS_ERROR                  0x00
#define I2C_STAT_START                      0x08
#define I2C_STAT_REPEATED_START             0x10
#define I2C_STAT_ARB_LOST                   0x38
/* Master Transmit Mode */
#define I2C_STAT_SLA_W_SENT_ACK             0x18
#define I2C_STAT_SLA_W_SENT_NACK            0x20
#define I2C_STAT_DATA_SENT_ACK              0x28
#define I2C_STAT_DATA_SENT_NACK             0x30
/* Master Receiver Mode */
#define I2C_STAT_SLA_R_SENT_ACK             0x40
#define I2C_STAT_SLA_R_SENT_NACK            0x48
#define I2C_STAT_DATA_RECV_ACK              0x50
#define I2C_STAT_DATA_RECV_NACK             0x58
/* Slave Receiver Mode */
#define I2C_STAT_SLA_W_RECV_ACK             0x60
#define I2C_STAT_ARB_LOST_SLA_W_RECV_ACK    0x68
#define I2C_STAT_GEN_CALL_ACK               0x70
#define I2C_STAT_ARB_LOST_GEN_CALL_ACK      0x68
#define I2C_STAT_SLA_DATA_RECV_ACK          0x80
#define I2C_STAT_SLA_DATA_RECV_NACK         0x88
#define I2C_STAT_GEN_CALL_DATA_RECV_ACK     0x90
#define I2C_STAT_GEN_CALL_DATA_RECV_NACK    0x98
#define I2C_STAT_SLA_STOP_REP_START         0xA0
/* Slave Transmitter Mode */
#define I2C_STAT_SLA_R_RECV_ACK             0xA8
#define I2C_STAT_ARB_LOST_SLA_R_RECV_ACK    0xB0
#define I2C_STAT_SLA_DATA_SENT_ACK          0xB8
#define I2C_STAT_SLA_DATA_SENT_NACK         0xC0
#define I2C_STAT_SLA_LAST_DATA_SENT_ACK     0xC8

/* GA pins definition */
typedef enum {
    GROUNDED = 0,
    POWERED,
    UNCONNECTED
}GA_Pin_state;

/* I2C Modes definition
 *  -> I2C_Mode_Local_Master is able to perform both Master read and write
 *  -> I2C_Mode_IPMB is able to perform Master Write and Slave Read only
 *  Slave write is not implemented
 */
typedef enum {
    I2C_Mode_Local_Master = 0,
    I2C_Mode_IPMB
} I2C_Mode;

typedef enum {
    i2c_err_SUCCESS = 0,
    i2c_err_FAILURE,
    i2c_err_MAX_LENGTH,
    i2c_err_SLA_R_SENT_NACK,
    i2c_err_SLA_DATA_RECV_NACK,
    i2c_err_SLA_W_SENT_NACK,
    i2c_err_DATA_SENT_NACK
} i2c_err;

/* I2C transaction parameter structure
 */
typedef struct xI2C_msg
{
    I2C_ID_T i2c_id;                        /* I2C interface number (0, 1 or 2) */
    uint8_t addr;                           /* Slave address of I2C device */
    uint8_t tx_data[i2cMAX_MSG_LENGTH];     /* Pointer to bytes to transmit */
    uint8_t tx_len;                         /* Number of bytes to transmit */
    uint8_t rx_data[i2cMAX_MSG_LENGTH];     /* Pointer to received bytes */
    uint8_t rx_len;                         /* Number of bytes to receive */
    i2c_err error;
} xI2C_msg;

typedef struct xI2C_pins {
    uint8_t sda_port;
    uint8_t sda_pin;
    uint8_t scl_port;
    uint8_t scl_pin;
    uint8_t pin_func;
} xI2C_pins_t;

/* I2C common interface structure */
typedef struct xI2C_Config {
    LPC_I2C_T *reg;      /* IP base address of the I2C device */
    xI2C_pins_t pins;
    IRQn_Type irq;
    I2C_Mode mode;
    TaskHandle_t caller_task;
    uint8_t rx_cnt;
    uint8_t tx_cnt;
    xI2C_msg msg;
} xI2C_Config;

extern struct xI2C_Config i2c_cfg[];

/* Macro to obtain the I2C base address by its name */
#define LPC_I2Cx(x)      ((i2c_cfg[x].reg))

/* Function Prototypes */
void vI2CTask( void * pvParameters );
void vI2CInit( I2C_ID_T i2c_id, I2C_Mode mode );
i2c_err xI2CWrite( I2C_ID_T i2c_id, uint8_t addr, uint8_t * tx_data, uint8_t tx_len );
i2c_err xI2CRead( I2C_ID_T i2c_id, uint8_t addr, uint8_t * rx_data, uint8_t rx_len );
uint8_t xI2CSlaveTransfer ( I2C_ID_T i2c_id, uint8_t * rx_data, uint32_t timeout );
uint8_t ulCFG_MMC_GA( void );

#endif /*I2C_H_*/
