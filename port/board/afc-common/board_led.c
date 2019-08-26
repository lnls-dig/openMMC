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

/* C Standard includes */
#include "string.h"

/* Project includes */
#include "FreeRTOS.h"
#include "queue.h"
#include "ipmi.h"
#include "port.h"
#include "led.h"
#include "string.h"
#include "task_priorities.h"
#include "fru.h"
#include "led.h"
#include "board_led.h"

const uint32_t led_pincfg[] = {
    [LED_BLUE] = GPIO_LEDBLUE,
    [LED1] = GPIO_LEDRED,
    [LED2] = GPIO_LEDGREEN
};

const LEDConfig_t leds_config[] = {
    [LED_BLUE] = {
        .id = LED_BLUE,
        .color = LEDCOLOR_BLUE,
        .act_func = led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 0,
                .t_toggle = 0,
                .init_status = LEDINIT_ON,
            }
        }
    },

    [LED1] = {
        .id = LED1,
        .color = LEDCOLOR_RED,
        .act_func = led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 0,
                .t_toggle = 0,
                .init_status = LEDINIT_OFF,
            }
        }
    },

    [LED2] = {
        .id = LED2,
        .color = LEDCOLOR_GREEN,
        .act_func = led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 5,
                .t_toggle = 5,
                .init_status = LEDINIT_ON,
            }
        }
    },
};

#ifdef MODULE_RTM
const LEDConfig_t rtm_leds_config[] = {
    [LED_BLUE] = {
        .id = LED_BLUE,
        .color = LEDCOLOR_BLUE,
        .act_func = rtm_led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 0xFF,
                .t_toggle = 0,
                .init_status = LEDINIT_ON,
            }
        }
    },

    [LED1] = {
        .id = LED1,
        .color = LEDCOLOR_RED,
        .act_func = rtm_led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 0,
                .t_toggle = 0,
                .init_status = LEDINIT_OFF,
            }
        }
    },

    [LED2] = {
        .id = LED2,
        .color = LEDCOLOR_GREEN,
        .act_func = rtm_led_act,
        .mode = LEDMODE_LOCAL,
        .mode_cfg = {
            [LEDMODE_LOCAL] = {
                .t_init = 5,
                .t_toggle = 5,
                .init_status = LEDINIT_ON,
            }
        }
    },
};
#endif
