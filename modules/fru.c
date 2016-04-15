/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
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
 *
 *   @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

/*! @file fru.c
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"

#include "fru.h"
#include "at24mac.h"
#include "utils.h"
#include "ipmi.h"
#include "fru_editor.h"

void fru_init( void )
{
    portENABLE_INTERRUPTS();

    fru_runtime = false;

    /* Read FRU info Common Header */
    uint8_t common_header[8];

    if ( at24mac_read_nortos( 0x00, &common_header[0], 8 ) == 8 ) {
        if ( (calculate_chksum( &common_header[0], 7 ) != common_header[7]) || common_header[0] != 1 ) {
            fru_info_build( fru_info );

#ifdef FRU_WRITE_EEPROM
            at24mac_write_nortos( 0x00, &fru_info[0], sizeof(fru_info) );
#endif
            at24mac_read_nortos( 0x00, &common_header[0], 8 );
            if (common_header[0] != 1) {
            	fru_runtime = true;
            }
        }
    } else {
        /* Could not access the SEEPROM, create a runtime fru info */
        fru_info_build( fru_info );
        fru_runtime = true;
    }

    portDISABLE_INTERRUPTS();
}

size_t fru_read( uint8_t *rx_buff, uint16_t offset, size_t len )
{
    uint16_t i;
    uint16_t j = offset;

    if (fru_runtime) {
        for (i = 0; i < len; i++, j++ ) {
            if (j < (sizeof(fru_info)/sizeof(fru_info[0]))) {
                rx_buff[i] = fru_info[j];
            } else {
                rx_buff[i] = 0xFF;
            }
        }
        return i;
    } else {
        return at24mac_read( offset, rx_buff, len );
    }
}

size_t fru_write( uint8_t *tx_buff, uint16_t offset, size_t len )
{
    if ( fru_runtime ) {
        uint8_t i;
        for (i = 0; i < len; i++) {
            fru_info[offset+i] = tx_buff[i];
        }
        return i;
    } else {
        return at24mac_write( offset, tx_buff, len );
    }
}


/* IPMI Handlers */
IPMI_HANDLER(ipmi_storage_get_fru_info, NETFN_STORAGE, IPMI_GET_FRU_INVENTORY_AREA_INFO_CMD, ipmi_msg * req, ipmi_msg * rsp )
{
    uint8_t len = rsp->data_len = 0;

    uint8_t fru_id = req->data[0];

    rsp->completion_code = IPMI_CC_OK;

    if (fru_id == 0) {
        rsp->data[len++] = FRU_INFO_SIZE & 0xFF;
        rsp->data[len++] = FRU_INFO_SIZE & 0xFF00;
        rsp->data[len++] = 0x00; /* Device accessed by bytes */
    } else if (fru_id == 1) {
        /* RTM FRU - Not implemented yet */
    }

    rsp->data_len = len;
}

IPMI_HANDLER(ipmi_storage_read_fru_data_cmd, NETFN_STORAGE, IPMI_READ_FRU_DATA_CMD, ipmi_msg * req, ipmi_msg * rsp )
{
    uint32_t offset;
    uint8_t len = rsp->data_len = 0;
    //uint8_t fru_id = req->data[0];

    /* Count byte on the request is "1" based */
    uint8_t count = req->data[3];

    if ( (count-1) > IPMI_MSG_MAX_LENGTH ) {
        rsp->completion_code = IPMI_CC_CANT_RET_NUM_REQ_BYTES;
        return;
    }

    offset = (req->data[2] << 8) | (req->data[1]);


    count = fru_read( &(rsp->data[len]), offset, count );

    rsp->data[len++] = count;
    rsp->data_len = len + count;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_storage_write_fru_data_cmd, NETFN_STORAGE, IPMI_WRITE_FRU_DATA_CMD, ipmi_msg * req, ipmi_msg * rsp )
{
    uint8_t len = rsp->data_len = 0;
    uint16_t offset =  (req->data[2] << 8) | (req->data[1]);
    uint8_t count;

    rsp->completion_code = IPMI_CC_OK;

    /* Use signed comparison here in case offset + data_len < 3 */
    if ((offset + req->data_len - 3) < (int16_t)FRU_INFO_SIZE) {
        /* Write data to the FRU */
        count = fru_write( &req->data[3], offset, req->data_len - 3);

        /* Count written (1 based) */
        rsp->data[len++] = count +1;
    } else {
        /* Count written (1 based) */
        rsp->data[len++] = 0;
        rsp->completion_code = IPMI_CC_PARAM_OUT_OF_RANGE;
    }
    rsp->data_len = len;
}
