/*
 *   openMMC  --
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
 * @file lpc17_i2c.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date March 2016
 *
 * @brief I2C driver for LPC17xx
 */

/*! @brief Max message length (in bits) used in I2C */
#define i2cMAX_MSG_LENGTH               32

#define xI2CMasterWrite(id, addr, tx_buff, tx_len) Chip_I2C_MasterSend(id, addr, tx_buff, tx_len)
#define xI2CMasterRead(id, addr, rx_buff, rx_len) Chip_I2C_MasterRead(id, addr, rx_buff, rx_len)

uint8_t xI2CSlaveReceive( I2C_ID_T id, uint8_t * rx_buff, uint8_t buff_len, uint32_t timeout );
void vI2CSlaveSetup ( I2C_ID_T id, uint8_t slave_addr );
void vI2CConfig( I2C_ID_T id, uint32_t speed );
int xI2CMasterWriteRead(I2C_ID_T id, uint8_t addr, const uint8_t *tx_buff, int tx_len, uint8_t *rx_buff, int rx_len);
