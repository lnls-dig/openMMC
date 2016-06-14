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

void adn4604_setup(void)
{
    uint8_t i2c_addr;
    uint8_t i2c_interf;
    t_adn_connect_map con;
    t_adn_connect_cfg cfg;

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
    gpio_set_pin_dir(GPIO_ADN_UPDATE_PORT, GPIO_ADN_UPDATE_PIN, OUTPUT);
    gpio_set_pin_state( GPIO_ADN_UPDATE_PORT, GPIO_ADN_UPDATE_PIN, HIGH);

    /* There's a delay circuit in the Reset pin of the clock switch, we must wait until it clears out */
    gpio_set_pin_dir( GPIO_ADN_RESETN_PORT, GPIO_ADN_RESETN_PIN, INPUT);

    while( gpio_read_pin( GPIO_ADN_RESETN_PORT, GPIO_ADN_RESETN_PIN ) == 0) {
        vTaskDelay(50);
    }

    if (i2c_take_by_chipid(CHIP_ID_ADN, &i2c_addr, &i2c_interf, (TickType_t)10) == pdFALSE) {
        return;
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

    /* Select the desired MAP register to load the configuration */
    cfg.map_reg = ADN_XPT_MAP0_CON_REG;
    cfg.map_connect = con;

    xI2CMasterWrite( i2c_interf, i2c_addr, (uint8_t *)&cfg, sizeof(cfg) );

    /* Select the active map */
    uint8_t map_sel[2] = { ADN_XPT_MAP_TABLE_SEL_REG, ADN_XPT_MAP0 };
    xI2CMasterWrite( i2c_interf, i2c_addr, map_sel, sizeof(map_sel) );

    /* Enable desired outputs */
    for (uint8_t i = 0; i < 16; i++) {
        if ( (out_enable_flag >> i) & 0x1 ) {
            adn4604_tx_enable(i2c_interf, i2c_addr, i);
        }
    }

    adn4604_update( i2c_interf, i2c_addr );

    i2c_give(i2c_interf);
}

void adn4604_tx_enable( uint8_t i2c_interf, uint8_t i2c_addr, uint8_t output )
{
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

    xI2CMasterWrite( i2c_interf, i2c_addr, enable, sizeof(enable) );
}

void adn4604_update( uint8_t i2c_interf, uint8_t i2c_addr )
{
    uint8_t update[2] = { ADN_XPT_UPDATE_REG, 0x01 };

    xI2CMasterWrite( i2c_interf, i2c_addr, update, sizeof(update) );
}
