/****************************************************************************
 * bootloader/src/lpc17_iap.h
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
#include <stddef.h>

enum iap_err
{
	iap_cmd_success = 0,
	iap_invalid_command,
	iap_src_addr_error,
	iap_dst_addr_error,
	iap_src_addr_not_mapped,
	iap_dst_addr_not_mapped,
	iap_count_error,
	iap_invalid_sector,
	iap_sector_not_blank,
	iap_sector_not_prepared_for_write,
	iap_compare_error,
	iap_busy,
	iap_param_error,
	iap_addr_error,
	iap_addr_not_mapped,
	iap_cmd_locked,
	iap_invalid_code,
	iap_invalid_baud_rate,
	iap_invalid_stop_bit,
	iap_code_read_protection_enabled,
};

enum iap_err lpc17_iap_prepare_sectors(uint8_t start_sector, uint8_t end_sector);
enum iap_err lpc17_iap_copy_ram_flash(const uint32_t* src_ram, const uint32_t* dest_flash, size_t len, uint32_t cpu_clk_khz);
enum iap_err lpc17_iap_erase_sectors(uint8_t start_sector, uint8_t end_sector, uint32_t cpu_clk_khz);
enum iap_err lpc17_iap_blank_check(uint8_t start_sector, uint8_t end_sector);
