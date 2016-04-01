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

#include "fru.h"

/*********************************************
 * Common defines
 *********************************************/
#define LANG_CODE               0
#define FRU_FILE_ID             "AFCFRU"       //Allows knowing the source of the FRU present in the memory

#define BOARD_INFO_AREA_ENABLE
#define PRODUCT_INFO_AREA_ENABLE
#define MULTIRECORD_AREA_ENABLE

/*********************************************
 * Board information area
 *********************************************/
#define BOARD_MANUFACTURER      "Creotech"
#define BOARD_NAME              "AMC-FMC-Carrier"
#define BOARD_SN                "000000001"
#define BOARD_PN                "AFC"

/*********************************************
 * Product information area
 *********************************************/
#define PRODUCT_MANUFACTURER    "LNLS"
#define PRODUCT_NAME            "AFC Timing"
#define PRODUCT_PN              "00001"
#define PRODUCT_VERSION         "1"
#define PRODUCT_SN              "00001"
#define PRODUCT_ASSET_TAG       "No tag"

/*********************************************
 * AMC: Point to point connectivity record
 *********************************************/
#define AMC_POINT_TO_POINT_RECORD_CNT           0

/*********************************************
 * AMC: Point to point clock record
 *********************************************/
#define AMC_CLOCK_CONFIGURATION_DESCRIPTORS_CNT               2
/* Example:
   DIRECT_CLOCK_CONNECTION(CLOCK_ID, ACTIVATION, PLL_USE, SOURCE/RECEIVER, CLOCK_FAMILY, ACCURACY, FREQUENCY, MIN FREQUENCY, MAX FREQUENCY) */
#define AMC_CLOCK_CONFIGURATION_LIST                                    \
    DIRECT_CLOCK_CONNECTION(FCLKA, CIPMC, NO_PLL, RECEIVER, PCI_RESERVED, 0, MHz(100), MHz(99), MHz(101)), \
        DIRECT_CLOCK_CONNECTION(TCLKB, APP, NO_PLL, SOURCE, UNSPEC_FAMILY, 0, MHz(11), MHz(10), MHz(12))
/**********************************************
 * PICMG: Module current record
 **********************************************/
#define MODULE_CURRENT_RECORD            current_in_ma(4000)
