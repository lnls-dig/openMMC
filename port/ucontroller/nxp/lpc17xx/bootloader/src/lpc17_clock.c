/****************************************************************************
 * bootloader/src/lpc17_clock.c
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
#include "LPC176x5x.h"

static inline void lpc17_pll0_feed()
{
    LPC_SYSCON->PLL0FEED = 0xAA;
    LPC_SYSCON->PLL0FEED = 0x55;
}

/*
 * PLL0 output clock Fout = (2 * pll_mul * Fsrc) / pll_div
 * CPU clock Fout / cpu_div
 */
int lpc17_set_pll0(uint16_t pll_div, uint16_t pll_mul, uint16_t cpu_div, enum lpc17_pll0_src src)
{
    if (pll_div < 1 || pll_div > 32) return -1;
    if (pll_mul < 6 || pll_mul > 24170) return -1;
    if (cpu_div < 1 || cpu_div > 256) return -1;

    cpu_div--;
    pll_div--;
    pll_mul--;

    /*
     * Change CPU source to the Internal RC oscillator
     */
    LPC_SYSCON->PLL0CON &= ~(SYSCON_PLL0CON_PLLC0_Msk);
    lpc17_pll0_feed();

    /*
     * Disable PLL0
     */
    LPC_SYSCON->PLL0CON &= ~(SYSCON_PLL0CON_PLLE0_Msk);
    lpc17_pll0_feed();

    switch (src)
    {
    case pll0_irc_src:
        LPC_SYSCON->CLKSRCSEL = 0;
        break;

    case pll0_osc_src:
        LPC_SYSCON->CLKSRCSEL = 1;
        break;

    case pll0_rtc_src:
        LPC_SYSCON->CLKSRCSEL = 2;
        break;

    default:
        break;
    }

    /*
     * Set the multiplier and divider values
     */
    LPC_SYSCON->PLL0CFG = (pll_div << SYSCON_PLL0CFG_NSEL0_Pos) |
        (pll_mul << SYSCON_PLL0CFG_MSEL0_Pos);

    /*
     * Enable the PLL0
     */
    LPC_SYSCON->PLL0CON |= (SYSCON_PLL0CON_PLLE0_Msk);
    lpc17_pll0_feed();

    /*
     * Set the CPU clock divider
     */
    LPC_SYSCON->CCLKCFG = cpu_div;

    /*
     * Wait for PLL0 to lock
     */
    while (1)
    {
        volatile uint32_t stat = LPC_SYSCON->PLL0STAT;
        volatile uint32_t start_tst = stat & SYSCON_PLL0STAT_PLOCK0_Msk;
        if (start_tst) break;
    }

    /*
     * Change CPU clock source to PLL0
     */
    LPC_SYSCON->PLL0CON |= SYSCON_PLL0CON_PLLC0_Msk;
    lpc17_pll0_feed();

    return 0;
}

void lpc17_set_pclk(enum lpc17_pclk pclk, enum lpc17_pclk_div div)
{
    uint32_t clk_div = 0;
    uint32_t tmp = 0;

    switch (div)
    {
    case pclk_div1:
        clk_div = 1;
        break;

    case pclk_div2:
        clk_div = 2;
        break;

    case pclk_div4:
        clk_div = 0;
        break;

    case pclk_can_div6:
    case pclk_div8:
        clk_div = 3;
        break;

    default:
        break;
    }

    switch (pclk)
    {
    /* PCLKSEL0 */
    case pclk_wdt:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_WDT_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_WDT_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_timer0:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~ SYSCON_PCLKSEL0_PCLK_TIMER0_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_TIMER0_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_timer1:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_TIMER1_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_TIMER1_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_uart0:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_UART0_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_UART0_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_uart1:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_UART1_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_UART1_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_pwm1:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_PWM1_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_PWM1_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_i2c0:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_I2C0_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_I2C0_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_spi:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_SPI_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_SPI_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_ssp1:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_SSP1_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_SSP1_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_dac:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_DAC_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_DAC_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_adc:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_ADC_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_ADC_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_can1:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_CAN1_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_CAN1_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_can2:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_CAN2_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_CAN2_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    case pclk_acf:
        tmp = LPC_SYSCON->PCLKSEL0;
        tmp &= ~SYSCON_PCLKSEL0_PCLK_ACF_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL0_PCLK_ACF_Pos;
        LPC_SYSCON->PCLKSEL0 = tmp;
        break;

    /* PCLKSEL1 */
    case pclk_qei:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_QEI_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_QEI_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_gpioint:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_GPIOINT_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_GPIOINT_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_pcb:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_PCB_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_PCB_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_i2c1:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_I2C1_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_I2C1_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_ssp0:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_SSP0_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_SSP0_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_timer2:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_TIMER2_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_TIMER2_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_timer3:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_TIMER3_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_TIMER3_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_uart2:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_UART2_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_UART2_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_uart3:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_UART3_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_UART3_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_i2c2:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_I2C2_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_I2C2_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_i2s:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_I2S_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_I2S_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_rit:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_RIT_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_RIT_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_syscon:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_SYSCON_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_SYSCON_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    case pclk_mc:
        tmp = LPC_SYSCON->PCLKSEL1;
        tmp &= ~SYSCON_PCLKSEL1_PCLK_MC_Msk;
        tmp |= clk_div << SYSCON_PCLKSEL1_PCLK_MC_Pos;
        LPC_SYSCON->PCLKSEL1 = tmp;
        break;

    default:
        break;
    }
}
