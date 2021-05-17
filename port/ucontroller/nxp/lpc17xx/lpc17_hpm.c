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
 * @file lpc17_hpm.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 * @date March 2016
 *
 * @brief Firmware update functions (HPM.1)
 */

/* LPC17xx HPM Functions */
#ifdef CHIP_LPC177X_8X
#include "chip_lpc177x_8x.h"
#else
#include "chip_lpc175x_6x.h"
#endif

#include "lpc17_hpm.h"
#include "iap.h"
#include "modules/ipmi.h"
#include "modules/sys_utils.h"
#include "boot/boot.h"
#include "string.h"

uint32_t ipmc_page_addr = 0;
uint32_t ipmc_image_size = 0;
uint32_t ipmc_pg_index = 0;
uint32_t ipmc_page[64];

uint8_t ipmc_hpm_prepare_comp( void )
{
    ipmc_image_size = 0;
    ipmc_pg_index = 0;
    ipmc_page_addr = 0;

    for(uint32_t i=0; i<(sizeof(ipmc_page)/sizeof(uint32_t)); i++) {
        ipmc_page[i] = 0xFFFFFFFF;
    }

    if (ipmc_erase_sector(IPMC_UPDATE_SECTOR_START, IPMC_UPDATE_SECTOR_END) != IAP_CMD_SUCCESS) {
        return IPMI_CC_UNSPECIFIED_ERROR;
    }
    return IPMI_CC_OK;
}

uint8_t ipmc_hpm_upload_block( uint8_t * block, uint16_t size )
{
    uint8_t remaining_bytes_start;

    if ( sizeof(ipmc_page)/4 - ipmc_pg_index > size/4) {
        /* Our page is not full yet, just append the new data */
        for (uint16_t i =0; i < size; i+=4, ipmc_pg_index++) {
            ipmc_page[ipmc_pg_index] = (block[i+3] << 24)|(block[i+2] << 16)|(block[i+1] << 8)|(block[i]);
        }

        return IPMI_CC_OK;

    } else {
        /* Complete the remaining bytes on the buffer */
        remaining_bytes_start = (sizeof(ipmc_page)/4 - ipmc_pg_index)*4;

        for (uint16_t i =0; ipmc_pg_index < sizeof(ipmc_page)/4 ; i+=4, ipmc_pg_index++) {
            ipmc_page[ipmc_pg_index] = (block[i+3] << 24)|(block[i+2] << 16)|(block[i+1] << 8)|(block[i]);
        }

        /* Program the complete page in the Flash */
        ipmc_program_page( ipmc_page_addr, ipmc_page, sizeof(ipmc_page));

        /* Advance the address counter */
        ipmc_page_addr += sizeof(ipmc_page);

        ipmc_image_size += sizeof(ipmc_page);

        /* Empty our buffer and reset the index */
        for(uint32_t i=0; i<(sizeof(ipmc_page)/sizeof(uint32_t)); i++) {
            ipmc_page[i] = 0xFFFFFFFF;
        }
        ipmc_pg_index = 0;

        /* Save the trailing bytes */
        for (uint8_t i =0; i <(size-remaining_bytes_start); i+=4) {
            ipmc_page[ipmc_pg_index++] = (block[i+3+remaining_bytes_start] << 24)|(block[i+2+remaining_bytes_start] << 16)|(block[i+1+remaining_bytes_start] << 8)|(block[i+remaining_bytes_start]);
        }

        return IPMI_CC_COMMAND_IN_PROGRESS;
    }
}

uint8_t ipmc_hpm_finish_upload( uint32_t image_size )
{
    /* Check if the last page was already programmed */
    if (ipmc_pg_index != 0) {
        /* Program the complete page in the Flash */
        ipmc_program_page( ipmc_page_addr, ipmc_page, sizeof(ipmc_page));
        ipmc_image_size += ipmc_pg_index*4;
        ipmc_pg_index = 0;
        ipmc_page_addr = 0;
    }

    if (ipmc_image_size != image_size) {
        /* HPM CC: Number of bytes received does not match the size provided in the "Finish firmware upload" request */
        return 0x81;
    }
    /* Copy the last page (we'll change only the last word) */
    memcpy(ipmc_page, (uint32_t *) (UPGRADE_FLASH_END_ADDR-256), sizeof(ipmc_page));

    /* TODO: Write actual firmware ID */
    /* Write bootloader magic word */
    ipmc_page[63] = 0x55555555;
    ipmc_program_page( UPGRADE_FLASH_END_ADDR-IPMC_UPDATE_ADDRESS_OFFSET-256, ipmc_page, sizeof(ipmc_page));

    return IPMI_CC_OK;
}

uint8_t ipmc_hpm_get_upgrade_status( void )
{
    /* The IAP commands run when they're called and block the firmware. Long commands would cause timeouts on the IPMB */
    return IPMI_CC_OK;
}

uint8_t ipmc_hpm_activate_firmware( void )
{
    /* Schedule a reset to 500ms from now */
    sys_schedule_reset(500);
    return IPMI_CC_OK;
}

uint8_t ipmc_program_page( uint32_t address, uint32_t * data, uint32_t size )
{
    if (size % 256) {
        /* Data should be a 256 byte boundary */
        return IPMI_CC_PARAM_OUT_OF_RANGE;
    }

    portDISABLE_INTERRUPTS();

    if (Chip_IAP_PreSectorForReadWrite( IPMC_UPDATE_SECTOR_START, IPMC_UPDATE_SECTOR_END ) != IAP_CMD_SUCCESS) {
        portENABLE_INTERRUPTS();
        return IPMI_CC_UNSPECIFIED_ERROR;
    }

    if (Chip_IAP_CopyRamToFlash( IPMC_UPDATE_ADDRESS_OFFSET + address, data, size )) {
        portENABLE_INTERRUPTS();
        return IPMI_CC_UNSPECIFIED_ERROR;
    }

    portENABLE_INTERRUPTS();
    return IPMI_CC_OK;
}

uint8_t ipmc_erase_sector( uint32_t sector_start, uint32_t sector_end)
{
    portDISABLE_INTERRUPTS();
    if (Chip_IAP_PreSectorForReadWrite( sector_start, sector_end ) != IAP_CMD_SUCCESS) {
        portENABLE_INTERRUPTS();
        return IPMI_CC_UNSPECIFIED_ERROR;
    }
    if (Chip_IAP_EraseSector( sector_start, sector_end ) != IAP_CMD_SUCCESS) {
        portENABLE_INTERRUPTS();
        return IPMI_CC_UNSPECIFIED_ERROR;
    }
    portENABLE_INTERRUPTS();
    return IPMI_CC_OK;
}
