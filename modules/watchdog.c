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
 * @file watchdog.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Reset the uC if it fails to reload the watchdog timer
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Project includes */
#include "port.h"
#include "watchdog.h"
#include "task_priorities.h"
#include "uart_debug.h"

void watchdog_init( void )
{
    wdt_init();
    wdt_config();
    wdt_set_timeout(((WATCHDOG_TIMEOUT/1000)*WATCHDOG_CLK_FREQ));
    xTaskCreate( WatchdogTask, (const char *) "Watchdog Task", 60, (void * ) NULL, tskWATCHDOG_PRIORITY, ( TaskHandle_t * ) NULL);
}

void WatchdogTask (void * Parameters)
{
    wdt_start();
    for ( ;; ) {
        wdt_feed();
        vTaskDelay(pdMS_TO_TICKS(WATCHDOG_FEED_DELAY));
    }
}
