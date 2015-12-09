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
 * @file watchdog.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date December 2015
 *
 * @brief Reset the uC if it fails to reload the watchdog timer
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* Project includes */
#include "port.h"
#include "watchdog.h"
#include "task_priorities.h"

#define WATCHDOG_CLK_FREQ 8000000
#define WATCHDOG_TIMEOUT 3 /* in seconds */
#define WATCHDOG_FEED_DELAY (WATCHDOG_TIMEOUT/3)*2*1000

void watchdog_init( void )
{
    wdt_init();
    wdt_config();
    wdt_set_timeout(WATCHDOG_TIMEOUT*WATCHDOG_CLK_FREQ);
    xTaskCreate( WatchdogTask, (const char *) "Watchdog Task", configMINIMAL_STACK_SIZE, (void * ) NULL, tskWATCHDOG_PRIORITY, ( TaskHandle_t * ) NULL);
}

void WatchdogTask (void * Parameters)
{
    wdt_start();
    for ( ;; ) {
	vTaskDelay(WATCHDOG_FEED_DELAY);
	wdt_feed();
    }
}
