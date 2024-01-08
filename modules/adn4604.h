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
 * @file   adn4604.h
 * @author Henrique Silva <henrique.silva@lnls.br>
 *
 * @brief  ADN4604 module interface declarations
 *
 * @ingroup ADN4604
 */

/**
 * @defgroup ADN4604 ADN4604 16x16 Digital Crosspoint Switch
 * @ingroup PERIPH_IC
 */

#ifndef ADN4604_H_
#define ADN4604_H_

#include "mmc_error.h"

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
} adn_connect_map_t;

typedef struct adn_connect_cfg{
    uint8_t map_reg;
    adn_connect_map_t map_connect;
} adn_connect_cfg_t;

#define ADN_XPT_MAP0_CON_REG            0x90
#define ADN_XPT_MAP1_CON_REG            0x98

#define ADN_RESET_REG                   0x00

#define ADN_XPT_UPDATE_REG              0x80
#define ADN_XPT_MAP_TABLE_SEL_REG       0x81

#define ADN_XPT_MAP0                    0x00
#define ADN_XPT_MAP1                    0x01

#define ADN_XPT_BROADCAST               0x82

#define ADN_XPT_STATUS_REG              0xB0

#define ADN_TERMINATION_CTL_REG         0xF0

#define ADN_TX_CON_OUT0                 0x20

enum adn4604_term_ctl {
    RXW_TERM = 0,  /*< Input[7:0]   (West)  termination control */
    RXE_TERM,      /*< Input[15:8]  (East)  termination control */
    TXS_TERM,      /*< Output[7:0]  (South) termination control */
    TXN_TERM       /*< Output[15:8] (North) termination control */
};

enum adn4604_tx_ctl {
    TX_DISABLED,
    TX_STANDBY,
    TX_SQUELCHED,
    TX_ENABLED
};

/**
 * @brief Sets the output status
 *
 * @param output Output number (0 to 15)
 * @param tx_mode Selected mode: (TX_DISABLED, TX_STANDBY, TX_SQUELCHED or TX_ENABLED)
 */
mmc_err adn4604_tx_control( uint8_t output, uint8_t tx_mode );

/**
 * @brief Activates the current stored configuration
 *
 * @note The Update pin has precedence over the software register, so if the Update pin is asserted, but the low-to-high step doesn't occur, you won't be able to update the IC configuration
 *
 */
mmc_err adn4604_update( void );

/**
 * @brief ADN4604 Software Reset
 */
mmc_err adn4604_reset( void );

/**
 * @brief Configures the cross-connection map
 *
 * @param map Selected map to configure (0 or 1)
 * @param xpt_con Outputs assignment
 */
mmc_err adn4604_xpt_config( uint8_t map, adn_connect_map_t xpt_con );

/**
 * @brief Sets the active map on the IC
 *
 * @param map Selected map (0 or 1)
 */
mmc_err adn4604_active_map( uint8_t map );

/**
 * @brief Reads the outputs current connections
 *
 * @return Outputs current connection
 */
adn_connect_map_t adn4604_out_status( void );

/**
 * @brief Controls the inputs/outputs line termination
 *
 * @param cfg Selected in/outputs (separated in quadrants defined in #adn4604_term_ctl)
 */
mmc_err adn4604_termination_ctl( uint8_t cfg );

#endif
