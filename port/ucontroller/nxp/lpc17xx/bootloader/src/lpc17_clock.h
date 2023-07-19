/****************************************************************************
 * bootloader/src/lpc17_clock.h
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

enum lpc17_pll0_src
{
	pll0_irc_src,
	pll0_osc_src,
	pll0_rtc_src,
};

enum lpc17_pclk
{
	/* PCLKSEL0 */
	pclk_wdt,
	pclk_timer0,
	pclk_timer1,
	pclk_uart0,
	pclk_uart1,
	pclk_pwm1,
	pclk_i2c0,
	pclk_spi,
	pclk_ssp1,
	pclk_dac,
	pclk_adc,
	pclk_can1,
	pclk_can2,
	pclk_acf,
	/* PCLKSEL1 */
	pclk_qei,
	pclk_gpioint,
	pclk_pcb,
	pclk_i2c1,
	pclk_ssp0,
	pclk_timer2,
	pclk_timer3,
	pclk_uart2,
	pclk_uart3,
	pclk_i2c2,
	pclk_i2s,
	pclk_rit,
	pclk_syscon,
	pclk_mc,
};

enum lpc17_pclk_div
{
	pclk_div1,
	pclk_div2,
	pclk_div4,
	pclk_can_div6,
	pclk_div8,
};

int lpc17_set_pll0(uint16_t pll_div, uint16_t pll_mul, uint16_t cpu_div, enum lpc17_pll0_src src);
void lpc17_set_pclk(enum lpc17_pclk pclk, enum lpc17_pclk_div div);
