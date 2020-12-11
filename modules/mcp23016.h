/*
 *   openMMC -- Open Source modular IPM Controller firmware
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
 *
 *   @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

/**
 * @file   mcp23016.h
 *
 * @brief  MCP23016 module interface functions declarations
 *
 * @ingroup MCP23016
 */

#ifndef MCP23016_H_
#define MCP23016_H_

#include "port.h"

/**
 * @defgroup MCP23016 MCP23016 16-bit I2C/SMBus I/O Expander Module
 * @ingroup PERIPH_IC
 */

/**
 * @defgroup MCP23016_REGS MCP23016 Registers
 * @ingroup MCP23016
 * @{
 */

/**
 * @brief MCP23016 General purpose I/O port register (Read/write)
 * @note Default register value = 0x00
 */
#define MCP23016_GP_REG          0x0

/**
 * @brief MCP23016 Output latch register (Read/write)
 * @note Default register value = 0x00
 */
#define MCP23016_OLAT_REG        0x2

/**
 * @brief MCP23016 Input polarity port register (Read/write)
 * @note Default register value = 0x00
 */
#define MCP23016_IPOL_REG        0x4

/**
 * @brief MCP23016 I/O direction register (Read/write)
 * @note Default register value = 0xFF
 */
#define MCP23016_IODIR_REG       0x6

/**
 * @brief MCP23016 Interrupt captured value register (Read only)
 * @note Default register value not defined
 */
#define MCP23016_INTCAP_REG      0x8

/**
 * @brief MCP23016 I/0 control register (Read/write)
 * @note Default register value not defined
 */
#define MCP23016_IOCON_REG       0xA

/**
 * @}
 */


/* Pins Read/Write */
/**
 * @brief Read port pins status
 *
 * @return 8-bit value with the status of port pins
 */
uint8_t mcp23016_read_port( uint8_t port_num, uint8_t *readout );

/**
 * @brief Read a pin status
 *
 * @param pin Selected pin to read
 *
 * @return Pin logical status
 */
uint8_t mcp23016_read_pin( uint8_t port_num, uint8_t pin, uint8_t *status );

/**
 * @brief Write port output pins
 *
 * @param data 8-bit value with the pins status
 *
 * @return None
 */
uint8_t mcp23016_write_port( uint8_t port_num, uint8_t data );

/**
 * @brief Write a output pin
 *
 * @param pin Selected pin to write
 * @param data Logical status to be set
 *
 * @return None
 */
uint8_t mcp23016_write_pin( uint8_t port_num, uint8_t pin, bool data );

/* Polarity Control */
/**
 * @brief Set port pins polarity
 *
 * @param pol Polarity (active-high/low)
 */
uint8_t mcp23016_set_port_pol( uint8_t port_num, uint8_t pol );

/**
 * @brief Read port pins polarity
 *
 * @return Pins polarity (active-high/low)
 */
uint8_t mcp23016_get_port_pol( uint8_t port_num, uint8_t *pol );

/* Pins direction (output/input) */
/**
 * @brief Set port pins direction
 *
 * @param dir Pin direction ( 0-Output 1-Input )
 *
 * @return None
 */
uint8_t mcp23016_set_port_dir( uint8_t port_num, uint8_t dir );

/**
 * @brief Read port pins direction
 *
 * @return 8-bit value with the pins direction setup
 */
uint8_t mcp23016_get_port_dir( uint8_t port_num, uint8_t *dir );

#endif
