/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Julian Mendez  <julian.mendez@cern.ch>
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

#include "fru_editor.h"

/*********************************************
 * Common defines
 *********************************************/
#define FMC_LANG_CODE               0
#define FMC_FRU_FILE_ID             "FMCFRU"       //Allows knowing the source of the FRU present in the memory

/*********************************************
 * Board information area
 *********************************************/
#define FMC_BOARD_MANUFACTURING_TIME 10913760         /* Amount of minutes since 0:00 1/1/96 until 1/oct/2016 */
#define FMC_BOARD_MANUFACTURER      "MacroFab"
#define FMC_BOARD_NAME              "FMC ADC 250M 16b"
#define FMC_BOARD_SN                "1100150001"
#define FMC_BOARD_PN                "FMC-250"

/*********************************************
 * Product information area
 *********************************************/
#define FMC_PRODUCT_MANUFACTURER    "LNLS"
#define FMC_PRODUCT_NAME            "FMC ADC 250M 16b"
#define FMC_PRODUCT_PN              "00001"
#define FMC_PRODUCT_VERSION         "v2.0"
#define FMC_PRODUCT_SN              "1100150001"
#define FMC_PRODUCT_ASSET_TAG       "No tag"

/*********************************************
 * FMC specific records
 *********************************************/
#define FMC_CLOCK_DIR                CARRIER_TO_MEZZANINE
#define FMC_MODULE_SIZE              FMC_SINGLE_WIDTH
#define FMC_P1_CONN_SIZE             FMC_CONN_HPC
#define FMC_P2_CONN_SIZE             FMC_CONN_NOT_FITTED
#define FMC_P1_BANK_A_COUNT          102
#define FMC_P1_BANK_B_COUNT          38
#define FMC_P2_BANK_A_COUNT          0
#define FMC_P2_BANK_B_COUNT          0
#define FMC_P1_GBT                   4
#define FMC_P2_GBT                   0

/*********************************************
 * FMC Load records
 *********************************************/
#define FMC_VADJ_VOLT                2500  /* in mV */
#define FMC_VADJ_MIN                 1800
#define FMC_VADJ_MAX                 3300
#define FMC_VADJ_RIPPLE              0
#define FMC_VADJ_MIN_LOAD            0
#define FMC_VADJ_MAX_LOAD            1000

#define FMC_P3V3_VOLT                3300
#define FMC_P3V3_MIN                 3100
#define FMC_P3V3_MAX                 3400
#define FMC_P3V3_RIPPLE              0
#define FMC_P3V3_MIN_LOAD            0
#define FMC_P3V3_MAX_LOAD            2000

#define FMC_P12V_VOLT                12000
#define FMC_P12V_MIN                 10800
#define FMC_P12V_MAX                 12800
#define FMC_P12V_RIPPLE              0
#define FMC_P12V_MIN_LOAD            0
#define FMC_P12V_MAX_LOAD            4000
