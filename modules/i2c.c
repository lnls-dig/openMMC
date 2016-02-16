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

/*!
 * @file i2c.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date August 2015
 *
 * @brief Implementation of a generic I2C driver using FreeRTOS features
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"

/* Project includes */
#include "i2c.h"
#include "pin_mapping.h"
#include "port.h"

void vI2CInit( I2C_ID_T i2c_id, uint32_t speed, I2C_Mode mode )
{

    /* Enable and configure I2C clock */
    vI2CConfig( i2c_id, speed );

    if ( mode == I2C_Mode_IPMB ) {
        /* Configure Slave Address */
        ipmb_addr = get_ipmb_addr( );

        vI2CSlaveSetup( i2c_id, ipmb_addr );
    }

} /* End of vI2C_Init */

/*
 *==============================================================
 * MMC ADDRESSING
 *==============================================================
 */

/*! @brief Table holding all possible address values in IPMB specification
 * @see get_ipmb_addr()
 */
unsigned char IPMBL_TABLE[IPMBL_TABLE_SIZE] = {
    0x70, 0x8A, 0x72, 0x8E, 0x92, 0x90, 0x74, 0x8C, 0x76,
    0x98, 0x9C, 0x9A, 0xA0, 0xA4, 0x88, 0x9E, 0x86, 0x84,
    0x78, 0x94, 0x7A, 0x96, 0x82, 0x80, 0x7C, 0x7E, 0xA2 };

/*! The state of each GA signal is represented by G (grounded), U (unconnected),
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
 */
#define GPIO_GA_DELAY 10

uint8_t get_ipmb_addr( void )
{
    uint8_t ga0, ga1, ga2;
    uint8_t index;

    /* Set the test pin and read all GA pins */
    gpio_set_pin_dir(GA_TEST_PORT, GA_TEST_PIN, OUTPUT);
    gpio_set_pin_state(GA_TEST_PORT, GA_TEST_PIN, HIGH);

    /* when using NAMC-EXT-RTM at least 11 instruction cycles required
     *  to have correct GA value after GA_TEST_PIN changes */
    {
        uint8_t i;
        for (i = 0; i < GPIO_GA_DELAY; i++){
            __asm volatile ("nop");
        }
    }

    ga0 = gpio_read_pin(GA0_PORT, GA0_PIN);
    ga1 = gpio_read_pin(GA1_PORT, GA1_PIN);
    ga2 = gpio_read_pin(GA2_PORT, GA2_PIN);

    /* Clear the test pin and see if any GA pin has changed is value,
     * meaning that it is unconnected */
    gpio_set_pin_state(GA_TEST_PORT, GA_TEST_PIN, LOW);

    /* when using NAMC-EXT-RTM at least 11 instruction cycles required
     *  to have correct GA value after GA_TEST_PIN changes */
    {
        uint8_t i;
        for (i = 0; i < GPIO_GA_DELAY; i++)
            __asm volatile ("nop");
    }


    if ( ga0 != gpio_read_pin(GA0_PORT, GA0_PIN) ){
        ga0 = UNCONNECTED;
    }

    if ( ga1 != gpio_read_pin(GA1_PORT, GA1_PIN) ){
        ga1 = UNCONNECTED;
    }

    if ( ga2 != gpio_read_pin(GA2_PORT, GA2_PIN) ){
        ga2 = UNCONNECTED;
    }

    /* Transform the 3-based code in a decimal number */
    index = (9 * ga2) + (3 * ga1) + (1 * ga0);

    if ( index >= IPMBL_TABLE_SIZE ){
        return 0;
    }

    return IPMBL_TABLE[index];
}
#undef GPIO_GA_DELAY
