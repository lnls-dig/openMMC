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
#include "chip_lpc175x_6x.h"
#include "lpc17_hpm.h"
#include "iap.h"
#include "modules/ipmi.h"
#include "modules/sys_utils.h"
#include "boot/boot.h"
#include "string.h"
#include "arm_cm3_reset.h"

typedef struct
{
    uint8_t version[3];
    uint8_t fw_type;
    uint32_t magic;
    uint8_t RESERVED[248];
} fw_info;

/*
 * Flash symbols defined in the linker script
 */
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

uint32_t ipmc_page_addr = 0;
uint32_t ipmc_image_size = 0;
uint32_t ipmc_pg_index = 0;
uint32_t ipmc_page[64];

static uint8_t get_sector_number(const void* flash_addr)
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

uint8_t ipmc_hpm_prepare_comp( void )
{
    ipmc_image_size = 0;
    ipmc_pg_index = 0;
    ipmc_page_addr = 0;

    for(uint32_t i=0; i<(sizeof(ipmc_page)/sizeof(uint32_t)); i++) {
        ipmc_page[i] = 0xFFFFFFFF;
    }

    /*
     * Assumes that the bootloader has erased the flash update
     * area. Erasing it here will lock the flash, preventing code
     * execution until it finishes. This can take tens of ms for
     * multiple sector erases, and for all this time no interrupt
     * exceptions can be served.
     *
     * For LPC1768 devices it would cause the HPM update to fail as it
     * wouldn't answer the ipmi request in time.
     */

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

    return IPMI_CC_OK;
}

uint8_t ipmc_hpm_get_upgrade_status( void )
{
    /* The IAP commands run when they're called and block the firmware. Long commands would cause timeouts on the IPMB */
    return IPMI_CC_OK;
}

uint8_t ipmc_hpm_activate_firmware( void )
{
    fw_info fw_update_header;
    memset(&fw_update_header, 0xFF, sizeof(fw_update_header));

    /*
     * Write firmware update record to inform the bootloader that a
     * new firmware is available.
     *
     * TODO: Write actual firmware ID
     */
    fw_update_header.magic = 0xAAAAAAAA;
    fw_update_header.fw_type = 1; // Update application
    fw_update_header.version[0] = 1;
    fw_update_header.version[1] = 4;
    fw_update_header.version[2] = 1;
    ipmc_program_page((uint32_t)fw_header - (uint32_t)update_start_addr, (uint32_t*)&fw_update_header, sizeof(fw_update_header));

    /*
     * Imediately reset the mcu, for some motive scheduling a reset
     * using FreeRTOS timers isn't working (xTimerStart hangs for ever)
     */
    mcu_reset();

    /* Schedule a reset to 500ms from now */
    /* sys_schedule_reset(500); */
    return IPMI_CC_OK;
}

uint8_t ipmc_program_page( uint32_t address, uint32_t * data, uint32_t size )
{
    const uint32_t update_start_sec = get_sector_number(update_start_addr);
    const uint32_t update_end_sec = get_sector_number(update_end_addr);

    if (size % 256) {
        /* Data should be a 256 byte boundary */
        return IPMI_CC_PARAM_OUT_OF_RANGE;
    }

    portDISABLE_INTERRUPTS();

    if (Chip_IAP_PreSectorForReadWrite(update_start_sec, update_end_sec) != IAP_CMD_SUCCESS) {
        portENABLE_INTERRUPTS();
        return IPMI_CC_UNSPECIFIED_ERROR;
    }

    if (Chip_IAP_CopyRamToFlash((uint32_t)update_start_addr + address, data, size)) {
        portENABLE_INTERRUPTS();
        return IPMI_CC_UNSPECIFIED_ERROR;
    }

    portENABLE_INTERRUPTS();
    return IPMI_CC_OK;
}

uint8_t ipmc_erase_sector( uint32_t sector_start, uint32_t sector_end)
{
    portDISABLE_INTERRUPTS();
    if (Chip_IAP_PreSectorForReadWrite(sector_start, sector_end) != IAP_CMD_SUCCESS) {
        portENABLE_INTERRUPTS();
        return IPMI_CC_UNSPECIFIED_ERROR;
    }
    if (Chip_IAP_EraseSector(sector_start, sector_end) != IAP_CMD_SUCCESS) {
        portENABLE_INTERRUPTS();
        return IPMI_CC_UNSPECIFIED_ERROR;
    }
    portENABLE_INTERRUPTS();
    return IPMI_CC_OK;
}
