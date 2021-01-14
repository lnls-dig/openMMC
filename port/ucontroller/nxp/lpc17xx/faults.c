/*
 *   Fault handlers
 *
 *   Copyright (C) 2021  Augusto Fraga Giachero  <augusto.fraga@cnpem.br>
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
 * @file faults.c
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>, LNLS
 * @date January 2021
 *
 * @brief ARM Cortex-M fault handlers
 */


#include <stdint.h>
#include <stddef.h>

#include "port.h"
#include "lpc17_uart.h"

static void uartb_send_str(const char *str);
static void uint32_to_hexstr(char *buffer, uint32_t number);

enum fault_type {
    hardfault = 0,
    memmanage = 1,
    busfault = 2,
    usagefault = 3
};

/*
 * The handler functions should be marked as 'naked' so the compiler
 * will not mess with the stack.
 */
void HardFault_Handler(void) __attribute__((naked));
void MemManage_Handler(void) __attribute__((naked));
void BusFault_Handler(void) __attribute__((naked));
void UsageFault_Handler(void) __attribute__((naked));

void dump_registers(uint32_t *stack_addr, enum fault_type fault)
{
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr; /* Link register. */
    volatile uint32_t pc; /* Program counter. */
    volatile uint32_t psr;/* Program status register. */
    char tmp_str[16];

    switch(fault) {
    case hardfault:
        uartb_send_str("\r\nHardFault!!!\r\n");
        break;
    case memmanage:
        uartb_send_str("\r\nMemManage!!!\r\n");
        break;
    case busfault:
        uartb_send_str("\r\nBusFault!!!\r\n");
        break;
    case usagefault:
        uartb_send_str("\r\nUsageFault!!!\r\n");
        break;
    default:
        uartb_send_str("\r\nUnknown\r\n");
        break;
    }

    r0 = stack_addr[0];
    r1 = stack_addr[1];
    r2 = stack_addr[2];
    r3 = stack_addr[3];
    r12 = stack_addr[4];
    lr = stack_addr[5];
    pc = stack_addr[6];
    psr = stack_addr[7];


    uartb_send_str("R0: 0x");
    uint32_to_hexstr(tmp_str, r0);
    uartb_send_str(tmp_str);
    uartb_send_str("\r\nR1: 0x");
    uint32_to_hexstr(tmp_str, r1);
    uartb_send_str(tmp_str);
    uartb_send_str("\r\nR2: 0x");
    uint32_to_hexstr(tmp_str, r2);
    uartb_send_str(tmp_str);
    uartb_send_str("\r\nR3: 0x");
    uint32_to_hexstr(tmp_str, r3);
    uartb_send_str(tmp_str);
    uartb_send_str("\r\nR12: 0x");
    uint32_to_hexstr(tmp_str, r12);
    uartb_send_str(tmp_str);
    uartb_send_str("\r\nLR: 0x");
    uint32_to_hexstr(tmp_str, lr);
    uartb_send_str(tmp_str);
    uartb_send_str("\r\nPC: 0x");
    uint32_to_hexstr(tmp_str, pc);
    uartb_send_str(tmp_str);
    uartb_send_str("\r\nPSR: 0x");
    uint32_to_hexstr(tmp_str, psr);
    uartb_send_str(tmp_str);
    /*
     * FIXME
     *
     * For some motive uartb_send_str should me called again to flush
     * the previous line, maybe a bug in lpcopen?
     */
    uartb_send_str("\r\n------------\r\n");

    for( ;; );
}

void HardFault_Handler(void)
{
    __asm volatile
    (
        "tst lr, #4                                   \n"
        "ite eq                                       \n"
        "mrseq r0, msp                                \n"
        "mrsne r0, psp                                \n"
        "movs r1, #0                                  \n"
        "ldr r3, =dump_registers                      \n"
        "bx r3                                        \n"
    );
}

void MemManage_Handler(void)
{
    __asm volatile
    (
        "tst lr, #4                                   \n"
        "ite eq                                       \n"
        "mrseq r0, msp                                \n"
        "mrsne r0, psp                                \n"
        "movs r1, #1                                  \n"
        "ldr r3, =dump_registers                      \n"
        "bx r3                                        \n"
    );
}

void BusFault_Handler(void)
{
    __asm volatile
    (
        "tst lr, #4                                   \n"
        "ite eq                                       \n"
        "mrseq r0, msp                                \n"
        "mrsne r0, psp                                \n"
        "movs r1, #2                                  \n"
        "ldr r3, =dump_registers                      \n"
        "bx r3                                        \n"
    );
}

void UsageFault_Handler(void)
{
    __asm volatile
    (
        "tst lr, #4                                   \n"
        "ite eq                                       \n"
        "mrseq r0, msp                                \n"
        "mrsne r0, psp                                \n"
        "movs r1, #3                                  \n"
        "ldr r3, =dump_registers                      \n"
        "bx r3                                        \n"
    );
}

static void uartb_send_str(const char *str)
{
    do {
        uart_send(UART_DEBUG, str++, 1);
    } while(*str);
}

static void uint32_to_hexstr(char *buffer, uint32_t number)
{
    for(int8_t i = 28; i >= 0; i -= 4) {
        uint8_t nibble = 0x0F & (number >> i);
        if(nibble > 9) nibble += 'A' - 10;
        else nibble += '0';
        *buffer++ = nibble;
    }
    *buffer = '\0';
}
