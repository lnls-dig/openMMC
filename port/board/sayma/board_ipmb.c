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



/* Project includes */
#include "ipmb.h"
#include "port.h"


/*
 *==============================================================
 * MMC ADDRESSING
 *==============================================================
 */

/**
 * @brief Table holding all possible address values in IPMB specification
 * @see get_ipmb_addr()
 */
const unsigned char IPMBL_TABLE[IPMBL_TABLE_SIZE] = {
    0x70, 0x8A, 0x72, 0x8E, 0x92, 0x90, 0x74, 0x8C, 0x76,
    0x98, 0x9C, 0x9A, 0xA0, 0xA4, 0x88, 0x9E, 0x86, 0x84,
    0x78, 0x94, 0x7A, 0x96, 0x82, 0x80, 0x7C, 0x7E, 0xA2 };

uint8_t get_ipmb_addr( void )
{
    uint8_t ga0, ga1, ga2;
    uint8_t index;

    /* Set the test pin and read all GA pins */
    gpio_set_pin_state(PIN_PORT(GPIO_GA_TEST), PIN_NUMBER(GPIO_GA_TEST), GPIO_LEVEL_HIGH);

    /* when using NAMC-EXT-RTM at least 11 instruction cycles required
     *  to have correct GA value after GPIO_GA_TEST_PIN changes */
    {
        uint8_t i;
        for (i = 0; i < GPIO_GA_DELAY; i++) {
            asm("NOP");
        }
    }

    ga0 = gpio_read_pin(PIN_PORT(GPIO_GA0), PIN_NUMBER(GPIO_GA0));
    ga1 = gpio_read_pin(PIN_PORT(GPIO_GA1), PIN_NUMBER(GPIO_GA1));
    ga2 = gpio_read_pin(PIN_PORT(GPIO_GA2), PIN_NUMBER(GPIO_GA2));

    /* Clear the test pin and see if any GA pin has changed is value,
     * meaning that it is unconnected */
    gpio_set_pin_state(PIN_PORT(GPIO_GA_TEST), PIN_NUMBER(GPIO_GA_TEST), GPIO_LEVEL_LOW);

    /* when using NAMC-EXT-RTM at least 11 instruction cycles required
     *  to have correct GA value after GPIO_GA_TEST_PIN changes */
    {
        uint8_t i;
        for (i = 0; i < GPIO_GA_DELAY; i++) {
            asm("NOP");
        }
    }

    if ( ga0 != gpio_read_pin(PIN_PORT(GPIO_GA0), PIN_NUMBER(GPIO_GA0)) ){
        ga0 = UNCONNECTED;
    }

    if ( ga1 != gpio_read_pin(PIN_PORT(GPIO_GA1), PIN_NUMBER(GPIO_GA1)) ){
        ga1 = UNCONNECTED;
    }

    if ( ga2 != gpio_read_pin(PIN_PORT(GPIO_GA2), PIN_NUMBER(GPIO_GA2)) ){
        ga2 = UNCONNECTED;
    }

    /* Transform the 3-based code in a decimal number */
    index = (9 * ga2) + (3 * ga1) + (1 * ga0);

    if ( index >= IPMBL_TABLE_SIZE ) {
        return 0;
    }

    // If all address pins (GA) are unconnected activate bench test (outside uTCA crate)
    if (IPMBL_TABLE[index] == 0xA2) {
        bench_test = true;
    }

    return IPMBL_TABLE[index];
}
