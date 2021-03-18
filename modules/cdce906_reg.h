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
 * @file   cdce906_reg.h
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  CDCE906 PLL register definitions
 *
 * @ingroup CDCE906
 */

#ifndef CDCE906_REG_H_
#define CDCE906_REG_H_

#define CDCE906_PLL1_REF_DIV_M_LOW_BYTE       1
#define CDCE906_PLL1_REF_DIV_M_LOW_SHIFT      0
#define CDCE906_PLL1_REF_DIV_M_LOW_MASK       0xFF
#define CDCE906_PLL1_REF_DIV_M_HIGH_BYTE      3
#define CDCE906_PLL1_REF_DIV_M_HIGH_SHIFT     0
#define CDCE906_PLL1_REF_DIV_M_HIGH_MASK      0x01

#define CDCE906_PLL1_REF_DIV_N_LOW_BYTE       2
#define CDCE906_PLL1_REF_DIV_N_LOW_SHIFT      0
#define CDCE906_PLL1_REF_DIV_N_LOW_MASK       0xFF
#define CDCE906_PLL1_REF_DIV_N_HIGH_BYTE      3
#define CDCE906_PLL1_REF_DIV_N_HIGH_SHIFT     1
#define CDCE906_PLL1_REF_DIV_N_HIGH_MASK      0x1E

#define CDCE906_PLL2_REF_DIV_M_LOW_BYTE       4
#define CDCE906_PLL2_REF_DIV_M_LOW_SHIFT      0
#define CDCE906_PLL2_REF_DIV_M_LOW_MASK       0xFF
#define CDCE906_PLL2_REF_DIV_M_HIGH_BYTE      6
#define CDCE906_PLL2_REF_DIV_M_HIGH_SHIFT     0
#define CDCE906_PLL2_REF_DIV_M_HIGH_MASK      0x01

#define CDCE906_PLL2_REF_DIV_N_LOW_BYTE       5
#define CDCE906_PLL2_REF_DIV_N_LOW_SHIFT      0
#define CDCE906_PLL2_REF_DIV_N_LOW_MASK       0xFF
#define CDCE906_PLL2_REF_DIV_N_HIGH_BYTE      6
#define CDCE906_PLL2_REF_DIV_N_HIGH_SHIFT     1
#define CDCE906_PLL2_REF_DIV_N_HIGH_MASK      0x1E

#define CDCE906_PLL3_REF_DIV_M_LOW_BYTE       7
#define CDCE906_PLL3_REF_DIV_M_LOW_SHIFT      0
#define CDCE906_PLL3_REF_DIV_M_LOW_MASK       0xFF
#define CDCE906_PLL3_REF_DIV_M_HIGH_BYTE      9
#define CDCE906_PLL3_REF_DIV_M_HIGH_SHIFT     0
#define CDCE906_PLL3_REF_DIV_M_HIGH_MASK      0x01

#define CDCE906_PLL3_REF_DIV_N_LOW_BYTE       8
#define CDCE906_PLL3_REF_DIV_N_LOW_SHIFT      0
#define CDCE906_PLL3_REF_DIV_N_LOW_MASK       0xFF
#define CDCE906_PLL3_REF_DIV_N_HIGH_BYTE      9
#define CDCE906_PLL3_REF_DIV_N_HIGH_SHIFT     1
#define CDCE906_PLL3_REF_DIV_N_HIGH_MASK      0x1E

#define CDCE906_PLL1_VCO_MUX_BYTE             3
#define CDCE906_PLL1_VCO_MUX_SHIFT            7
#define CDCE906_PLL1_VCO_MUX_MASK             0x80

#define CDCE906_PLL2_VCO_MUX_BYTE             3
#define CDCE906_PLL2_VCO_MUX_SHIFT            6
#define CDCE906_PLL2_VCO_MUX_MASK             0x40

#define CDCE906_PLL3_VCO_MUX_BYTE             3
#define CDCE906_PLL3_VCO_MUX_SHIFT            5
#define CDCE906_PLL3_VCO_MUX_MASK             0x20

#define CDCE906_PLL1_FVCO_BYTE                6
#define CDCE906_PLL1_FVCO_SHIFT               7
#define CDCE906_PLL1_FVCO_MASK                0x80

#define CDCE906_PLL2_FVCO_BYTE                6
#define CDCE906_PLL2_FVCO_SHIFT               6
#define CDCE906_PLL2_FVCO_MASK                0x40

#define CDCE906_PLL3_FVCO_BYTE                6
#define CDCE906_PLL3_FVCO_SHIFT               5
#define CDCE906_PLL3_FVCO_MASK                0x20

#define CDCE906_P0_SW_A_BYTE                  9
#define CDCE906_P0_SW_A_SHIFT                 5
#define CDCE906_P0_SW_A_MASK                  0xE0

#define CDCE906_P1_SW_A_BYTE                  10
#define CDCE906_P1_SW_A_SHIFT                 5
#define CDCE906_P1_SW_A_MASK                  0xE0

#define CDCE906_P2_SW_A_BYTE                  11
#define CDCE906_P2_SW_A_SHIFT                 0
#define CDCE906_P2_SW_A_MASK                  0x07

#define CDCE906_P3_SW_A_BYTE                  11
#define CDCE906_P3_SW_A_SHIFT                 3
#define CDCE906_P3_SW_A_MASK                  0x38

#define CDCE906_P4_SW_A_BYTE                  12
#define CDCE906_P4_SW_A_SHIFT                 0
#define CDCE906_P4_SW_A_MASK                  0x07

#define CDCE906_P5_SW_A_BYTE                  12
#define CDCE906_P5_SW_A_SHIFT                 3
#define CDCE906_P5_SW_A_MASK                  0x38

#define CDCE906_CLKSEL_BYTE                   10
#define CDCE906_CLKSEL_SHIFT                  4
#define CDCE906_CLKSEL_MASK                   0x10

#define CDCE906_S0_IN_CFG_BYTE                10
#define CDCE906_S0_IN_CFG_SHIFT               0
#define CDCE906_S0_IN_CFG_MASK                0x03

#define CDCE906_S1_IN_CFG_BYTE                10
#define CDCE906_S1_IN_CFG_SHIFT               2
#define CDCE906_S1_IN_CFG_MASK                0x0C

#define CDCE906_CLKIN_SRC_BYTE                11
#define CDCE906_CLKIN_SRC_SHIFT               6
#define CDCE906_CLKIN_SRC_MASK                0xC0

#define CDCE906_POWER_DOWN_BYTE               12
#define CDCE906_POWER_DOWN_SHIFT              6
#define CDCE906_POWER_DOWN_MASK               0x40

#define CDCE906_P0_DIV_BYTE                   13
#define CDCE906_P0_DIV_SHIFT                  0
#define CDCE906_P0_DIV_MASK                   0x7F

#define CDCE906_P1_DIV_BYTE                   14
#define CDCE906_P1_DIV_SHIFT                  0
#define CDCE906_P1_DIV_MASK                   0x7F

#define CDCE906_P2_DIV_BYTE                   15
#define CDCE906_P2_DIV_SHIFT                  0
#define CDCE906_P2_DIV_MASK                   0x7F

#define CDCE906_P3_DIV_BYTE                   16
#define CDCE906_P3_DIV_SHIFT                  0
#define CDCE906_P3_DIV_MASK                   0x7F

#define CDCE906_P4_DIV_BYTE                   17
#define CDCE906_P4_DIV_SHIFT                  0
#define CDCE906_P4_DIV_MASK                   0x7F

#define CDCE906_P5_DIV_BYTE                   18
#define CDCE906_P5_DIV_SHIFT                  0
#define CDCE906_P5_DIV_MASK                   0x7F

#define CDCE906_Y0_POL_BYTE                   19
#define CDCE906_Y0_POL_SHIFT                  6
#define CDCE906_Y0_POL_MASK                   0x40

#define CDCE906_Y0_SLEW_RATE_BYTE             19
#define CDCE906_Y0_SLEW_RATE_SHIFT            4
#define CDCE906_Y0_SLEW_RATE_MASK             0x30

#define CDCE906_Y0_EN_BYTE                    19
#define CDCE906_Y0_EN_SHIFT                   3
#define CDCE906_Y0_EN_MASK                    0x08

#define CDCE906_Y0_SW_B_BYTE                  19
#define CDCE906_Y0_SW_B_SHIFT                 0
#define CDCE906_Y0_SW_B_MASK                  0x07

#define CDCE906_Y1_POL_BYTE                   20
#define CDCE906_Y1_POL_SHIFT                  6
#define CDCE906_Y1_POL_MASK                   0x40

#define CDCE906_Y1_SLEW_RATE_BYTE             20
#define CDCE906_Y1_SLEW_RATE_SHIFT            4
#define CDCE906_Y1_SLEW_RATE_MASK             0x30

#define CDCE906_Y1_EN_BYTE                    20
#define CDCE906_Y1_EN_SHIFT                   3
#define CDCE906_Y1_EN_MASK                    0x08

#define CDCE906_Y1_SW_B_BYTE                  20
#define CDCE906_Y1_SW_B_SHIFT                 0
#define CDCE906_Y1_SW_B_MASK                  0x07

#define CDCE906_Y2_POL_BYTE                   21
#define CDCE906_Y2_POL_SHIFT                  6
#define CDCE906_Y2_POL_MASK                   0x40

#define CDCE906_Y2_SLEW_RATE_BYTE             21
#define CDCE906_Y2_SLEW_RATE_SHIFT            4
#define CDCE906_Y2_SLEW_RATE_MASK             0x30

#define CDCE906_Y2_EN_BYTE                    21
#define CDCE906_Y2_EN_SHIFT                   3
#define CDCE906_Y2_EN_MASK                    0x08

#define CDCE906_Y2_SW_B_BYTE                  21
#define CDCE906_Y2_SW_B_SHIFT                 0
#define CDCE906_Y2_SW_B_MASK                  0x07

#define CDCE906_Y3_POL_BYTE                   22
#define CDCE906_Y3_POL_SHIFT                  6
#define CDCE906_Y3_POL_MASK                   0x40

#define CDCE906_Y3_SLEW_RATE_BYTE             22
#define CDCE906_Y3_SLEW_RATE_SHIFT            4
#define CDCE906_Y3_SLEW_RATE_MASK             0x30

#define CDCE906_Y3_EN_BYTE                    22
#define CDCE906_Y3_EN_SHIFT                   3
#define CDCE906_Y3_EN_MASK                    0x08

#define CDCE906_Y3_SW_B_BYTE                  22
#define CDCE906_Y3_SW_B_SHIFT                 0
#define CDCE906_Y3_SW_B_MASK                  0x07

#define CDCE906_Y4_POL_BYTE                   23
#define CDCE906_Y4_POL_SHIFT                  6
#define CDCE906_Y4_POL_MASK                   0x40

#define CDCE906_Y4_SLEW_RATE_BYTE             23
#define CDCE906_Y4_SLEW_RATE_SHIFT            4
#define CDCE906_Y4_SLEW_RATE_MASK             0x30

#define CDCE906_Y4_EN_BYTE                    23
#define CDCE906_Y4_EN_SHIFT                   3
#define CDCE906_Y4_EN_MASK                    0x08

#define CDCE906_Y4_SW_B_BYTE                  23
#define CDCE906_Y4_SW_B_SHIFT                 0
#define CDCE906_Y4_SW_B_MASK                  0x07

#define CDCE906_Y5_POL_BYTE                   24
#define CDCE906_Y5_POL_SHIFT                  6
#define CDCE906_Y5_POL_MASK                   0x40

#define CDCE906_Y5_SLEW_RATE_BYTE             24
#define CDCE906_Y5_SLEW_RATE_SHIFT            4
#define CDCE906_Y5_SLEW_RATE_MASK             0x30

#define CDCE906_Y5_EN_BYTE                    24
#define CDCE906_Y5_EN_SHIFT                   3
#define CDCE906_Y5_EN_MASK                    0x08

#define CDCE906_Y5_SW_B_BYTE                  24
#define CDCE906_Y5_SW_B_SHIFT                 0
#define CDCE906_Y5_SW_B_MASK                  0x07

#define CDCE906_EEPIP_BYTE                    24
#define CDCE906_EEPIP_SHIFT                   7
#define CDCE906_EEPIP_MASK                    0x80

#define CDCE906_EELOCK_BYTE                   25
#define CDCE906_EELOCK_SHIFT                  7
#define CDCE906_EELOCK_MASK                   0x80

#define CDCE906_SSC_MOD_SEL_BYTE              25
#define CDCE906_SSC_MOD_SEL_SHIFT             4
#define CDCE906_SSC_MOD_SEL_MASK              0x70

#define CDCE906_SSC_FREQ_SEL_BYTE             25
#define CDCE906_SSC_FREQ_SEL_SHIFT            0
#define CDCE906_SSC_FREQ_SEL_MASK             0x0F

#define CDCE906_EEWRITE_BYTE                  26
#define CDCE906_EEWRITE_SHIFT                 7
#define CDCE906_EEWRITE_MASK                  0x80

#define CDCE906_EECNT_BYTE                    26
#define CDCE906_EECNT_SHIFT                   0
#define CDCE906_EECNT_MASK                    0x7F

enum cdce906_clkin_src {
    CDCE906_CLKIN_SRC_CRYSTAL = 0b00,
    CDCE906_CLKIN_SRC_LVCMOS = 0b01,
    CDCE906_CLKIN_SRC_DIFF = 0b10,
};

#endif
