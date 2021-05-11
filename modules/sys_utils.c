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
 * @file   sys_utils.c
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  System utilities functions
 *
 * @ingroup sysutils
 */

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "timers.h"

/* Project includes */
#include "port.h"

static void sys_reset_callback(TimerHandle_t timer)
{
    xTimerDelete(timer, 0);
    mcu_reset();
}

int sys_schedule_reset(int period_ms)
{
    TimerHandle_t timer_sys_rst;
    int ret = 0;

    timer_sys_rst = xTimerCreate("System Reset", pdMS_TO_TICKS(period_ms),
                                 pdFALSE, (void*)0, sys_reset_callback);

    if (timer_sys_rst != NULL) {
        BaseType_t start_res = xTimerStart(timer_sys_rst, 0);
        if (start_res == pdFAIL) ret = -1;
    } else {
        ret = -1;
    }

    return ret;
}
