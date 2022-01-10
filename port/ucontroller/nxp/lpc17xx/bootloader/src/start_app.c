/****************************************************************************
 * bootloader/src/start_app.c
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

#include "LPC176x5x.h"

__attribute__ ((long_call, noinline, section (".ramtext")))
void start_app(const uint32_t* vtor)
{
	__disable_irq();
	SCB->VTOR = (uint32_t)vtor;
	uint32_t sp = vtor[0];
	uint32_t pc = vtor[1];
	__asm__ volatile (
		"mov sp, %0 \n"
		"bx %1 \n"
		:
		: "r"(sp), "r"(pc)
		:
		);
}
