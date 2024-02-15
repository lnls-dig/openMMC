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
 * @file   cdce906.c
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  CDCE906 PLL driver functions
 *
 * @ingroup CDCE906
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"

/* Project Includes */
#include "port.h"
#include "cdce906.h"
#include "cdce906_reg.h"
#include "i2c.h"

static inline void cdce906_get_ssc(cdce906_cfg* cfg, const uint8_t* buff)
{
    uint8_t reg;

    reg = (buff[CDCE906_SSC_MOD_SEL_BYTE] & CDCE906_SSC_MOD_SEL_MASK) >>
        CDCE906_SSC_MOD_SEL_SHIFT;
    cfg->ssc_mod_amount = (enum cdce906_ssc_mod_amount)reg;

    reg = (buff[CDCE906_SSC_FREQ_SEL_BYTE] & CDCE906_SSC_FREQ_SEL_MASK) >>
        CDCE906_SSC_FREQ_SEL_SHIFT;
    cfg->ssc_mod_freq = (enum cdce906_ssc_mod_freq)reg;
}

static inline void cdce906_set_ssc(const cdce906_cfg* cfg, uint8_t* buff)
{
    uint8_t reg;

    reg = (uint8_t)cfg->ssc_mod_amount << CDCE906_SSC_MOD_SEL_SHIFT;
    buff[CDCE906_SSC_MOD_SEL_BYTE] |= reg;

    reg = (uint8_t)cfg->ssc_mod_freq << CDCE906_SSC_FREQ_SEL_SHIFT;
    buff[CDCE906_SSC_FREQ_SEL_BYTE] |= reg;
}

static inline void cdce906_get_pll_fvco(cdce906_cfg* cfg, const uint8_t* buff)
{
    if (buff[CDCE906_PLL1_FVCO_BYTE] & CDCE906_PLL1_FVCO_MASK) {
        cfg->pll_fvco[0] = CDCE906_PLL_FVCO_180_300MHZ;
    } else {
        cfg->pll_fvco[0] = CDCE906_PLL_FVCO_80_200MHZ;
    }

    if (buff[CDCE906_PLL2_FVCO_BYTE] & CDCE906_PLL2_FVCO_MASK) {
        cfg->pll_fvco[1] = CDCE906_PLL_FVCO_180_300MHZ;
    } else {
        cfg->pll_fvco[1] = CDCE906_PLL_FVCO_80_200MHZ;
    }

    if (buff[CDCE906_PLL3_FVCO_BYTE] & CDCE906_PLL3_FVCO_MASK) {
        cfg->pll_fvco[2] = CDCE906_PLL_FVCO_180_300MHZ;
    } else {
        cfg->pll_fvco[2] = CDCE906_PLL_FVCO_80_200MHZ;
    }
}

static inline void cdce906_set_pll_fvco(const cdce906_cfg* cfg, uint8_t* buff)
{
    if (cfg->pll_fvco[0] == CDCE906_PLL_FVCO_180_300MHZ) {
        buff[CDCE906_PLL1_FVCO_BYTE] |= CDCE906_PLL1_FVCO_MASK;
    }

    if (cfg->pll_fvco[1] == CDCE906_PLL_FVCO_180_300MHZ) {
        buff[CDCE906_PLL2_FVCO_BYTE] |= CDCE906_PLL2_FVCO_MASK;
    }

    if (cfg->pll_fvco[2] == CDCE906_PLL_FVCO_180_300MHZ) {
        buff[CDCE906_PLL3_FVCO_BYTE] |= CDCE906_PLL3_FVCO_MASK;
    }
}

static inline void cdce906_get_pll_mux(cdce906_cfg* cfg, const uint8_t* buff)
{
    if (buff[CDCE906_PLL1_VCO_MUX_BYTE] & CDCE906_PLL1_VCO_MUX_MASK) {
        cfg->pll_vco_mux[0] = CDCE906_PLL_VCO_MUX_VCO;
    } else {
        cfg->pll_vco_mux[0] = CDCE906_PLL_VCO_MUX_PLL;
    }

    if (buff[CDCE906_PLL2_VCO_MUX_BYTE] & CDCE906_PLL2_VCO_MUX_MASK) {
        cfg->pll_vco_mux[1] = CDCE906_PLL_VCO_MUX_VCO;
    } else {
        cfg->pll_vco_mux[1] = CDCE906_PLL_VCO_MUX_PLL;
    }

    if (buff[CDCE906_PLL3_VCO_MUX_BYTE] & CDCE906_PLL3_VCO_MUX_MASK) {
        cfg->pll_vco_mux[2] = CDCE906_PLL_VCO_MUX_VCO;
    } else {
        cfg->pll_vco_mux[2] = CDCE906_PLL_VCO_MUX_PLL;
    }
}

static inline void cdce906_set_pll_mux(const cdce906_cfg* cfg, uint8_t* buff)
{
    if (cfg->pll_vco_mux[0] == CDCE906_PLL_VCO_MUX_VCO) {
        buff[CDCE906_PLL1_VCO_MUX_BYTE] |= CDCE906_PLL1_VCO_MUX_MASK;
    }

    if (cfg->pll_vco_mux[1] == CDCE906_PLL_VCO_MUX_VCO) {
        buff[CDCE906_PLL2_VCO_MUX_BYTE] |= CDCE906_PLL2_VCO_MUX_MASK;
    }

    if (cfg->pll_vco_mux[2] == CDCE906_PLL_VCO_MUX_VCO) {
        buff[CDCE906_PLL3_VCO_MUX_BYTE] |= CDCE906_PLL3_VCO_MUX_MASK;
    }
}

static inline void cdce906_get_clksrc(cdce906_cfg* cfg, const uint8_t* buff)
{
    switch(buff[CDCE906_CLKIN_SRC_BYTE] & CDCE906_CLKIN_SRC_MASK)
    {
    case 0:
        cfg->clksrc = CDCE906_CLK_SRC_CRYSTAL;
        break;

    case 1 << CDCE906_CLKIN_SRC_SHIFT:
        if (buff[CDCE906_CLKSEL_BYTE] & CDCE906_CLKSEL_MASK) {
            cfg->clksrc = CDCE906_CLK_SRC_CLKIN1_LVCMOS;
        } else {
            cfg->clksrc = CDCE906_CLK_SRC_CLKIN0_LVCMOS;
        }
        break;

    case 2 << CDCE906_CLKIN_SRC_SHIFT:
        cfg->clksrc = CDCE906_CLK_SRC_CLKIN_DIFF;
        break;

    default:
        cfg->clksrc = CDCE906_CLK_SRC_INVALID;
        break;
    }
}

static inline void cdce906_set_clksrc(const cdce906_cfg* cfg, uint8_t* buff)
{
    switch(cfg->clksrc)
    {
    case CDCE906_CLK_SRC_CRYSTAL:
        break;

    case CDCE906_CLK_SRC_CLKIN0_LVCMOS:
        buff[CDCE906_CLKIN_SRC_BYTE] |= 1 << CDCE906_CLKIN_SRC_SHIFT;
        break;

    case CDCE906_CLK_SRC_CLKIN1_LVCMOS:
        buff[CDCE906_CLKIN_SRC_BYTE] |= 1 << CDCE906_CLKIN_SRC_SHIFT;
        buff[CDCE906_CLKSEL_BYTE] |= CDCE906_CLKSEL_MASK;
        break;

    case CDCE906_CLK_SRC_CLKIN_DIFF:
        buff[CDCE906_CLKIN_SRC_BYTE] |= 2 << CDCE906_CLKIN_SRC_SHIFT;
        break;

    default:
        break;
    }
}

static inline void cdce906_get_px_pll_sel(cdce906_cfg* cfg, const uint8_t* buff)
{
    uint8_t px_sel_val[6];

    px_sel_val[0] = (buff[CDCE906_P0_SW_A_BYTE] & CDCE906_P0_SW_A_MASK) >> CDCE906_P0_SW_A_SHIFT;
    px_sel_val[1] = (buff[CDCE906_P1_SW_A_BYTE] & CDCE906_P1_SW_A_MASK) >> CDCE906_P1_SW_A_SHIFT;
    px_sel_val[2] = (buff[CDCE906_P2_SW_A_BYTE] & CDCE906_P2_SW_A_MASK) >> CDCE906_P2_SW_A_SHIFT;
    px_sel_val[3] = (buff[CDCE906_P3_SW_A_BYTE] & CDCE906_P3_SW_A_MASK) >> CDCE906_P3_SW_A_SHIFT;
    px_sel_val[4] = (buff[CDCE906_P4_SW_A_BYTE] & CDCE906_P4_SW_A_MASK) >> CDCE906_P4_SW_A_SHIFT;
    px_sel_val[5] = (buff[CDCE906_P5_SW_A_BYTE] & CDCE906_P5_SW_A_MASK) >> CDCE906_P5_SW_A_SHIFT;

    for (int i = 0; i < 6; i++) {
        switch(px_sel_val[i]) {
        case 0:
            cfg->pll_sel[i] = CDCE906_Px_PLL_SEL_BYPASS;
            break;

        case 1:
            cfg->pll_sel[i] = CDCE906_Px_PLL_SEL_PLL1;
            break;

        case 2:
            cfg->pll_sel[i] = CDCE906_Px_PLL_SEL_PLL2;
            break;

        case 3:
            cfg->pll_sel[i] = CDCE906_Px_PLL_SEL_PLL2_SSC;
            break;

        case 4:
            cfg->pll_sel[i] = CDCE906_Px_PLL_SEL_PLL3;
            break;

        default:
            cfg->pll_sel[i] = CDCE906_Px_PLL_SEL_INVALID;
            break;
        }
    }
}

static inline void cdce906_set_px_pll_sel(const cdce906_cfg* cfg, uint8_t* buff)
{
    const uint8_t px_byte[] = {CDCE906_P0_SW_A_BYTE, CDCE906_P1_SW_A_BYTE, CDCE906_P2_SW_A_BYTE,
        CDCE906_P3_SW_A_BYTE, CDCE906_P4_SW_A_BYTE, CDCE906_P5_SW_A_BYTE};
    const uint8_t px_shift[] = {CDCE906_P0_SW_A_SHIFT, CDCE906_P1_SW_A_SHIFT, CDCE906_P2_SW_A_SHIFT,
        CDCE906_P3_SW_A_SHIFT, CDCE906_P4_SW_A_SHIFT, CDCE906_P5_SW_A_SHIFT};

    for (int i = 0; i < 6; i++) {
        switch(cfg->pll_sel[i]) {
        case CDCE906_Px_PLL_SEL_BYPASS:
            break;

        case CDCE906_Px_PLL_SEL_PLL1:
            buff[px_byte[i]] |= 1 << px_shift[i];
            break;

        case CDCE906_Px_PLL_SEL_PLL2:
            buff[px_byte[i]] |= 2 << px_shift[i];
            break;

        case CDCE906_Px_PLL_SEL_PLL2_SSC:
            buff[px_byte[i]] |= 3 << px_shift[i];
            break;

        case CDCE906_Px_PLL_SEL_PLL3:
            buff[px_byte[i]] |= 4 << px_shift[i];
            break;

        default:
            break;
        }
    }
}

static inline void cdce906_set_pll_div(const cdce906_cfg* cfg, uint8_t* buff)
{
    const uint8_t div_m_low_byte[] = {CDCE906_PLL1_REF_DIV_M_LOW_BYTE, CDCE906_PLL2_REF_DIV_M_LOW_BYTE,
        CDCE906_PLL3_REF_DIV_M_LOW_BYTE};
    const uint8_t div_m_high_byte[] = {CDCE906_PLL1_REF_DIV_M_HIGH_BYTE, CDCE906_PLL2_REF_DIV_M_HIGH_BYTE,
        CDCE906_PLL3_REF_DIV_M_HIGH_BYTE};
    const uint8_t div_m_low_shift[] = {CDCE906_PLL1_REF_DIV_M_LOW_SHIFT, CDCE906_PLL2_REF_DIV_M_LOW_SHIFT,
        CDCE906_PLL3_REF_DIV_M_LOW_SHIFT};
    const uint8_t div_m_high_shift[] = {CDCE906_PLL1_REF_DIV_M_HIGH_SHIFT, CDCE906_PLL2_REF_DIV_M_HIGH_SHIFT,
        CDCE906_PLL3_REF_DIV_M_HIGH_SHIFT};
    const uint8_t div_m_low_mask[] = {CDCE906_PLL1_REF_DIV_M_LOW_MASK, CDCE906_PLL2_REF_DIV_M_LOW_MASK,
        CDCE906_PLL3_REF_DIV_M_LOW_MASK};
    const uint8_t div_m_high_mask[] = {CDCE906_PLL1_REF_DIV_M_HIGH_MASK, CDCE906_PLL2_REF_DIV_M_HIGH_MASK,
        CDCE906_PLL3_REF_DIV_M_HIGH_MASK};
    const uint8_t div_n_low_byte[] = {CDCE906_PLL1_REF_DIV_N_LOW_BYTE, CDCE906_PLL2_REF_DIV_N_LOW_BYTE,
        CDCE906_PLL3_REF_DIV_N_LOW_BYTE};
    const uint8_t div_n_high_byte[] = {CDCE906_PLL1_REF_DIV_N_HIGH_BYTE, CDCE906_PLL2_REF_DIV_N_HIGH_BYTE,
        CDCE906_PLL3_REF_DIV_N_HIGH_BYTE};
    const uint8_t div_n_low_shift[] = {CDCE906_PLL1_REF_DIV_N_LOW_SHIFT, CDCE906_PLL2_REF_DIV_N_LOW_SHIFT,
        CDCE906_PLL3_REF_DIV_N_LOW_SHIFT};
    const uint8_t div_n_high_shift[] = {CDCE906_PLL1_REF_DIV_N_HIGH_SHIFT, CDCE906_PLL2_REF_DIV_N_HIGH_SHIFT,
        CDCE906_PLL3_REF_DIV_N_HIGH_SHIFT};
    const uint8_t div_n_low_mask[] = {CDCE906_PLL1_REF_DIV_N_LOW_MASK, CDCE906_PLL2_REF_DIV_N_LOW_MASK,
        CDCE906_PLL3_REF_DIV_N_LOW_MASK};
    const uint8_t div_n_high_mask[] = {CDCE906_PLL1_REF_DIV_N_HIGH_MASK, CDCE906_PLL2_REF_DIV_N_HIGH_MASK,
        CDCE906_PLL3_REF_DIV_N_HIGH_MASK};

    for (int i = 0; i < 3; i++) {
        uint8_t div_m_low = cfg->pll_div_m[i] & 0xFF;
        uint8_t div_n_low = cfg->pll_div_n[i] & 0xFF;
        uint8_t div_m_high = (cfg->pll_div_m[i] & 0x100) >> 8;
        uint8_t div_n_high = (cfg->pll_div_n[i] & 0xF00) >> 8;
        buff[div_m_low_byte[i]] |= (div_m_low << div_m_low_shift[i]) & div_m_low_mask[i];
        buff[div_m_high_byte[i]] |= (div_m_high << div_m_high_shift[i]) & div_m_high_mask[i];
        buff[div_n_low_byte[i]] |= (div_n_low << div_n_low_shift[i]) & div_n_low_mask[i];
        buff[div_n_high_byte[i]] |= (div_n_high << div_n_high_shift[i]) & div_n_high_mask[i];
    }
}

static inline void cdce906_get_pll_div(cdce906_cfg* cfg, const uint8_t* buff)
{
    const uint8_t div_m_low_byte[] = {CDCE906_PLL1_REF_DIV_M_LOW_BYTE, CDCE906_PLL2_REF_DIV_M_LOW_BYTE,
        CDCE906_PLL3_REF_DIV_M_LOW_BYTE};
    const uint8_t div_m_high_byte[] = {CDCE906_PLL1_REF_DIV_M_HIGH_BYTE, CDCE906_PLL2_REF_DIV_M_HIGH_BYTE,
        CDCE906_PLL3_REF_DIV_M_HIGH_BYTE};
    const uint8_t div_m_high_shift[] = {CDCE906_PLL1_REF_DIV_M_HIGH_SHIFT, CDCE906_PLL2_REF_DIV_M_HIGH_SHIFT,
        CDCE906_PLL3_REF_DIV_M_HIGH_SHIFT};
    const uint8_t div_m_low_mask[] = {CDCE906_PLL1_REF_DIV_M_LOW_MASK, CDCE906_PLL2_REF_DIV_M_LOW_MASK,
        CDCE906_PLL3_REF_DIV_M_LOW_MASK};
    const uint8_t div_m_high_mask[] = {CDCE906_PLL1_REF_DIV_M_HIGH_MASK, CDCE906_PLL2_REF_DIV_M_HIGH_MASK,
        CDCE906_PLL3_REF_DIV_M_HIGH_MASK};
    const uint8_t div_n_low_byte[] = {CDCE906_PLL1_REF_DIV_N_LOW_BYTE, CDCE906_PLL2_REF_DIV_N_LOW_BYTE,
        CDCE906_PLL3_REF_DIV_N_LOW_BYTE};
    const uint8_t div_n_high_byte[] = {CDCE906_PLL1_REF_DIV_N_HIGH_BYTE, CDCE906_PLL2_REF_DIV_N_HIGH_BYTE,
        CDCE906_PLL3_REF_DIV_N_HIGH_BYTE};
    const uint8_t div_n_high_shift[] = {CDCE906_PLL1_REF_DIV_N_HIGH_SHIFT, CDCE906_PLL2_REF_DIV_N_HIGH_SHIFT,
        CDCE906_PLL3_REF_DIV_N_HIGH_SHIFT};
    const uint8_t div_n_low_mask[] = {CDCE906_PLL1_REF_DIV_N_LOW_MASK, CDCE906_PLL2_REF_DIV_N_LOW_MASK,
        CDCE906_PLL3_REF_DIV_N_LOW_MASK};
    const uint8_t div_n_high_mask[] = {CDCE906_PLL1_REF_DIV_N_HIGH_MASK, CDCE906_PLL2_REF_DIV_N_HIGH_MASK,
        CDCE906_PLL3_REF_DIV_N_HIGH_MASK};

    for (int i = 0; i < 3; i++) {
        cfg->pll_div_m[i] = (buff[div_m_low_byte[i]] & div_m_low_mask[i]) |
            ((buff[div_m_high_byte[i]] & div_m_high_mask[i])  << (8 - div_m_high_shift[i]));
        cfg->pll_div_n[i] = (buff[div_n_low_byte[i]] & div_n_low_mask[i]) |
            ((buff[div_n_high_byte[i]] & div_n_high_mask[i]) << (8 - div_n_high_shift[i]));
    }
}

static inline void cdce906_set_yx_px_sel(const cdce906_cfg* cfg, uint8_t* buff)
{
    const uint8_t yx_byte[] = {CDCE906_Y0_SW_B_BYTE, CDCE906_Y1_SW_B_BYTE, CDCE906_Y2_SW_B_BYTE,
        CDCE906_Y3_SW_B_BYTE, CDCE906_Y4_SW_B_BYTE, CDCE906_Y5_SW_B_BYTE};
    const uint8_t yx_shift[] = {CDCE906_Y0_SW_B_SHIFT, CDCE906_Y1_SW_B_SHIFT, CDCE906_Y2_SW_B_SHIFT,
        CDCE906_Y3_SW_B_SHIFT, CDCE906_Y4_SW_B_SHIFT, CDCE906_Y5_SW_B_SHIFT};

    for (int i = 0; i < 6; i++) {
        switch(cfg->y_p_sel[i]) {
        case CDCE906_Yx_Px_SEL_P0:
            break;

        case CDCE906_Yx_Px_SEL_P1:
            buff[yx_byte[i]] |= 1 << yx_shift[i];
            break;

        case CDCE906_Yx_Px_SEL_P2:
            buff[yx_byte[i]] |= 2 << yx_shift[i];
            break;

        case CDCE906_Yx_Px_SEL_P3:
            buff[yx_byte[i]] |= 3 << yx_shift[i];
            break;

        case CDCE906_Yx_Px_SEL_P4:
            buff[yx_byte[i]] |= 4 << yx_shift[i];
            break;

        case CDCE906_Yx_Px_SEL_P5:
            buff[yx_byte[i]] |= 5 << yx_shift[i];
            break;

        default:
            break;
        }
    }
}

static inline void cdce906_get_yx_px_sel(cdce906_cfg* cfg, const uint8_t* buff)
{
    uint8_t yx_sel_val[6];

    yx_sel_val[0] = (buff[CDCE906_Y0_SW_B_BYTE] & CDCE906_Y0_SW_B_MASK) >> CDCE906_Y0_SW_B_SHIFT;
    yx_sel_val[1] = (buff[CDCE906_Y1_SW_B_BYTE] & CDCE906_Y1_SW_B_MASK) >> CDCE906_Y1_SW_B_SHIFT;
    yx_sel_val[2] = (buff[CDCE906_Y2_SW_B_BYTE] & CDCE906_Y2_SW_B_MASK) >> CDCE906_Y2_SW_B_SHIFT;
    yx_sel_val[3] = (buff[CDCE906_Y3_SW_B_BYTE] & CDCE906_Y3_SW_B_MASK) >> CDCE906_Y3_SW_B_SHIFT;
    yx_sel_val[4] = (buff[CDCE906_Y4_SW_B_BYTE] & CDCE906_Y4_SW_B_MASK) >> CDCE906_Y4_SW_B_SHIFT;
    yx_sel_val[5] = (buff[CDCE906_Y5_SW_B_BYTE] & CDCE906_Y5_SW_B_MASK) >> CDCE906_Y5_SW_B_SHIFT;

    for (int i = 0; i < 6; i++) {
        switch(yx_sel_val[i]) {
        case 0:
            cfg->y_p_sel[i] = CDCE906_Yx_Px_SEL_P0;
            break;

        case 1:
            cfg->y_p_sel[i] = CDCE906_Yx_Px_SEL_P1;
            break;

        case 2:
            cfg->y_p_sel[i] = CDCE906_Yx_Px_SEL_P2;
            break;

        case 3:
            cfg->y_p_sel[i] = CDCE906_Yx_Px_SEL_P3;
            break;

        case 4:
            cfg->y_p_sel[i] = CDCE906_Yx_Px_SEL_P4;
            break;

        case 5:
            cfg->y_p_sel[i] = CDCE906_Yx_Px_SEL_P5;
            break;

        default:
            cfg->y_p_sel[i] = CDCE906_Yx_Px_SEL_INVALID;
            break;
        }
    }
}

static inline void cdce906_set_yx_slew(const cdce906_cfg* cfg, uint8_t* buff)
{
    const uint8_t yx_byte[] = {CDCE906_Y0_SLEW_RATE_BYTE, CDCE906_Y1_SLEW_RATE_BYTE,
        CDCE906_Y2_SLEW_RATE_BYTE, CDCE906_Y3_SLEW_RATE_BYTE,
        CDCE906_Y4_SLEW_RATE_BYTE, CDCE906_Y5_SLEW_RATE_BYTE};
    const uint8_t yx_shift[] = {CDCE906_Y0_SLEW_RATE_SHIFT, CDCE906_Y1_SLEW_RATE_SHIFT,
        CDCE906_Y2_SLEW_RATE_SHIFT, CDCE906_Y3_SLEW_RATE_SHIFT,
        CDCE906_Y4_SLEW_RATE_SHIFT, CDCE906_Y5_SLEW_RATE_SHIFT};

    for (int i = 0; i < 6; i++) {
        switch(cfg->y_slew[i]) {
        case CDCE906_Yx_SLEW_CFG_NOMINAL:
            buff[yx_byte[i]] |= 3 << yx_shift[i];
            break;

        case CDCE906_Yx_SLEW_CFG_NOMINAL_1NS:
            buff[yx_byte[i]] |= 2 << yx_shift[i];
            break;

        case CDCE906_Yx_SLEW_CFG_NOMINAL_2NS:
            buff[yx_byte[i]] |= 1 << yx_shift[i];
            break;

        case CDCE906_Yx_SLEW_CFG_NOMINAL_3NS:
            break;

        default:
            break;
        }
    }
}

static inline void cdce906_get_yx_slew(cdce906_cfg* cfg, const uint8_t* buff)
{
    uint8_t yx_slew[6];

    yx_slew[0] = (buff[CDCE906_Y0_SLEW_RATE_BYTE] & CDCE906_Y0_SLEW_RATE_MASK) >> CDCE906_Y0_SLEW_RATE_SHIFT;
    yx_slew[1] = (buff[CDCE906_Y1_SLEW_RATE_BYTE] & CDCE906_Y1_SLEW_RATE_MASK) >> CDCE906_Y1_SLEW_RATE_SHIFT;
    yx_slew[2] = (buff[CDCE906_Y2_SLEW_RATE_BYTE] & CDCE906_Y2_SLEW_RATE_MASK) >> CDCE906_Y2_SLEW_RATE_SHIFT;
    yx_slew[3] = (buff[CDCE906_Y3_SLEW_RATE_BYTE] & CDCE906_Y3_SLEW_RATE_MASK) >> CDCE906_Y3_SLEW_RATE_SHIFT;
    yx_slew[4] = (buff[CDCE906_Y4_SLEW_RATE_BYTE] & CDCE906_Y4_SLEW_RATE_MASK) >> CDCE906_Y4_SLEW_RATE_SHIFT;
    yx_slew[5] = (buff[CDCE906_Y5_SLEW_RATE_BYTE] & CDCE906_Y5_SLEW_RATE_MASK) >> CDCE906_Y5_SLEW_RATE_SHIFT;

    for (int i = 0; i < 6; i++) {
        switch(yx_slew[i]) {
        case 0:
            cfg->y_slew[i] = CDCE906_Yx_SLEW_CFG_NOMINAL_3NS;
            break;

        case 1:
            cfg->y_slew[i] = CDCE906_Yx_SLEW_CFG_NOMINAL_2NS;
            break;

        case 2:
            cfg->y_slew[i] = CDCE906_Yx_SLEW_CFG_NOMINAL_1NS;
            break;

        case 3:
            cfg->y_slew[i] = CDCE906_Yx_SLEW_CFG_NOMINAL;
            break;

        default:
            break;
        }
    }
}

static inline void cdce906_set_px_div(const cdce906_cfg* cfg, uint8_t* buff)
{
    const uint8_t px_byte[] = {CDCE906_P0_DIV_BYTE, CDCE906_P1_DIV_BYTE, CDCE906_P2_DIV_BYTE,
        CDCE906_P3_DIV_BYTE, CDCE906_P4_DIV_BYTE, CDCE906_P5_DIV_BYTE};
    const uint8_t px_shift[] = {CDCE906_P0_DIV_SHIFT, CDCE906_P1_DIV_SHIFT, CDCE906_P2_DIV_SHIFT,
        CDCE906_P3_DIV_SHIFT, CDCE906_P4_DIV_SHIFT, CDCE906_P5_DIV_SHIFT};
    const uint8_t px_mask[] = {CDCE906_P0_DIV_MASK, CDCE906_P1_DIV_MASK, CDCE906_P2_DIV_MASK,
        CDCE906_P3_DIV_MASK, CDCE906_P4_DIV_MASK, CDCE906_P5_DIV_MASK};

    for (int i = 0; i < 6; i++) {
        buff[px_byte[i]] |= (cfg->p_div[i] << px_shift[i]) & px_mask[i];
    }
}

static inline void cdce906_get_px_div(cdce906_cfg* cfg, const uint8_t* buff)
{
    const uint8_t px_byte[] = {CDCE906_P0_DIV_BYTE, CDCE906_P1_DIV_BYTE, CDCE906_P2_DIV_BYTE,
        CDCE906_P3_DIV_BYTE, CDCE906_P4_DIV_BYTE, CDCE906_P5_DIV_BYTE};
    const uint8_t px_shift[] = {CDCE906_P0_DIV_SHIFT, CDCE906_P1_DIV_SHIFT, CDCE906_P2_DIV_SHIFT,
        CDCE906_P3_DIV_SHIFT, CDCE906_P4_DIV_SHIFT, CDCE906_P5_DIV_SHIFT};
    const uint8_t px_mask[] = {CDCE906_P0_DIV_MASK, CDCE906_P1_DIV_MASK, CDCE906_P2_DIV_MASK,
        CDCE906_P3_DIV_MASK, CDCE906_P4_DIV_MASK, CDCE906_P5_DIV_MASK};

    for (int i = 0; i < 6; i++) {
        cfg->p_div[i] = (buff[px_byte[i]] & px_mask[i]) >> px_shift[i];
    }
}

static inline void cdce906_set_yx_out_cfg(const cdce906_cfg* cfg, uint8_t* buff)
{
    const uint8_t yx_en_byte[] = {CDCE906_Y0_EN_BYTE, CDCE906_Y1_EN_BYTE, CDCE906_Y2_EN_BYTE,
        CDCE906_Y3_EN_BYTE, CDCE906_Y4_EN_BYTE, CDCE906_Y5_EN_BYTE};
    const uint8_t yx_en_shift[] = {CDCE906_Y0_EN_SHIFT, CDCE906_Y1_EN_SHIFT, CDCE906_Y2_EN_SHIFT,
        CDCE906_Y3_EN_SHIFT, CDCE906_Y4_EN_SHIFT, CDCE906_Y5_EN_SHIFT};
    const uint8_t yx_pol_byte[] = {CDCE906_Y0_POL_BYTE, CDCE906_Y1_POL_BYTE, CDCE906_Y2_POL_BYTE,
        CDCE906_Y3_POL_BYTE, CDCE906_Y4_POL_BYTE, CDCE906_Y5_POL_BYTE};
    const uint8_t yx_pol_shift[] = {CDCE906_Y0_POL_SHIFT, CDCE906_Y1_POL_SHIFT, CDCE906_Y2_POL_SHIFT,
        CDCE906_Y3_POL_SHIFT, CDCE906_Y4_POL_SHIFT, CDCE906_Y5_POL_SHIFT};

    for (int i = 0; i < 6; i++) {
        switch(cfg->y_out[i]) {
        case CDCE906_Yx_OUT_CFG_EN:
            buff[yx_en_byte[i]] |= 1 << yx_en_shift[i];
            break;

        case CDCE906_Yx_OUT_CFG_EN_INV:
            buff[yx_en_byte[i]] |= 1 << yx_en_shift[i];
            buff[yx_pol_byte[i]] |= 1 << yx_pol_shift[i];
            break;

        case CDCE906_Yx_OUT_CFG_DIS_LOW:
            break;

        case CDCE906_Yx_OUT_CFG_DIS_HIGH:
            buff[yx_pol_byte[i]] |= 1 << yx_pol_shift[i];
            break;
        }
    }
}

static inline void cdce906_get_yx_out_cfg(cdce906_cfg* cfg, const uint8_t* buff)
{
    const uint8_t yx_en_byte[] = {CDCE906_Y0_EN_BYTE, CDCE906_Y1_EN_BYTE, CDCE906_Y2_EN_BYTE,
        CDCE906_Y3_EN_BYTE, CDCE906_Y4_EN_BYTE, CDCE906_Y5_EN_BYTE};
    const uint8_t yx_en_mask[] = {CDCE906_Y0_EN_MASK, CDCE906_Y1_EN_MASK, CDCE906_Y2_EN_MASK,
        CDCE906_Y3_EN_MASK, CDCE906_Y4_EN_MASK, CDCE906_Y5_EN_MASK};
    const uint8_t yx_pol_byte[] = {CDCE906_Y0_POL_BYTE, CDCE906_Y1_POL_BYTE, CDCE906_Y2_POL_BYTE,
        CDCE906_Y3_POL_BYTE, CDCE906_Y4_POL_BYTE, CDCE906_Y5_POL_BYTE};
    const uint8_t yx_pol_mask[] = {CDCE906_Y0_POL_MASK, CDCE906_Y1_POL_MASK, CDCE906_Y2_POL_MASK,
        CDCE906_Y3_POL_MASK, CDCE906_Y4_POL_MASK, CDCE906_Y5_POL_MASK};

    for (int i = 0; i < 6; i++) {
        if ((buff[yx_en_byte[i]] & yx_en_mask[i]) && !(buff[yx_pol_byte[i]] & yx_pol_mask[i])) {
            cfg->y_out[i] = CDCE906_Yx_OUT_CFG_EN;
        } else if ((buff[yx_en_byte[i]] & yx_en_mask[i]) && (buff[yx_pol_byte[i]] & yx_pol_mask[i])) {
            cfg->y_out[i] = CDCE906_Yx_OUT_CFG_EN_INV;
        } else if (!(buff[yx_en_byte[i]] & yx_en_mask[i]) && !(buff[yx_pol_byte[i]] & yx_pol_mask[i])) {
            cfg->y_out[i] = CDCE906_Yx_OUT_CFG_DIS_LOW;
        } else if (!(buff[yx_en_byte[i]] & yx_en_mask[i]) && (buff[yx_pol_byte[i]] & yx_pol_mask[i])) {
            cfg->y_out[i] = CDCE906_Yx_OUT_CFG_DIS_HIGH;
        }
    }
}

int cdce906_read_cfg(uint8_t chip_id, cdce906_cfg* cfg)
{
    uint8_t data[27];
    uint8_t i2c_addr;
    uint8_t i2c_id;
    int i2c_trans = 0;
    int ret = 0;
    const uint8_t cmd = 0x00;

    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, (TickType_t)10)) {
        i2c_trans = xI2CMasterWriteRead(i2c_id, i2c_addr, &cmd, 1, data, sizeof(data));
        i2c_give(i2c_id);
    }

    if (i2c_trans == sizeof(data)) {
        /*
         * Ignore the first byte from the data buffer as it is the
         * number of bytes read, not the register 0
         */
        cdce906_get_clksrc(cfg, &data[1]);
        cdce906_get_px_pll_sel(cfg, &data[1]);
        cdce906_get_pll_div(cfg, &data[1]);
        cdce906_get_yx_px_sel(cfg, &data[1]);
        cdce906_get_yx_slew(cfg, &data[1]);
        cdce906_get_px_div(cfg, &data[1]);
        cdce906_get_yx_out_cfg(cfg, &data[1]);
        cdce906_get_pll_fvco(cfg, &data[1]);
        cdce906_get_pll_mux(cfg, &data[1]);
        cdce906_get_ssc(cfg, &data[1]);
    } else {
        ret = -1;
    }

    return ret;
}

int cdce906_write_cfg(uint8_t chip_id, const cdce906_cfg* cfg)
{
    uint8_t data[28] = {0, 25, 1}; // Write 25 bytes starting from
                                   // address 0
    uint8_t tmp[2];
    uint8_t i2c_addr;
    uint8_t i2c_id;
    int i2c_trans = 0;

    cdce906_set_clksrc(cfg, &data[2]);
    cdce906_set_px_pll_sel(cfg, &data[2]);
    cdce906_set_pll_div(cfg, &data[2]);
    cdce906_set_yx_px_sel(cfg, &data[2]);
    cdce906_set_yx_slew(cfg, &data[2]);
    cdce906_set_px_div(cfg, &data[2]);
    cdce906_set_yx_out_cfg(cfg, &data[2]);
    cdce906_set_pll_fvco(cfg, &data[2]);
    cdce906_set_pll_mux(cfg, &data[2]);
    cdce906_set_ssc(cfg, &data[2]);

    /*
     * Clear EELOCK bit to avoid permanently locking the EEPROM
     */
    data[CDCE906_EELOCK_BYTE + 2] &= 0x7F;

    /*
     * Bypass PLL2 to change the SSC configuration on-the-fly
     */
    tmp[0] = 0x80 | CDCE906_PLL2_VCO_MUX_BYTE;
    tmp[1] = data[CDCE906_PLL2_VCO_MUX_BYTE + 2] | CDCE906_PLL2_VCO_MUX_MASK;
    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, (TickType_t)10)) {
        i2c_trans = xI2CMasterWrite(i2c_id, i2c_addr, tmp, sizeof(tmp));
        i2c_give(i2c_id);
    }

    if (i2c_trans != sizeof(tmp)) {
        return -1;
    }

    /*
     * Write the SSC configuration
     */
    tmp[0] = 0x80 | CDCE906_SSC_FREQ_SEL_BYTE;
    tmp[1] = data[CDCE906_SSC_FREQ_SEL_BYTE + 2];
    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, (TickType_t)10)) {
        i2c_trans = xI2CMasterWrite(i2c_id, i2c_addr, tmp, sizeof(tmp));
        i2c_give(i2c_id);
    }

    if (i2c_trans != sizeof(tmp)) {
        return -1;
    }

    /*
     * Write to all registers except for SSC
     */
    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, (TickType_t)10)) {
        i2c_trans = xI2CMasterWrite(i2c_id, i2c_addr, data, sizeof(data) - 1);
        i2c_give(i2c_id);
    }

    if (i2c_trans != sizeof(data)) {
        return -1;
    }

    return 0;
}

int cdce906_write_eeprom(uint8_t chip_id)
{
    uint8_t data[2];
    uint8_t i2c_addr;
    uint8_t i2c_id;
    int i2c_trans = 0;
    int ret = 0;

    /*
     * Byte write operation, address 26
     */
    data[0] = 26 | 0x80;

    /*
     * Starts an EEPROM write cycle, keep the default block read size
     * (27 bytes)
     */
    data[1] = CDCE906_EEWRITE_MASK | 27;

    if (i2c_take_by_chipid(chip_id, &i2c_addr, &i2c_id, (TickType_t)10)) {
        i2c_trans = xI2CMasterWrite(i2c_id, i2c_addr, data, sizeof(data));
        i2c_give(i2c_id);
    }

    if (i2c_trans != sizeof(data)) {
        ret = -1;
    }

    return ret;
}
