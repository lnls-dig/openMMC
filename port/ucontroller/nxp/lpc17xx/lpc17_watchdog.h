/*
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
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
 */

/*!
 * @file lpc17_watchdog.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date December 2015
 *
 * @brief Definitions of Watchdog driver implementation for LPC17xx
 */

#ifdef LPC17_WATCHDOG_H_
#undef LPC17_WATCHDOG_H_
#endif
#define LPC17_WATCHDOG_H_

#include "wwdt_17xx_40xx.h"

#define wdt_init()                  Chip_WWDT_Init(LPC_WWDT)
#define wdt_start()                 Chip_WWDT_Start(LPC_WWDT)
#define wdt_config()                Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET)
#define wdt_set_timeout( timeout )  Chip_WWDT_SetTimeOut(LPC_WWDT, timeout)
#define wdt_feed()                  Chip_WWDT_Feed(LPC_WWDT)
