/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2021  Krzysztof Macias <krzysztof.macias@creotech.pl>
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

/* Project Includes */
#include "port.h"
#include "idt_8v54816.h"


uint8_t clock_switch_default_config() {
    uint8_t i;
    uint8_t clock_mux_config[] = {
        // CH 0: AMC-CLK input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 1: FMC2-CLK1-M2C input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 2: FMC1-CLK1-M2C input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 3: FMC1-CLK0-M2C input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 4: FMC1-CLK2-BIDIR input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 5: SI57X_PRI_CLK
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 6: FMC2-CLK0-M2C input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 7: FMC2-CLK2-BIDIR input
        IDT_DIR_OUT | IDT_TERM_ON | IDT_POL_P | IDT_SRC_CH5,

        // CH 8: TCLKD input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 9: TCLKC input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 10: TCLKA input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 11: TCLKB input
        IDT_DIR_IN | IDT_TERM_ON | IDT_POL_P | 0,

        // CH 12: FLEX_GTP113_CLK0 output from SI57X_PRI_CLK
        IDT_DIR_OUT | IDT_TERM_ON | IDT_POL_P | IDT_SRC_CH5,

        // CH 13: FLEX_CLK1 output from SI57X_PRI_CLK
        IDT_DIR_OUT | IDT_TERM_ON | IDT_POL_P | IDT_SRC_CH5,

        // CH 14: FLEX_CLK2 output from SI57X_PRI_CLK
        IDT_DIR_OUT | IDT_TERM_ON | IDT_POL_P | IDT_SRC_CH5,

        // CH 15: FLEX_CLK3 output from SI57X_PRI_CLK
        IDT_DIR_OUT | IDT_TERM_ON | IDT_POL_P | IDT_SRC_CH5,
    };
    //FIXME: This can freeze MMC
    while(clock_switch_write_reg(clock_mux_config) != 16) {}
}

void board_init() {
    /* I2C MUX Init */
    gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR1), PIN_NUMBER(GPIO_I2C_MUX_ADDR1), GPIO_LEVEL_LOW);
    gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR2), PIN_NUMBER(GPIO_I2C_MUX_ADDR2), GPIO_LEVEL_LOW);
    gpio_set_pin_state(PIN_PORT(GPIO_I2C_SW_RESETn), PIN_NUMBER(GPIO_I2C_SW_RESETn), GPIO_LEVEL_HIGH);
}

void board_config() {
    
}
