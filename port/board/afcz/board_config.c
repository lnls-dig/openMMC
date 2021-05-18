/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2021  Wojciech Ruclo <wojciech.ruclo@creotech.pl>
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

#include "board_config.h"
#include "pin_mapping.h"
#include "i2c_mapping.h"
#include "stopwatch.h"
#include "tca9539.h"
#include "idt_8v54816.h"

#include "afcz_si5341_LVDS18.h"
#include "afcz_tca9539_clk.h"

static void tca9539_clk_config()
{
    uint8_t ret = 0;

    uint8_t pin_dir = 1 << 2 | GPIO_CLK_Si5341_INTR_N | GPIO_CLK_Si5341_SYNC_B; // set CLK_Si5341_INTR_N and CLK_Si5341_SYNC_B pins as inputs
    tca9539_port_dir_set(CHIP_ID_TCA9539_CLK, 0, pin_dir);
    tca9539_port_dir_set(CHIP_ID_TCA9539_CLK, 1, 0);

    uint8_t output = GPIO_CLK_CLK_SW_RSTn | GPIO_CLK_SI57X_OE1 | GPIO_CLK_SI53xx_RST | GPIO_CLK_IN_SEL1 | GPIO_CLK_IN_SEL0;
    ret = tca9539_output_port_set(CHIP_ID_TCA9539_CLK, 0, output);

    if (!ret) {
        printf("Failed to configure CLK GPIO expander.\n");
    }
}

static void crosspoint_8V54816_config() {

    uint8_t ret = 0;

    // Reset
    tca9539_output_pin_set(CHIP_ID_TCA9539_CLK, 0, 7, 0x0);
    tca9539_output_pin_set(CHIP_ID_TCA9539_CLK, 0, 7, 0x1);
    StopWatch_DelayMs(100);

    // Test configuration
    ret |= clock_switch_set_single_channel(0,  IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // TCLKA IN
    ret |= clock_switch_set_single_channel(1,  IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // TCLKB IN
    ret |= clock_switch_set_single_channel(2,  IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // FMC2_CLK0_M2C
    ret |= clock_switch_set_single_channel(3,  IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // FMC1_CLK3_BIDIR (option: FMC1 CLK1 M2C)
    ret |= clock_switch_set_single_channel(4,  IDT_DIR_IN  | IDT_TERM_OFF | IDT_POL_P);                   // Si57X_2 IN
    ret |= clock_switch_set_single_channel(5,  IDT_DIR_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH6);     // MGT_REFCLK0_228 OUT
    ret |= clock_switch_set_single_channel(6,  IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // Si5341_OUT7 IN (option: TCLKC)
    ret |= clock_switch_set_single_channel(7,  IDT_DIR_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH6);     // MGT_REFCLK0_227 OUT (option: TCLKD)
    ret |= clock_switch_set_single_channel(8,  IDT_DIR_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH6);     // MGT_REFCLK0_226 OUT
    ret |= clock_switch_set_single_channel(9,  IDT_DIR_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH12);    // Si5341_IN1 OUT
    ret |= clock_switch_set_single_channel(10, IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // FMC2_CLK2_BIDIR
    ret |= clock_switch_set_single_channel(11, IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // FMC2_CLK3_BIDIR (option: FMC2_CLK1_M2C)
    ret |= clock_switch_set_single_channel(12, IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // RTM_CLK IN (option: AMC_CLK OUT)
    ret |= clock_switch_set_single_channel(13, IDT_DIR_IN  | IDT_TERM_OFF | IDT_POL_P);                   // Si57X IN (option: Si5341_IN0 OUT)
    ret |= clock_switch_set_single_channel(14, IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // FMC1_CLK2_BIDIR (option: FPGA_CLK1)
    ret |= clock_switch_set_single_channel(15, IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // FMC1_CLK0_M2C

    // Default configuration
    // ret |= clock_switch_set_single_channel(0,  IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // TCLKA IN
    // ret |= clock_switch_set_single_channel(1,  IDT_DIR_IN  | IDT_TERM_OFF | IDT_POL_P);                   // TCLKB IN
    // ret |= clock_switch_set_single_channel(2,  IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // FMC2_CLK0_M2C
    // ret |= clock_switch_set_single_channel(3,  IDT_DIR_IN  | IDT_TERM_OFF | IDT_POL_P);                   // FMC1_CLK3_BIDIR (option: FMC1 CLK1 M2C)
    // ret |= clock_switch_set_single_channel(4,  IDT_DIR_IN  | IDT_TERM_OFF | IDT_POL_P);                   // Si57X_2 IN
    // ret |= clock_switch_set_single_channel(5,  IDT_DIR_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH13);    // MGT_REFCLK0_228 OUT
    // ret |= clock_switch_set_single_channel(6,  IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // Si5341_OUT7 IN (option: TCLKC)
    // ret |= clock_switch_set_single_channel(7,  IDT_DIR_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH13);    // MGT_REFCLK0_227 OUT (option: TCLKD)
    // ret |= clock_switch_set_single_channel(8,  IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // MGT_REFCLK0_226 OUT
    // ret |= clock_switch_set_single_channel(9,  IDT_DIR_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH13);    // Si5341_IN1 OUT
    // ret |= clock_switch_set_single_channel(10, IDT_DIR_IN  | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH12);    // FMC2_CLK2_BIDIR
    // ret |= clock_switch_set_single_channel(11, IDT_DIR_OUT | IDT_TERM_OFF | IDT_POL_P);                   // FMC2_CLK3_BIDIR (option: FMC2_CLK1_M2C)
    // ret |= clock_switch_set_single_channel(12, IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // RTM_CLK IN (option: AMC_CLK OUT)
    // ret |= clock_switch_set_single_channel(13, IDT_DIR_IN  | IDT_TERM_OFF | IDT_POL_P);                   // Si57X IN (option: Si5341_IN0 OUT)
    // ret |= clock_switch_set_single_channel(14, IDT_DIR_OUT | IDT_TERM_OFF | IDT_POL_P | IDT_SRC_CH12);    // FMC1_CLK2_BIDIR (option: FPGA_CLK1)
    // ret |= clock_switch_set_single_channel(15, IDT_DIR_IN  | IDT_TERM_ON  | IDT_POL_P);                   // FMC1_CLK0_M2C

    if (!ret) {
        printf("Failed to configure 8V54816A clock cross-point switch.\n");
    }
}

static void si5341_config()
{
    uint8_t ret = 0;

    // Reset
    tca9539_output_pin_set(CHIP_ID_TCA9539_CLK, 0, 5, 0x0);
    tca9539_output_pin_set(CHIP_ID_TCA9539_CLK, 0, 5, 0x1);
    StopWatch_DelayMs(100);

    uint8_t i2c_interf, i2c_addr;
    uint8_t data[2];

    // Write configuration
    if (i2c_take_by_chipid(CHIP_ID_SI5341, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        for (uint8_t i = 0; i < 6; i++) {
            // extract page address
            data[0] = 0x01;
            data[1] = ((si5341_revb_registers[i].address) >> 8) & 0xFF;
            xI2CMasterWrite(i2c_interf, i2c_addr, data, 2); // set page

            data[0] = ((si5341_revb_registers[i].address)) & 0xFF;
            data[1] = si5341_revb_registers[i].value;
            ret = xI2CMasterWrite(i2c_interf, i2c_addr, data, 2);
        }
        i2c_give(i2c_interf);
    }

    StopWatch_DelayMs(300);

    if ( i2c_take_by_chipid( CHIP_ID_SI5341, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE ) {
        for (int i = 6; i < SI5341_REVB_REG_CONFIG_NUM_REGS; i++) {
            // extract page address
            data[0] = 0x01;
            data[1] = ((si5341_revb_registers[i].address) >>8) & 0xFF;
            xI2CMasterWrite( i2c_interf, i2c_addr, data, 2); // set page

            data[0] = ((si5341_revb_registers[i].address) ) & 0xFF;
            data[1] = si5341_revb_registers[i].value;
            ret = xI2CMasterWrite( i2c_interf, i2c_addr, data, 2);
        }
        i2c_give( i2c_interf );
    }

    if (!ret) {
        printf("Failed to configure Si5341.\n");
    }
}

static void ethernet_init()
{
    gpio_set_pin_state(PIN_PORT(GPIO_PHY_MII1_MODE), PIN_NUMBER(GPIO_PHY_MII1_MODE), true);
    vTaskDelay(100);
    gpio_set_pin_state(PIN_PORT(GPIO_PHY_RESETn), PIN_NUMBER(GPIO_PHY_RESETn), true);
}


void board_init(void)
{
    StopWatch_Init();

    /* I2C MUX Initialization */
    gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR1), PIN_NUMBER(GPIO_I2C_MUX_ADDR1), GPIO_LEVEL_LOW);
    gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR2), PIN_NUMBER(GPIO_I2C_MUX_ADDR2), GPIO_LEVEL_LOW);
    gpio_set_pin_state(PIN_PORT(GPIO_SW_RESETn), PIN_NUMBER(GPIO_SW_RESETn), GPIO_LEVEL_HIGH);

    /* I2C FIX for 1776 */
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 1, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
    Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_FUNC3 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));

    /* Configure JTAG chain */
    gpio_set_pin_state(PIN_PORT(GPIO_SOC_JTAG_OVERRIDE), PIN_NUMBER(GPIO_SOC_JTAG_OVERRIDE), GPIO_LEVEL_HIGH);
}

void board_config()
{
    ethernet_init();

    /* CLK GPIO expander configuration */
    tca9539_clk_config();

    /* CLK cross-point switch configuration */
    // crosspoint_8V54816_config();

    /* Si5341 configuration */
    // si5341_config();
}
