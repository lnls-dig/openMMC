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
 * @file   max116xx.h
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  MAX116XX ADC driver function declarations
 *
 * @ingroup MAX116XX
 */

#ifndef MAX116XX_H_
#define MAX116XX_H_

#include "error.h"
#include "port.h"

/**
 * @brief MAX11606-MAX11611 reference selection enum
 */
enum max116xx_ref_sel {
    MAX116XX_REF_VDD = 0b0000000, // VDD as voltage reference, AIN_/REF configured as an analog input
    MAX116XX_REF_EXT = 0b0100000, // External reference, AIN_/REF configured as an reference input
    MAX116XX_REF_INT_OFF = 0b1000000, // Internal reference off, AIN_/REF configured as an analog input
    MAX116XX_REF_INT_ON = 0b1010000, // Internal reference on, AIN_/REF configured as an analog input
    MAX116XX_REF_INT_OFF_OUT = 0b1100000, // Internal reference off, AIN_/REF configured as reference output
    MAX116XX_REF_INT_ON_OUT = 0b1110000, // Internal reference on, AIN_/REF configured as reference output
};

enum max116xx_clk_sel {
    MAX116XX_CLK_INT = 0b0000,
    MAX116XX_CLK_EXT = 0b1000,
};

enum max116xx_pol_sel {
    MAX116XX_UNIPOLAR = 0b000,
    MAX116XX_BIPOLAR = 0b100,
};

enum max116xx_scan_mode {
    MAX116XX_SCAN_FROM_AIN0 = 0b000000,
    MAX116XX_SCAN_OFF = 0b010000,
    MAX116XX_SCAN_UPPER = 0b100000,
    MAX116XX_SCAN_OFF_SINGLE_CONV = 0b110000,
};

enum max116xx_diff_mode {
    MAX116XX_DIFFERENTIAL = 0,
    MAX116XX_SINGLE_ENDED = 1,
};

/**
 * @brief MAX11606-MAX11611 configuration struct
 */
typedef struct {
    enum max116xx_ref_sel ref_sel;
    enum max116xx_clk_sel clk_sel;
    enum max116xx_pol_sel pol_sel;
    enum max116xx_scan_mode scan_mode;
    uint8_t channel_sel;
    enum max116xx_diff_mode diff_mode;
} max116xx_cfg;

/**
 * @brief Configure the MAX11606-MAX11611 ADC
 *
 * @param[in] chip_id Chip ID to communicate
 * @param[in] cfg     max116xx_cfg struct with the current configuration
 *
 * @return MMC_OK if success, an error code otherwise
 */
mmc_err max116xx_set_config(uint8_t chip_id, const max116xx_cfg* cfg);

/**
 * @brief Convert and read single or multiple channels, unipolar mode
 *
 * @param[in]  chip_id Chip ID to communicate
 * @param[out] data    Array containing the samples, 0 - +1023 range
 * @param[in]  samples Number of samples requested
 *
 * @return MMC_OK if success, an error code otherwise
 */
mmc_err max116xx_read_uni(uint8_t chip_id, int16_t data[], uint8_t samples);

/**
 * @brief Convert and read single or multiple channels, bipolar mode
 *
 * @param[in]  chip_id Chip ID to communicate
 * @param[out] data    Array containing the samples, -512 - +511 range
 * @param[in]  samples Number of samples requested
 *
 * @return MMC_OK if success, an error code otherwise
 */
mmc_err max116xx_read_bip(uint8_t chip_id, int16_t data[], uint8_t samples);

#endif
