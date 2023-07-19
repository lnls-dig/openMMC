/****************************************************************************
 * bootloader/src/lpc17_pincfg.h
 *
 *   Copyright (C) 2020 Augusto Fraga Giachero. All rights reserved.
 *   Author: Augusto Fraga Giachero <afg@augustofg.net>
 *
 * This file is part of the RFFE firmware.
 *
 * RFFE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RFFE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RFFE.  If not, see <https://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <stdint.h>

enum pincfg_mode
{
	pin_mode_pullup = 0,
	pin_mode_repeater = 1,
	pin_mode_none = 2,
	pin_mode_pulldown = 3,
};

enum pincfg_func
{
	pin_func_gpio = 0,
	pin_func_alt1 = 1,
	pin_func_alt2 = 2,
	pin_func_alt3 = 3,
};

void lp17_pincfg(uint8_t port, uint8_t pin, enum pincfg_mode mode, enum pincfg_func func);
