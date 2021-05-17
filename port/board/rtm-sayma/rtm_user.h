/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2021  Wojciech Ruclo <wojciech.ruclo@creotech.pl>
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

#ifndef RTM_USER_H_
#define RTM_USER_H_

/* PCF9574 outputs */
#define RTM_GPIO_HOTSWAP_HANDLE         0
#define RTM_GPIO_LED_BLUE               1
#define RTM_GPIO_LED_RED                2
#define RTM_GPIO_LED_GREEN              3
#define RTM_GPIO_POWER_GOOD             4
#define RTM_GPIO_RESETn                 5
#define RTM_GPIO_EN_DC_DC               6
#define RTM_GPIO_EEPROM_WP              7

/* Mandatory RTM module functions */
void rtm_enable_payload_power(void);
void rtm_disable_payload_power(void);
uint8_t rtm_get_hotswap_handle_status(uint8_t *state);
void rtm_check_presence(uint8_t *status);
void rtm_hardware_init(void);
void rtm_hardware_close(void);
void rtm_ctrl_led(uint8_t id, uint8_t state);
uint8_t rtm_read_led(uint8_t id);
bool rtm_quiesce(void);
bool rtm_compatibility_check(void);

#endif
