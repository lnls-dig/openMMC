/****************************************************************************
 * bootloader/src/main.c
 *
 *   Copyright (C) 2020 Augusto Fraga Giachero. All rights reserved.
 *   Copyright (C) 2021-2022 CNPEM. All rights reserved.
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
#include <stdio.h>

#include "LPC176x5x.h"
#include "lpc17_clock.h"
#include "lpc17_pincfg.h"
#include "lpc17_uart.h"
#include "lpc17_iap.h"
#include "start_app.h"

enum fw_update_type
{
    FW_UPDATE_APP = 1,
    FW_UPDATE_BOOT = 2,
};

typedef struct
{
    uint8_t version[3];
    uint8_t fw_type;
    uint32_t magic;
} fw_info;

extern const uint32_t __AppFlash_start;
extern const uint32_t __AppFlash_end;
extern const uint32_t __BootFlash_start;
extern const uint32_t __BootFlash_end;
extern const uint32_t __FWUpdateFlash_start;
extern const uint32_t __FWUpdateFlash_end;
extern const fw_info __FWInfo_addr;

const uint32_t* app_start_addr = &__AppFlash_start;
const uint32_t* app_end_addr = &__AppFlash_end;
const uint32_t* boot_start_addr = &__BootFlash_start;
const uint32_t* boot_end_addr = &__BootFlash_end;
const uint32_t* update_start_addr = &__FWUpdateFlash_start;
const uint32_t* update_end_addr = &__FWUpdateFlash_end;

const fw_info* fw_header = &__FWInfo_addr;

char* u8_to_str(uint8_t n, char* str)
{
    uint8_t digits[3];

    digits[0] = n / 100;
    digits[1] = (n - digits[0]) / 10;
    digits[2] = (n - digits[1]);

    if (digits[0] == 0)
    {
        if (digits[1] == 0)
        {
            str[0] = digits[2] | 0x30;
            str[1] = 0;
        }
        else
        {
            str[0] = digits[1] | 0x30;
            str[1] = digits[2] | 0x30;
            str[2] = 0;
        }
    }
    else
    {
        str[0] = digits[0] | 0x30;
        str[1] = digits[1] | 0x30;
        str[2] = digits[2] | 0x30;
        str[3] = 0;
    }

    return str;
}

uint8_t get_sector_number(const void* flash_addr)
{
    uint8_t ret = 0;
    const uint32_t flash = (const uint32_t)flash_addr;

    if (flash < 0x10000)
    {
        ret = (flash / 0x1000);
    }
    else
    {
        ret = ((flash - 0x10000) / 0x8000) + 16;
    }
    return ret;
}

void copy_flash_region(const uint32_t* src, const uint32_t* dest, size_t len, uint32_t cpu_clk_khz)
{
    uint32_t buffer[64];

    if (len % 256) return;

    for (size_t i = 0; i < len; i += 256, src += 64, dest += 64)
    {
        for (size_t i = 0; i < 64; i++)
        {
            buffer[i] = src[i];
        }

        uint8_t sector = get_sector_number(dest);

        lpc17_iap_prepare_sectors(sector, sector);
        lpc17_iap_copy_ram_flash(buffer, dest, 256, cpu_clk_khz);
    }
}

void update(uint32_t cpu_clk_khz, enum fw_update_type ftype)
{
    uint32_t target_start_sec;
    uint32_t target_end_sec;
    uint32_t target_size;
    const uint32_t* target_start_addr;
    const uint32_t update_start_sec = get_sector_number(update_start_addr);
    const uint32_t update_end_sec = get_sector_number(update_end_addr);

    if (ftype == FW_UPDATE_APP)
    {
        target_start_sec = get_sector_number(app_start_addr);
        target_end_sec = get_sector_number(app_end_addr);
        target_size = (uint32_t)app_end_addr - (uint32_t)app_start_addr + 1;
        target_start_addr = app_start_addr;
    }
    else if (ftype == FW_UPDATE_BOOT)
    {
        target_start_sec = get_sector_number(boot_start_addr);
        target_end_sec = get_sector_number(boot_end_addr);
        target_size = (uint32_t)boot_end_addr - (uint32_t)boot_start_addr + 1;
        target_start_addr = boot_start_addr;
    }
    else return;

    lpc17_iap_prepare_sectors(target_start_sec, target_end_sec);
    lpc17_iap_erase_sectors(target_start_sec, target_end_sec, cpu_clk_khz);

    copy_flash_region(update_start_addr, target_start_addr, target_size, cpu_clk_khz);

    /*
     * Erase flash firmware update section
     */
    lpc17_iap_prepare_sectors(update_start_sec, update_end_sec);
    lpc17_iap_erase_sectors(update_start_sec, update_end_sec, cpu_clk_khz);

    /*
     * Jump to application code
     */
    start_app(app_start_addr);
}

int main(void)
{
    /*
     * Update flash access time to 4 CPU clocks (required when running
     * from 60 to 80MHz)
     */
    LPC_SYSCON->FLASHCFG &= ~SYSCON_FLASHCFG_FLASHTIM_Msk;
    LPC_SYSCON->FLASHCFG |= (3 << SYSCON_FLASHCFG_FLASHTIM_Pos);

    /*
     * PLL0 output clock Fout = (2 * pll_mul * Fsrc) / pll_div
     * CPU clock Fout / cpu_div
     *
     * For a 72 MHz cpu clock using the internal RC:
     * pll_div = 1;
     * pll_mull = 18;
     * cpu_div = 2;
     */
    lpc17_set_pll0(1, 18, 2, pll0_irc_src);

    /*
     * Configure pins P0.2 as TXD0 and P0.3 as RXD0
     */
    lp17_pincfg(0, 2, pin_mode_none, pin_func_alt1);
    lp17_pincfg(0, 3, pin_mode_none, pin_func_alt1);

    /*
     * Initialize UAR0 (19200bps, 8N1)
     */
    lpc17_uart0_init(19200, 72000000);

    if (fw_header->magic == 0xAAAAAAAA)
    {
        char tmp[128];

        lpc17_uart0_write_str_blocking("[BOOTLOADER] DO NOT TURN OFF WHILE UPDATING!\r\n");

        if (fw_header->fw_type == 1)
        {
            lpc17_uart0_write_str_blocking("[BOOTLOADER] New app firmware update found!\r\nUpdating to ");
            lpc17_uart0_write_str_blocking(u8_to_str(fw_header->version[0], tmp));
            lpc17_uart0_write_str_blocking(".");
            lpc17_uart0_write_str_blocking(u8_to_str(fw_header->version[1], tmp));
            lpc17_uart0_write_str_blocking(".");
            lpc17_uart0_write_str_blocking(u8_to_str(fw_header->version[2], tmp));
            lpc17_uart0_write_str_blocking("...\r\n");

            update(72000, FW_UPDATE_APP);
        }
        else if (fw_header->fw_type == 2)
        {
            lpc17_uart0_write_str_blocking("[BOOTLOADER] New bootloader firmware update found!\r\nUpdating to ");
            lpc17_uart0_write_str_blocking(u8_to_str(fw_header->version[0], tmp));
            lpc17_uart0_write_str_blocking(".");
            lpc17_uart0_write_str_blocking(u8_to_str(fw_header->version[1], tmp));
            lpc17_uart0_write_str_blocking(".");
            lpc17_uart0_write_str_blocking(u8_to_str(fw_header->version[2], tmp));
            lpc17_uart0_write_str_blocking("...\r\n");

            update(72000, FW_UPDATE_BOOT);
        }
        else
        {
            lpc17_uart0_write_str_blocking("[BOOTLOADER] ERROR: Unknown fw_type ");
            lpc17_uart0_write_str_blocking(u8_to_str(fw_header->fw_type, tmp));
            lpc17_uart0_write_str_blocking(" !\r\n Jumping to application code...\r\n");
        }
    }

    /*
     * Jump to application code
     */
    start_app(app_start_addr);
    return 0;
}
