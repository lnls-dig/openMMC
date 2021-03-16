/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Henrique Silva <henrique.silva@lnls.br>
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

#ifdef ADN4604_USERCFG_H_
#error "User configuration for ADN4604 Clock switch already defined by other board port, check the build chain!"
#else
#define ADN4604_USERCFG_H_

/* User configuration defines for ADN4604 Clock switch output config */
#define ADN4604_CFG_OUT_0       0       /* TCLKD_OUT */
#define ADN4604_CFG_OUT_1       0       /* TCLKC_OUT */
#define ADN4604_CFG_OUT_2       0       /* TCLKA_OUT */
#define ADN4604_CFG_OUT_3       0       /* TCLKB_OUT */
#define ADN4604_CFG_OUT_4       13      /* FPGA_CCLK */
#define ADN4604_CFG_OUT_5       8       /* FP2_CLK2 */
#define ADN4604_CFG_OUT_6       5       /* LINK01_CLK */
#define ADN4604_CFG_OUT_7       15      /* FP2_CLK1 */
#define ADN4604_CFG_OUT_8       8       /* PCIE_CLK1 */
#define ADN4604_CFG_OUT_9       5       /* LINK23_CLK */
#define ADN4604_CFG_OUT_10      5       /* FIN1_CLK3 */
#define ADN4604_CFG_OUT_11      5       /* FIN1_CLK2 */
#define ADN4604_CFG_OUT_12      14      /* RTM_SYNC_CLK */
#define ADN4604_CFG_OUT_13      5       /* OP15C (Aux U-Fl connector) */
#define ADN4604_CFG_OUT_14      5       /* FIN2_CLK2 */
#define ADN4604_CFG_OUT_15      5       /* FIN2_CLK3 */

/* Output enable flags */
#define ADN4604_EN_OUT_0        0       /* TCLKD_OUT */
#define ADN4604_EN_OUT_1        0       /* TCLKC_OUT */
#define ADN4604_EN_OUT_2        0       /* TCLKA_OUT */
#define ADN4604_EN_OUT_3        0       /* TCLKB_OUT */
#define ADN4604_EN_OUT_4        1       /* FPGA_CCLK */
#define ADN4604_EN_OUT_5        0       /* FP2_CLK2 */
#define ADN4604_EN_OUT_6        1       /* LINK01_CLK */
#define ADN4604_EN_OUT_7        1       /* FP2_CLK1 */
#define ADN4604_EN_OUT_8        1       /* PCIE_CLK1 */
#define ADN4604_EN_OUT_9        0       /* LINK23_CLK */
#define ADN4604_EN_OUT_10       0       /* FIN1_CLK3 */
#define ADN4604_EN_OUT_11       0       /* FIN1_CLK2 */
#define ADN4604_EN_OUT_12       0       /* RTM_SYNC_CLK */
#define ADN4604_EN_OUT_13       1       /* OP15C (Aux U-Fl connector) */
#define ADN4604_EN_OUT_14       0       /* FIN2_CLK2 */
#define ADN4604_EN_OUT_15       0       /* FIN2_CLK3 */

#endif
