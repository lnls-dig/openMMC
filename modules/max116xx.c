/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2021  Augusto Fraga Giachero <augusto.fraga@cnpem.br>
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
 * @file   max116xx.c
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  MAX116XX ADC driver
 *
 * @ingroup MAX116XX
 */

#include "max116xx.h"
#include "i2c.h"

mmc_err max116xx_set_config(uint8_t chip_id, const max116xx_cfg* cfg)
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t tx_len;
    uint8_t setup_cfg[2] = {0};

    if (cfg == NULL) {
        return MMC_INVALID_ARG_ERR;
    }

    setup_cfg[0] = 0x80 | cfg->ref_sel | cfg->clk_sel | cfg->pol_sel | 0b10;
    setup_cfg[1] = cfg->scan_mode | (cfg->channel_sel << 1) | cfg->diff_mode;

    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, pdMS_TO_TICKS(10))) {
        tx_len = xI2CMasterWrite(i2c_id, i2c_addr, setup_cfg, sizeof(setup_cfg));
        i2c_give(i2c_id);
    } else {
        return MMC_TIMEOUT_ERR;
    }

    if (tx_len != sizeof(setup_cfg)) {
        return MMC_IO_ERR;
    }

    return MMC_OK;
}

mmc_err max116xx_read_uni(uint8_t chip_id, int16_t data[], uint8_t samples)
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx_len = 0;

    if (data == NULL) {
        return MMC_INVALID_ARG_ERR;
    }

    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, pdMS_TO_TICKS(10))) {
        rx_len = xI2CMasterRead(i2c_id, i2c_addr, (uint8_t*)data, (samples * 2));
        i2c_give(i2c_id);
    } else {
        return MMC_TIMEOUT_ERR;
    }

    /*
     * Convert from big endian to little endian
     */
    for (uint8_t i = 0; i < samples; i++)
    {
        data[i] = ((data[i] >> 8) & 0x00FF) | ((data[i] << 8) & 0x0300);
    }

    if (rx_len != (samples * 2)) {
        return MMC_IO_ERR;
    }

    return MMC_OK;
}

mmc_err max116xx_read_bip(uint8_t chip_id, int16_t data[], uint8_t samples)
{
    uint8_t i2c_addr;
    uint8_t i2c_id;
    uint8_t rx_len = 0;

    if (data == NULL) {
        return MMC_INVALID_ARG_ERR;
    }

    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, pdMS_TO_TICKS(10))) {
        rx_len = xI2CMasterRead(i2c_id, i2c_addr, (uint8_t*)data, (samples * 2));
        i2c_give(i2c_id);
    } else {
        return MMC_TIMEOUT_ERR;
    }

    /*
     * Convert from big endian to little endian
     */
    for (uint8_t i = 0; i < samples; i++)
    {
        data[i] = ((data[i] >> 8) & 0x00FF) | ((data[i] << 8) & 0x0300);

        /*
         * Sign extend if MSB = 1
         */
        if (data[i] & (1 << 9)) {
            data[i] |= 0xFC00;
        }
    }

    if (rx_len != (samples * 2)) {
        return MMC_IO_ERR;
    }

    return MMC_OK;
}
