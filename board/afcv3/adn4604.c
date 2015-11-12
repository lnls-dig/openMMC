/*
 *   adn4604.c
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

#include "FreeRTOS.h"
#include "adn4604.h"
#include "pin_mapping.h"
#include "board_version.h"

void adn4604_setup(void) {
    uint8_t i2c_bus_id = I2C1;
    uint8_t adn_slave_addr = 0x4B;
    t_adn_connect_map con;
    t_adn_connect_cfg cfg;

    /* Disable UPDATE' pin by pulling it HIGH */
    gpio_set_pin_state( GPIO_ADN_UPDATE_PORT, GPIO_ADN_UPDATE_PIN, HIGH);

    if (afc_i2c_take_by_busid(I2C_BUS_CPU_ID, &i2c_bus_id, (TickType_t)10) == pdFALSE) {
        return;
    }

    /* Configure the interconnects */
    con.out0 = 0;
    con.out1 = 0;
    con.out2 = 0;
    con.out3 = 0;
    con.out4 = 13;
    con.out5 = 8;
    con.out6 = 8;
    con.out7 = 8;
    con.out8 = 8;
    con.out9 = 8;
    con.out10 = 14;
    con.out11 = 14;
    con.out12 = 14;
    con.out13 = 5;
    con.out14 = 14;
    con.out15 = 14;

    /* Select the desired MAP register to load the configuration */
    cfg.map_reg = ADN_XPT_MAP0_CON_REG;
    cfg.map_connect = con;

    xI2CMasterWrite( i2c_bus_id, adn_slave_addr, (uint8_t *)&cfg, sizeof(cfg) );

    /* Select the active map */
    uint8_t map_sel[2] = { ADN_XPT_MAP_TABLE_SEL_REG, ADN_XPT_MAP0 };
    xI2CMasterWrite( i2c_bus_id, adn_slave_addr, map_sel, sizeof(map_sel) );

    /* Enable desired outputs */
    for (uint8_t i = 0; i < 16; i++) {
    	adn4604_tx_enable(i2c_bus_id, adn_slave_addr, i);
    }

    adn4604_update( i2c_bus_id, adn_slave_addr );

    afc_i2c_give(i2c_bus_id);
}

void adn4604_tx_enable( uint8_t i2c_bus_id, uint8_t slave_addr, uint8_t output )
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

    xI2CMasterWrite( i2c_bus_id, slave_addr, enable, sizeof(enable) );
}

void adn4604_update( uint8_t i2c_bus_id, uint8_t slave_addr )
{
    uint8_t update[2] = { ADN_XPT_UPDATE_REG, 0x01 };

    xI2CMasterWrite( i2c_bus_id, slave_addr, update, sizeof(update) );
}
