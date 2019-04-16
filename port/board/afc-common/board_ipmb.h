/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
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
 * @file board_ipmb.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Board specific definitions used in IPMB Layer
 */

#ifndef BOARD_IPMB_H_
#define BOARD_IPMB_H_

/**
 * @brief Default I2C interface to use in IPMB protocol
 */
#define IPMB_I2C                I2C0

/**
 * @brief I2C Clock Frequency for IPMB - 100kHz
 */
#define IPMB_I2C_FREQ 100000

/**
 * @brief Minimum clock cycles to the GPIO pin change take effect on some boards
 */
#define GPIO_GA_DELAY 10

/**
 * @brief Reads own I2C slave address using GA pins
 *
 * Based on coreipm/coreipm/mmc.c
 * @author Gokhan Sozmen
 *
 *  The state of each GA signal is represented by G (grounded), U (unconnected),
 *  or P (pulled up to Management Power).
 *
 *  The MMC drives P1 low and reads the GA lines. The MMC then drives P1 high and
 *  reads the GA lines. Any line that changes state between the two reads indicate
 *  an unconnected (U) pin.
 *
 *  The IPMB-L address of a Module can be calculated as (70h + Site Number x 2). <br>
 *  G = 0, P = 1, U = 2 <br>
 *  | Pin | Ternary | Decimal | Address |
 *  |:---:|:-------:|:-------:|:-------:|
 *  | GGG | 000 | 0  | 0x70 |
 *  | GGP | 001 | 1  | 0x8A |
 *  | GGU | 002 | 2  | 0x72 |
 *  | GPG | 010 | 3  | 0x8E |
 *  | GPP | 011 | 4  | 0x92 |
 *  | GPU | 012 | 5  | 0x90 |
 *  | GUG | 020 | 6  | 0x74 |
 *  | GUP | 021 | 7  | 0x8C |
 *  | GUU | 022 | 8  | 0x76 |
 *  | PGG | 100 | 9  | 0x98 |
 *  | PGP | 101 | 10 | 0x9C |
 *  | PGU | 102 | 11 | 0x9A |
 *  | PPG | 110 | 12 | 0xA0 |
 *  | PPP | 111 | 13 | 0xA4 |
 *  | PPU | 112 | 14 | 0x88 |
 *  | PUG | 120 | 15 | 0x9E |
 *  | PUP | 121 | 16 | 0x86 |
 *  | PUU | 122 | 17 | 0x84 |
 *  | UGG | 200 | 18 | 0x78 |
 *  | UGP | 201 | 19 | 0x94 |
 *  | UGU | 202 | 20 | 0x7A |
 *  | UPG | 210 | 21 | 0x96 |
 *  | UPP | 211 | 22 | 0x82 |
 *  | UPU | 212 | 23 | 0x80 |
 *  | UUG | 220 | 24 | 0x7C |
 *  | UUP | 221 | 25 | 0x7E |
 *  | UUU | 222 | 26 | 0xA2 |
 *
 * @return 7-bit Slave Address
 */
uint8_t get_ipmb_addr( void );

#endif /* BOARD_IPMB_H_ */
