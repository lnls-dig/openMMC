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
 * @file   cdce906.h
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  CDCE906 PLL driver function declarations
 *
 * @ingroup CDCE906
 */

#ifndef CDCE906_H_
#define CDCE906_H_

#include <stdint.h>

enum cdce906_clk_src {
    CDCE906_CLK_SRC_CRYSTAL,
    CDCE906_CLK_SRC_CLKIN0_LVCMOS,
    CDCE906_CLK_SRC_CLKIN1_LVCMOS,
    CDCE906_CLK_SRC_CLKIN_DIFF,
    CDCE906_CLK_SRC_INVALID,
};

enum cdce906_pll_mux {
    CDCE906_PLL_MUX_PLL,
    CDCE906_PLL_MUX_VCO,
};

enum cdce906_pll_fvco {
    CDCE906_PLL_FVCO_80_200MHZ,
    CDCE906_PLL_FVCO_180_300MHZ,
};

enum cdce906_s0_cfg {
    CDCE906_S0_CFG_POWER_DOWN_CTRL,
    CDCE906_S0_CFG_PLL_DIV_BYPASS_CTRL,
    CDCE906_S0_CFG_CLKSEL_CTRL,
    CDCE906_S0_CFG_I2C_ADDR,
};

enum cdce906_s1_cfg {
    CDCE906_S1_CFG_Yx_FIXED_OUTPUT_CTRL,
    CDCE906_S1_CFG_Yx_TRISTATE_CTRL,
    CDCE906_S1_CFG_I2C_ADDR,
    CDCE906_S1_CFG_INVALID,
};

enum cdce906_px_pll_sel {
    CDCE906_Px_PLL_SEL_BYPASS,
    CDCE906_Px_PLL_SEL_PLL1,
    CDCE906_Px_PLL_SEL_PLL2,
    CDCE906_Px_PLL_SEL_PLL2_SSC,
    CDCE906_Px_PLL_SEL_PLL3,
    CDCE906_Px_PLL_SEL_INVALID,
};

enum cdce906_yx_out_cfg {
    CDCE906_Yx_OUT_CFG_EN,
    CDCE906_Yx_OUT_CFG_EN_INV,
    CDCE906_Yx_OUT_CFG_DIS_LOW,
    CDCE906_Yx_OUT_CFG_DIS_HIGH,
};

enum cdce906_yx_slew_cfg {
    CDCE906_Yx_SLEW_CFG_NOMINAL,
    CDCE906_Yx_SLEW_CFG_NOMINAL_1NS,
    CDCE906_Yx_SLEW_CFG_NOMINAL_2NS,
    CDCE906_Yx_SLEW_CFG_NOMINAL_3NS,
};

enum cdce906_yx_px_sel {
    CDCE906_Yx_Px_SEL_P0,
    CDCE906_Yx_Px_SEL_P1,
    CDCE906_Yx_Px_SEL_P2,
    CDCE906_Yx_Px_SEL_P3,
    CDCE906_Yx_Px_SEL_P4,
    CDCE906_Yx_Px_SEL_P5,
    CDCE906_Yx_Px_SEL_INVALID,
};

typedef struct {
    enum cdce906_clk_src clksrc;
    uint16_t pll_div_m[3];
    uint16_t pll_div_n[3];
    enum cdce906_pll_mux pll_mux[3];
    enum cdce906_pll_fvco pll_fvco[3];
    enum cdce906_s0_cfg s0_cfg;
    enum cdce906_s1_cfg s1_cfg;
    enum cdce906_px_pll_sel pll_sel[6];
    uint8_t p_div[6];
    enum cdce906_yx_slew_cfg y_slew[6];
    enum cdce906_yx_px_sel y_p_sel[6];
    enum cdce906_yx_out_cfg y_out[6];
} cdce906_cfg;

/**
 * @brief Read the current CDCE906 configuration
 *
 * @param[in]  chip_id Chip ID to communicate
 * @param[out] cfg     cdce906_cfg struct with the current configuration
 *
 * @return 0 if successful, non zero if there was an I2C error
 */
int cdce906_read_cfg(uint8_t chip_id, cdce906_cfg* cfg);

/**
 * @brief Write the CDCE906 configuration
 *
 * @param[in]  chip_id Chip ID to communicate
 * @param[in]  cfg     cdce906_cfg struct containing the desired configuration
 *
 * @return 0 if successful, non zero if there was an I2C error
 */
int cdce906_write_cfg(uint8_t chip_id, const cdce906_cfg* cfg);

#endif
