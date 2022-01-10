/****************************************************************************
 * bootloader/src/lpc17_pincfg.c
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

#include <stdlib.h>

#include "LPC176x5x.h"
#include "lpc17_pincfg.h"

void lp17_pincfg(uint8_t port, uint8_t pin, enum pincfg_mode mode, enum pincfg_func func)
{
	volatile uint32_t* pinsel = NULL;
	volatile uint32_t* pinmode = NULL;
	uint8_t bit_pos = (pin * 2) % 32;
	uint32_t sel_mask = (uint32_t)3 << bit_pos;
	uint32_t mode_mask = (uint32_t)3 << bit_pos;
	uint32_t sel_val = (uint32_t)func << bit_pos;
	uint32_t mode_val = (uint32_t)mode << bit_pos;

	if (port == 0)
	{
		if (pin <= 15)
		{
			pinsel = &LPC_PINCONNECT->PINSEL0;
			pinmode = &LPC_PINCONNECT->PINMODE0;
		}
		else
		{
			pinsel = &LPC_PINCONNECT->PINSEL1;
			pinmode = &LPC_PINCONNECT->PINMODE1;
		}
	}
	else if (port == 1)
	{
		if (pin <= 15)
		{
			pinsel = &LPC_PINCONNECT->PINSEL2;
			pinmode = &LPC_PINCONNECT->PINMODE2;
		}
		else
		{
			pinsel = &LPC_PINCONNECT->PINSEL3;
			pinmode = &LPC_PINCONNECT->PINMODE3;
		}
	}
	else if (port == 2)
	{
		/*
		 * PINSEL5 / PINMODE5 not available
		 */
		if (pin <= 15)
		{
			pinsel = &LPC_PINCONNECT->PINSEL4;
			pinmode = &LPC_PINCONNECT->PINMODE4;
		}
	}
	else if (port == 3)
	{
		/*
		 * PINSEL6 / PINMODE6 not available
		 */
		if (pin >= 16)
		{
			pinsel = &LPC_PINCONNECT->PINSEL7;
			pinmode = &LPC_PINCONNECT->PINMODE7;
		}
	}
	else if (port == 4)
	{
		/*
		 * PINSEL8 / PINMODE8 not available
		 */
		if (pin >= 16)
		{
			pinsel = &LPC_PINCONNECT->PINSEL9;
			pinmode = &LPC_PINCONNECT->PINMODE9;
		}
	}

	if (pinsel && pinmode)
	{
		*pinsel &= ~sel_mask;
		*pinsel |= sel_val;
		*pinmode &= ~mode_mask;
		*pinmode |= mode_val;
	}
}
