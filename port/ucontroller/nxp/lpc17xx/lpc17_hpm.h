/*
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
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
 * @file lpc17_hpm.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date March 2016
 *
 * @brief Firmware update functions (HPM.1)
 */

#ifdef LPC17_HPM_H_
#undef LPC17_HPM_H_
#endif
#define LPC17_HPM_H_

#define IPMC_UPDATE_SECTOR_START 0x10
#define IPMC_UPDATE_SECTOR_END   0x11
#define IPMC_UPDATE_ADDRESS_OFFSET (IPMC_UPDATE_SECTOR_START << 12)

enum memory_area {
    BOOT_FLASH,
    FW_UPDATE_FLASH
};

uint8_t ipmc_hpm_prepare_comp(void);
uint8_t ipmc_hpm_upload_block(uint8_t *block, uint16_t size);
uint8_t ipmc_hpm_finish_upload(uint32_t image_size);
uint8_t ipmc_hpm_activate_firmware(void);
uint8_t ipmc_hpm_get_upgrade_status(void);
uint8_t program_page(uint32_t address, uint32_t *data, uint32_t size, enum memory_area area);
uint8_t ipmc_erase_sector(uint32_t sector_start, uint32_t sector_end);


uint8_t bootloader_hpm_prepare_comp(void);
uint8_t bootloader_hpm_upload_block(uint8_t *block, uint16_t size);
uint8_t bootloader_hpm_finish_upload(uint32_t image_size);
uint8_t bootloader_hpm_activate_firmware(void);
uint8_t bootloader_hpm_get_upgrade_status(void);
