/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
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

#ifndef IPMI_OEM_H_
#define IPMI_OEM_H_

/**
 * @file afc-timing/ipmi_oem.h
 * @brief Custom IPMI commands for AFC Timing
 *
 * @ingroup AFC_TIMING_IPMI_OEM
 */

/**
 * @defgroup AFC_TIMING_IPMI_OEM AFC-Timing IPMI OEM Commands
 * @ingroup AFC_TIMING
 * @{
 */

#include "ipmi.h"

/**
 * @brief Custom NetFN (User defined, value greater than 0x2C)
 */
#define NETFN_CUSTOM_OEM                    0x30

#define IPMI_OEM_CMD_I2C_TRANSFER               0x00

#define IPMI_OEM_CMD_ADN4604_SET_OUTPUT_CFG     0x01
#define IPMI_OEM_CMD_ADN4604_GET_OUTPUT_CFG     0x02
#define IPMI_OEM_CMD_ADN4604_RESET              0x03

#define IPMI_OEM_CMD_GPIO_PIN                   0x04
/**
 * @}
 */

#endif
