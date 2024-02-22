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
 * @file   cdce906_config.h
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  CDCE906 PLL configuration
 *
 * @ingroup RTM-LAMP
 */

#include "cdce906.h"

static const cdce906_cfg cdce906_rtm_cfg =
{
    .clksrc = CDCE906_CLK_SRC_CLKIN_DIFF,
    .pll_div_m = {2, 1, 1},
    .pll_div_n = {2, 1, 1},
    .pll_vco_mux = {
        CDCE906_PLL_VCO_MUX_PLL,
        CDCE906_PLL_VCO_MUX_PLL,
        CDCE906_PLL_VCO_MUX_PLL,
    },
    .pll_fvco = {
        CDCE906_PLL_FVCO_80_200MHZ,
        CDCE906_PLL_FVCO_80_200MHZ,
        CDCE906_PLL_FVCO_80_200MHZ,
    },
    .ssc_mod_amount = CDCE906_SSC_MOD_AMOUNT_OFF,
    .ssc_mod_freq = CDCE906_SSC_MOD_FREQ_3286,
    .s0_cfg = CDCE906_S0_CFG_POWER_DOWN_CTRL,
    .s1_cfg = CDCE906_S1_CFG_Yx_FIXED_OUTPUT_CTRL,
    .pll_sel = {
        CDCE906_Px_PLL_SEL_PLL1,
        CDCE906_Px_PLL_SEL_BYPASS,
        CDCE906_Px_PLL_SEL_BYPASS,
        CDCE906_Px_PLL_SEL_BYPASS,
        CDCE906_Px_PLL_SEL_BYPASS,
        CDCE906_Px_PLL_SEL_BYPASS,
    },
    .p_div = {1, 120, 30, 60, 1, 1},
    .y_slew = {
        CDCE906_Yx_SLEW_CFG_NOMINAL_1NS,
        CDCE906_Yx_SLEW_CFG_NOMINAL_1NS,
        CDCE906_Yx_SLEW_CFG_NOMINAL_1NS,
        CDCE906_Yx_SLEW_CFG_NOMINAL_1NS,
        CDCE906_Yx_SLEW_CFG_NOMINAL_1NS,
        CDCE906_Yx_SLEW_CFG_NOMINAL_1NS,
    },
    .y_p_sel = {
        CDCE906_Yx_Px_SEL_P0,
        CDCE906_Yx_Px_SEL_P1,
        CDCE906_Yx_Px_SEL_P2,
        CDCE906_Yx_Px_SEL_P3,
        CDCE906_Yx_Px_SEL_P4,
        CDCE906_Yx_Px_SEL_P5,
    },
    .y_out = {
        CDCE906_Yx_OUT_CFG_EN,
        CDCE906_Yx_OUT_CFG_DIS_HIGH,
        CDCE906_Yx_OUT_CFG_DIS_LOW,
        CDCE906_Yx_OUT_CFG_DIS_LOW,
        CDCE906_Yx_OUT_CFG_DIS_LOW,
        CDCE906_Yx_OUT_CFG_DIS_LOW,
    },
};
