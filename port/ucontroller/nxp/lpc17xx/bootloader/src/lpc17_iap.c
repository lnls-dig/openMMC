/****************************************************************************
 * bootloader/src/lpc17_iap.c
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
#include "lpc17_iap.h"

enum iap_cmds
{
	iap_prepare_sectors_write = 50,
	iap_copy_ram_flash,
	iap_erase_sectors,
	iap_blank_check_sectors,
	iap_read_part_id,
	iap_read_boot_code_version,
	iap_read_device_sn,
	iap_compare,
	iap_reinvoke,
};

static const void (*iap_entry) (uint32_t*, uint32_t*) = (void*) 0x1FFF1FF1;

__attribute__ ((long_call, noinline, section (".ramtext")))
enum iap_err lpc17_iap_prepare_sectors(uint8_t start_sector, uint8_t end_sector)
{
	uint32_t inout[5] = {
		(uint32_t)iap_prepare_sectors_write,
		start_sector,
		end_sector,
	};

	iap_entry(inout, inout);

	return (enum iap_err)inout[0];
}

__attribute__ ((long_call, noinline, section (".ramtext")))
enum iap_err lpc17_iap_copy_ram_flash(const uint32_t* src_ram, const uint32_t* dest_flash, size_t len, uint32_t cpu_clk_khz)
{
	uint32_t inout[5] = {
		(uint32_t)iap_copy_ram_flash,
		(uint32_t)dest_flash,
		(uint32_t)src_ram,
		(uint32_t)len,
		cpu_clk_khz,
	};

	iap_entry(inout, inout);

	return (enum iap_err)inout[0];
}

__attribute__ ((long_call, noinline, section (".ramtext")))
enum iap_err lpc17_iap_erase_sectors(uint8_t start_sector, uint8_t end_sector, uint32_t cpu_clk_khz)
{
	uint32_t inout[5] = {
		(uint32_t)iap_erase_sectors,
		start_sector,
		end_sector,
		cpu_clk_khz,
	};

	iap_entry(inout, inout);

	return (enum iap_err)inout[0];
}

__attribute__ ((long_call, noinline, section (".ramtext")))
enum iap_err lpc17_iap_blank_check(uint8_t start_sector, uint8_t end_sector)
{
	uint32_t inout[5] = {
		(uint32_t)iap_blank_check_sectors,
		start_sector,
		end_sector,
	};

	iap_entry(inout, inout);

	return (enum iap_err)inout[0];
}
