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
 * @file   watchdog.h
 * @author Henrique Silva <henrique.silva@lnls.br>
 *
 * @brief Watchdog module definitions
 *
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

/**
 * @brief Watchdog module clock frequency (in Hz)
 */
#define WATCHDOG_CLK_FREQ       8000000

/**
 * @brief Watchdog timer timeout (in ms)
 */
#define WATCHDOG_TIMEOUT        1000    /* in milisseconds */

/**
 * @brief Watchdog timer feed delay
 *
 * We reload the watchdog timer at 2/3 of the max count
 */
#define WATCHDOG_FEED_DELAY     (WATCHDOG_TIMEOUT/3)*2


/**
 * @brief Initializes and configures Watchdog timer
 *
 * @return None
 */
void watchdog_init( void );

/**
 * @brief Schedules a Reset on the next run of the Watchdog task
 *
 * @return None
 */
void watchdog_reset_mcu( void );

/**
 * @brief Periodically feed the Watchdog timer and checks if there's any Reset requests
 *
 * @param Parameters Pointer to a buffer of parameters passed to the task upon initialization
 *
 * @return None
 */
void WatchdogTask( void * Parameters );

#endif
