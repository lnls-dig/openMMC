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
 * @file   adn4604.c
 * @author Henrique Silva <henrique.silva@lnls.br>
 *
 * @brief  ADN4604 module interface implementations
 *
 * @ingroup ADN4604
 */

/* FreeRTOS Includes */
#include "FreeRTOS.h"

/* Project Includes */
#include "port.h"
#include "adn4604.h"
#include "i2c.h"
#include "i2c_mapping.h"

adn_connect_map_t con;

mmc_err adn4604_tx_control( uint8_t output, uint8_t tx_mode )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t enable[2];
    uint8_t tx_len = 0;

    /* TX Enable registers have an 0x20 offset from their value */
    enable[0] = 0x20 + output;

    /* TX Basic Control Register flags:
     * [6] TX CTL SELECT - 0: PE and output level control is derived from common lookup table
     *                     1: PE and output level control is derived from per port drive control registers
     * [5:4] TX EN[1:0]  - 00: TX disabled, lowest power state
     *                     01: TX standby
     *                     10: TX squelched
     *                     11: TX enabled
     * [3] Reserved      - Set to 0
     * [2:1] PE[2:0]     - If TX CTL SELECT = 0,
     *                       000: Table Entry 0
     *                       001: Table Entry 1
     *                       010: Table Entry 2
     *                       011: Table Entry 3
     *                       100: Table Entry 4
     *                       101: Table Entry 5
     *                       110: Table Entry 6
     *                       111: Table Entry 7
     *                   - If TX CTL SELECT = 1, PE[2:0] are ignored
     */
    enable[1] = tx_mode << 4;

    if (i2c_take_by_chipid(CHIP_ID_ADN, &i2c_addr, &i2c_interf, pdMS_TO_TICKS(10)) ) {
        tx_len = xI2CMasterWrite( i2c_interf, i2c_addr, enable, sizeof(enable) );
        i2c_give(i2c_interf);
    }

    if (!tx_len) return MMC_TIMEOUT_ERR;
    return MMC_OK;
}

mmc_err adn4604_update( void )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t update[2] = { ADN_XPT_UPDATE_REG, 0x01 };
    uint8_t tx_len = 0;

    if (i2c_take_by_chipid(CHIP_ID_ADN, &i2c_addr, &i2c_interf, pdMS_TO_TICKS(10)) ) {
        tx_len = xI2CMasterWrite( i2c_interf, i2c_addr, update, sizeof(update) );
        i2c_give(i2c_interf);
    }

    if (!tx_len) return MMC_TIMEOUT_ERR;
    return MMC_OK;
}

mmc_err adn4604_reset( void )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t update[2] = { ADN_RESET_REG, 0x01 };
    uint8_t tx_len = 0;

    if (i2c_take_by_chipid(CHIP_ID_ADN, &i2c_addr, &i2c_interf, pdMS_TO_TICKS(10)) ) {
        tx_len = xI2CMasterWrite( i2c_interf, i2c_addr, update, sizeof(update) );
        i2c_give(i2c_interf);
    }
    if (!tx_len) return MMC_TIMEOUT_ERR;
    return MMC_OK;
}

mmc_err adn4604_xpt_config( uint8_t map, adn_connect_map_t xpt_con )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t tx_len = 0;

    adn_connect_cfg_t cfg = { map, xpt_con };

    if (i2c_take_by_chipid(CHIP_ID_ADN, &i2c_addr, &i2c_interf, pdMS_TO_TICKS(10)) ) {
        tx_len = xI2CMasterWrite( i2c_interf, i2c_addr, (uint8_t *)&cfg, sizeof(cfg) );
        i2c_give(i2c_interf);
    }
    if (!tx_len) return MMC_TIMEOUT_ERR;
    return MMC_OK;
}

mmc_err adn4604_active_map( uint8_t map )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t tx_len = 0;

    /* Select the active map */
    uint8_t map_sel[2] = { ADN_XPT_MAP_TABLE_SEL_REG, map };

    if ( i2c_take_by_chipid( CHIP_ID_ADN, &i2c_addr, &i2c_interf, pdMS_TO_TICKS(10) ) ) {
        tx_len = xI2CMasterWrite( i2c_interf, i2c_addr, map_sel, sizeof(map_sel) );
        i2c_give( i2c_interf );
    }
    if (!tx_len) return MMC_TIMEOUT_ERR;
    return MMC_OK;
}

adn_connect_map_t adn4604_out_status( void )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t i;
    uint8_t cmd;
    adn_connect_map_t stat_map = {0};

    if ( i2c_take_by_chipid( CHIP_ID_ADN, &i2c_addr, &i2c_interf, pdMS_TO_TICKS(10) ) ) {

        /* Read all outputs status */
        for ( i = 0; i < 8; i++ ) {
            cmd = ADN_XPT_STATUS_REG+i;
            xI2CMasterWriteRead( i2c_interf, i2c_addr, &cmd, 1, (uint8_t *)(&stat_map)+i, 1 );
        }
        i2c_give( i2c_interf );
    }

    return stat_map;
}

mmc_err adn4604_termination_ctl( uint8_t cfg )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t msg[2] = { ADN_TERMINATION_CTL_REG, (cfg & 0xF0) };
    uint8_t tx_len = 0;

    if ( i2c_take_by_chipid( CHIP_ID_ADN, &i2c_addr, &i2c_interf, pdMS_TO_TICKS(10) ) ) {
        tx_len = xI2CMasterWrite( i2c_interf, i2c_addr, msg, sizeof(msg) );
        i2c_give( i2c_interf );
    }
    if (!tx_len) return MMC_TIMEOUT_ERR;
    return MMC_OK;

}
