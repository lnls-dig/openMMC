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

/* FreeRTOS Includes */
#include "FreeRTOS.h"

/* Project Includes */
#include "port.h"
#include "adn4604.h"
#include "adn4604_usercfg.h"
#include "i2c.h"
#include "i2c_mapping.h"
#include "ipmi_oem.h"

adn_connect_map_t con;

void adn4604_init( void )
{

    uint16_t out_enable_flag = {
        ADN4604_EN_OUT_0 << 0 |
        ADN4604_EN_OUT_1 << 1 |
        ADN4604_EN_OUT_2 << 2 |
        ADN4604_EN_OUT_3 << 3 |
        ADN4604_EN_OUT_4 << 4 |
        ADN4604_EN_OUT_5 << 5 |
        ADN4604_EN_OUT_6 << 6 |
        ADN4604_EN_OUT_7 << 7 |
        ADN4604_EN_OUT_8 << 8 |
        ADN4604_EN_OUT_9 << 9 |
        ADN4604_EN_OUT_10 << 10 |
        ADN4604_EN_OUT_11 << 11 |
        ADN4604_EN_OUT_12 << 12 |
        ADN4604_EN_OUT_13 << 13 |
        ADN4604_EN_OUT_14 << 14 |
        ADN4604_EN_OUT_15 << 15
    };

    /* Disable UPDATE' pin by pulling it HIGH */
    gpio_set_pin_dir( GPIO_ADN_UPDATE_PORT, GPIO_ADN_UPDATE_PIN, OUTPUT );
    gpio_set_pin_state( GPIO_ADN_UPDATE_PORT, GPIO_ADN_UPDATE_PIN, HIGH );

    /* There's a delay circuit in the Reset pin of the clock switch, we must wait until it clears out */
    gpio_set_pin_dir( GPIO_ADN_RESETN_PORT, GPIO_ADN_RESETN_PIN, INPUT );

    while( gpio_read_pin( GPIO_ADN_RESETN_PORT, GPIO_ADN_RESETN_PIN ) == 0 ) {
        vTaskDelay( 50 );
    }

    /* Configure the interconnects */
    con.out0 = ADN4604_CFG_OUT_0;
    con.out1 = ADN4604_CFG_OUT_1;
    con.out2 = ADN4604_CFG_OUT_2;
    con.out3 = ADN4604_CFG_OUT_3;
    con.out4 = ADN4604_CFG_OUT_4;
    con.out5 = ADN4604_CFG_OUT_5;
    con.out6 = ADN4604_CFG_OUT_6;
    con.out7 = ADN4604_CFG_OUT_7;
    con.out8 = ADN4604_CFG_OUT_8;
    con.out9 = ADN4604_CFG_OUT_9;
    con.out10 = ADN4604_CFG_OUT_10;
    con.out11 = ADN4604_CFG_OUT_11;
    con.out12 = ADN4604_CFG_OUT_12;
    con.out13 = ADN4604_CFG_OUT_13;
    con.out14 = ADN4604_CFG_OUT_14;
    con.out15 = ADN4604_CFG_OUT_15;

    adn4604_xpt_config( ADN_XPT_MAP0_CON_REG, con );

    /* Enable desired outputs */
    for ( uint8_t i = 0; i < 16; i++ ) {
        if ( ( out_enable_flag >> i ) & 0x1 ) {
            adn4604_tx_control( i, TX_ENABLED );
        }
    }

    adn4604_active_map( ADN_XPT_MAP0 );

    adn4604_update();
}

void adn4604_tx_enable( uint8_t output )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t enable[2];

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
    enable[1] = 0x30;

    if (i2c_take_by_chipid(CHIP_ID_ADN, &i2c_addr, &i2c_interf, (TickType_t)10) ) {
        xI2CMasterWrite( i2c_interf, i2c_addr, enable, sizeof(enable) );
        i2c_give(i2c_interf);
    }
}

void adn4604_update( void )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t update[2] = { ADN_XPT_UPDATE_REG, 0x01 };

    if (i2c_take_by_chipid(CHIP_ID_ADN, &i2c_addr, &i2c_interf, (TickType_t)10) ) {
        xI2CMasterWrite( i2c_interf, i2c_addr, update, sizeof(update) );
        i2c_give(i2c_interf);
    }
}

void adn4604_reset( void )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t update[2] = { ADN_RESET_REG, 0x01 };

    if (i2c_take_by_chipid(CHIP_ID_ADN, &i2c_addr, &i2c_interf, (TickType_t)10) ) {
        xI2CMasterWrite( i2c_interf, i2c_addr, update, sizeof(update) );
        i2c_give(i2c_interf);
    }
}

void adn4604_xpt_config( uint8_t map, adn_connect_map_t xpt_con )
{
    uint8_t i2c_addr, i2c_interf;

    adn_connect_cfg_t cfg = { map, xpt_con };

    if (i2c_take_by_chipid(CHIP_ID_ADN, &i2c_addr, &i2c_interf, (TickType_t)10) ) {
        xI2CMasterWrite( i2c_interf, i2c_addr, (uint8_t *)&cfg, sizeof(cfg) );
        i2c_give(i2c_interf);
    }
}

void adn4604_active_map( uint8_t map )
{
    uint8_t i2c_addr, i2c_interf;

    /* Select the active map */
    uint8_t map_sel[2] = { ADN_XPT_MAP_TABLE_SEL_REG, map };

    if ( i2c_take_by_chipid( CHIP_ID_ADN, &i2c_addr, &i2c_interf, (TickType_t)10 ) ) {
        xI2CMasterWrite( i2c_interf, i2c_addr, map_sel, sizeof(map_sel) );
        i2c_give( i2c_interf );
    }
}

adn_connect_map_t adn4604_out_status( void )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t i;
    adn_connect_map_t stat_map = {0};

    if ( i2c_take_by_chipid( CHIP_ID_ADN, &i2c_addr, &i2c_interf, (TickType_t)10 ) ) {

        /* Read all outputs status */
        for ( i = 0; i < 8; i++ ) {
            xI2CMasterWriteRead( i2c_interf, i2c_addr, ADN_XPT_STATUS_REG+i, (uint8_t *)(&stat_map)+i, 1 );
        }
        i2c_give( i2c_interf );
    }

    return stat_map;
}

void adn4604_termination_ctl( uint8_t cfg )
{
    uint8_t i2c_addr, i2c_interf;
    uint8_t msg[2] = { ADN_TERMINATION_CTL_REG, (cfg & 0xF0) };

    if ( i2c_take_by_chipid( CHIP_ID_ADN, &i2c_addr, &i2c_interf, (TickType_t)10 ) ) {
        xI2CMasterWrite( i2c_interf, i2c_addr, msg, sizeof(msg) );
        i2c_give( i2c_interf );
    }
}

IPMI_HANDLER(ipmi_oem_clock_crossbar_set, NETFN_CUSTOM_OEM, IPMI_OEM_CMD_CLOCK_CROSSBAR_SET, ipmi_msg *req, ipmi_msg* rsp)
{
    int len = rsp->data_len = 0;

    uint8_t map;
    uint8_t output = req->data[1];
    uint8_t input = req->data[2];
    uint8_t enable = req->data[3];

    if (output % 2) {
        *((uint8_t *)&con+(output/2)) &= 0x0F;
        *((uint8_t *)&con+(output/2)) |= (input << 4) & 0xF0;
    } else {
        *((uint8_t *)&con+(output/2)) &= 0xF0;
        *((uint8_t *)&con+(output/2)) |= input & 0x0F;
    }

    if ( req->data[0] == 0 ) {
        map = ADN_XPT_MAP0_CON_REG;
    } else {
        map = ADN_XPT_MAP1_CON_REG;
    }

    adn4604_xpt_config( map , con );

    if ( enable ) {
        adn4604_tx_control( output, TX_ENABLED );
    } else {
        adn4604_tx_control( output, TX_DISABLED );
    }

    adn4604_update();

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_oem_clock_crossbar_reset, NETFN_CUSTOM_OEM, 0x06, ipmi_msg *req, ipmi_msg* rsp)
{
    adn4604_reset();

    rsp->data_len = 0;
    rsp->completion_code = IPMI_CC_OK;
}
