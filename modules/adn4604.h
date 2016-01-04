/*
 *   adn4604.h
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015
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

#ifndef ADN4604_H_
#define ADN4604_H_

typedef struct __attribute__((__packed__)) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint8_t out1:4,
	out0:4;
    uint8_t out3:4,
	out2:4;
    uint8_t out5:4,
	out4:4;
    uint8_t out7:4,
	out6:4;
    uint8_t out9:4,
	out8:4;
    uint8_t out11:4,
	out10:4;
    uint8_t out13:4,
	out12:4;
    uint8_t out15:4,
	out14:4;
#else
    uint8_t out0:4,
	out1:4;
    uint8_t out2:4,
	out3:4;
    uint8_t out4:4,
	out5:4;
    uint8_t out6:4,
	out7:4;
    uint8_t out8:4,
	out9:4;
    uint8_t out10:4,
	out11:4;
    uint8_t out12:4,
	out13:4;
    uint8_t out14:4,
	out15:4;
#endif
} t_adn_connect_map;

typedef struct {
    uint8_t map_reg;
    t_adn_connect_map map_connect;
} t_adn_connect_cfg;

#define ADN_XPT_MAP0_CON_REG		0x90
#define ADN_XPT_MAP1_CON_REG		0x98

#define ADN_XPT_UPDATE_REG 		0x80
#define ADN_XPT_MAP_TABLE_SEL_REG 	0x81

#define ADN_XPT_MAP0			0x00
#define ADN_XPT_MAP1			0x01

#define ADN_XPT_BROADCAST		0x82

#define ADN_TX_CON_OUT0                 0x20

void adn4604_setup( void );
void adn4604_tx_enable( uint8_t i2c_bus_id, uint8_t slave_addr, uint8_t output );
void adn4604_update( uint8_t i2c_bus_id, uint8_t slave_addr );

#endif
