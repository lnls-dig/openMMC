/****************************************************************************
 * bootloader/src/lpc17_uart.c
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

#include "lpc17_clock.h"
#include "lpc17_uart.h"
#include "LPC176x5x.h"

int lpc17_uart0_init(uint32_t baud, uint32_t cpu_clk)
{
	/*
	 * Set UART0 clock as the same of CPU
	 */
	lpc17_set_pclk(pclk_uart0, pclk_div1);

	/*
	 * Enable the UART0 peripheral
	 */
	LPC_SYSCON->PCONP |= SYSCON_PCONP_PCUART0_Msk;

	/*
	 * Disable fractional baudrate generation
	 */
	LPC_UART0->FDR = (0 << UART0_FDR_DIVADDVAL_Pos) |
		(1 << UART0_FDR_MULVAL_Pos);

	/*
	 * Calculate the baudrate from cpu_clk;
	 */
	volatile uint32_t uart_div = cpu_clk / (16 * baud);

	if (uart_div < 1 || uart_div > 0xFFFF) return -1;

	/*
	 * Enable divisor latch access, write the divisor value
	 */
	LPC_UART0->LCR |= UART0_LCR_DLAB_Msk;
	LPC_UART0->DLM = (uart_div >> 8) & 0xFF;
	LPC_UART0->DLL = uart_div & 0xFF;
	LPC_UART0->LCR &= ~UART0_LCR_DLAB_Msk;

	/*
	 * Configure UART0: 8N1
	 */
	LPC_UART0->LCR = (3 << UART0_LCR_WLS_Pos);

	/*
	 * Enable the UART0 transmitter
	 */
	LPC_UART0->TER = UART0_TER_TXEN_Msk;

	return 0;
}

int lpc17_uart0_write_blocking(const uint8_t* buffer, size_t len)
{
	for (; len > 0; len--)
	{
		while ((LPC_UART0->LSR & UART0_LSR_THRE_Msk) == 0);
		LPC_UART0->THR = *buffer++;
	}
	return len;
}

int lpc17_uart0_write_str_blocking(const char* buffer)
{
	int len = 0;
	while (*buffer)
	{
		while ((LPC_UART0->LSR & UART0_LSR_THRE_Msk) == 0);
		LPC_UART0->THR = *buffer++;
		len++;
	}
	return len;
}
