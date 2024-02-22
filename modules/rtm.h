/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
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

/**
 * @file   rtm.h
 * @author Henrique Silva <henrique.silva@lnls.br>
 *
 * @brief  Generic RTM Module management functions
 *
 * @ingroup RTM_Manage
 */

#ifndef RTM_H_
#define RTM_H_

#include "rtm_user.h"

extern volatile bool rtm_present;

/**
 * @brief Initialize RTM Manage task
 *
 * This task creates the RTM_Manage task and initialize some GPIO pins used to detect the RTM Board presence
 *
 * @return None
 */
void rtm_manage_init( void );

/**
 * @brief Check RTM Presence
 *
 * This task use some GPIO pins to detect the RTM Board presence
 *
 * @return None
 */
void rtm_check_presence(uint8_t *status);

/**
 * @brief Enable payload power
 *
 * This function turn on the 12V payload power rail and initialise the RTM powered by it.
 *
 */
mmc_err rtm_enable_payload_power( void );

/**
 * @brief Disable payload power
 *
 * This function turn off the 12V payload power rail and de-initialise the RTM powered by it.
 *
 */
mmc_err rtm_disable_payload_power( void );


#endif
