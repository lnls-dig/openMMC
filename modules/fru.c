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

#include "FreeRTOS.h"

#include "port.h"
#include "fru.h"
#include "fru_editor.h"
#include "at24mac.h"
#include "eeprom_24xx64.h"
#include "utils.h"
#include "ipmi.h"
#include "i2c_mapping.h"
#include "uart_debug.h"

extern fru_data_t fru[FRU_COUNT];

void fru_init( uint8_t id )
{
    if ( id >= FRU_COUNT ) {
        return;
    }

#ifdef FRU_WRITE_EEPROM
    printf(">FRU_WRITE_EEPROM flag enabled! Building FRU info...\n");
    fru[id].fru_size = fru[id].cfg.build_f( &fru[id].buffer );

    printf(" Writing FRU info to EEPROM... \n");
    fru[id].cfg.write_f( fru[id].cfg.eeprom_id, 0x00, fru[id].buffer, fru[id].fru_size, 10 );
#endif

    /* Read FRU info Common Header */
    if ( !fru_check_integrity(id, &fru[id].fru_size) ) {
        /* Could not access the SEEPROM, create a runtime fru info */
        printf("Could not find a valid FRU information in EEPROM, building a runtime info...\n");
        fru[id].fru_size = fru[id].cfg.build_f( &fru[id].buffer );
        fru[id].runtime = true;
    }
}

uint8_t fru_check_integrity( uint8_t id, size_t *fru_size )
{
    uint8_t common_header[8] = {0};
    uint8_t chassis_off, board_off, product_off, multirec_off;
    uint8_t *rec_buff = pvPortMalloc(128);
    uint8_t rec_len = 0;
    size_t total_len = 0;

    if (fru[id].runtime) {
        memcpy( &common_header[0], &fru[id].buffer[0], 8);
    } else {
        fru[id].cfg.read_f( fru[id].cfg.eeprom_id, 0x00, &common_header[0], 8, 0 );
    }

    printf("[FRU][%s] Asserting FRU information integrity\n", id == FRU_AMC ? "AMC" : "RTM");
    total_len += 8;
    if ( !((calculate_chksum( &common_header[0], 8 ) == 0) && common_header[0] == 1) ) {
        /* Wrong checksum */
        printf("[FRU][%s] Error in COMMON HEADER checksum\n", id == FRU_AMC ? "AMC" : "RTM");
        return 0;
    }

    chassis_off = 8*common_header[2];
    board_off = 8*common_header[3];
    product_off = 8*common_header[4];
    multirec_off = 8*common_header[5];

    if (chassis_off > 0) {
        printf("[FRU][%s] Checking CHASSIS AREA record...", id == FRU_AMC ? "AMC" : "RTM");
        if (fru[id].runtime) {
            rec_len = fru[id].buffer[chassis_off+1];
        } else {
            fru[id].cfg.read_f( fru[id].cfg.eeprom_id, chassis_off+1, &rec_len, 1, 0 );
        }
        rec_len *= 8;
        if (rec_len > 0) {
            if (fru[id].runtime) {
                memcpy( &rec_buff[0], &fru[id].buffer[chassis_off], rec_len);
            } else {
                fru[id].cfg.read_f( fru[id].cfg.eeprom_id, chassis_off, &rec_buff[0], rec_len, 0 );
            }
            if ( !((calculate_chksum( &rec_buff[0], rec_len ) == 0) && rec_buff[0] == 1) ) {
                /* Wrong checksum */
                printf(" Error!\n");
                return 0;
            } else {
                printf(" Success!\n");
                total_len += rec_len;
            }
        }
    }

    if (board_off > 0) {
        printf("[FRU][%s] Checking BOARD AREA record...", id == FRU_AMC ? "AMC" : "RTM");
        if (fru[id].runtime) {
            rec_len = fru[id].buffer[board_off+1];
        } else {
            fru[id].cfg.read_f( fru[id].cfg.eeprom_id, board_off+1, &rec_len, 1, 0 );
        }
        rec_len *= 8;

        if (rec_len > 0) {
            if (fru[id].runtime) {
                memcpy( &rec_buff[0], &fru[id].buffer[board_off], rec_len);
            } else {
                fru[id].cfg.read_f( fru[id].cfg.eeprom_id, board_off, &rec_buff[0], rec_len, 0 );
            }
            if ( !((calculate_chksum( &rec_buff[0], rec_len ) == 0) && rec_buff[0] == 1) ) {
                /* Wrong checksum */
                printf(" Error!\n");
                return 0;
            } else {
                printf(" Success!\n");
                total_len += rec_len;
            }
        }
    }

    if (product_off > 0) {
        printf("[FRU][%s] Checking PRODUCT AREA record...", id == FRU_AMC ? "AMC" : "RTM");
        if (fru[id].runtime) {
            rec_len = fru[id].buffer[product_off+1];
        } else {
            fru[id].cfg.read_f( fru[id].cfg.eeprom_id, product_off+1, &rec_len, 1, 0 );
        }
        rec_len *= 8;

        if (rec_len > 0) {
            if (fru[id].runtime) {
                memcpy( &rec_buff[0], &fru[id].buffer[product_off], rec_len);
            } else {
                fru[id].cfg.read_f( fru[id].cfg.eeprom_id, product_off, &rec_buff[0], rec_len, 0 );
            }
            if ( !((calculate_chksum( &rec_buff[0], rec_len ) == 0 ) && rec_buff[0] == 1) ) {
                /* Wrong checksum */
                printf(" Error!\n");
                return 0;
            } else {
                printf(" Success!\n");
                total_len += rec_len;
            }
        }
    }

    if (multirec_off > 0) {
        uint8_t eol = 0;
        uint8_t rec_chksum;
        rec_len = 0;
        printf("[FRU][%s] Checking MULTIRECORD AREA records...\n", id == FRU_AMC ? "AMC" : "RTM");
        do {
            if (fru[id].runtime) {
                memcpy( &rec_buff[0], &fru[id].buffer[multirec_off], 5);
            } else {
                fru[id].cfg.read_f( fru[id].cfg.eeprom_id, multirec_off, &rec_buff[0], 5, 0 );
            }
            /* Calculate Multirecord header checksum */
            if ( !(calculate_chksum( &rec_buff[0], 5 ) == 0) ) {
                /* Wrong checksum */
                printf("[FRU][%s] Error in MULTIRECORD AREA HEADER integrity check!\n", id == FRU_AMC ? "AMC" : "RTM");
                return 0;
            }
            multirec_off += 5;
            total_len += 5;

            rec_chksum = rec_buff[3];
            rec_len = rec_buff[2];
            eol = rec_buff[1] & (1 << 7);
            if (rec_len > 0) {
                if (fru[id].runtime) {
                    memcpy( &rec_buff[0], &fru[id].buffer[multirec_off], rec_len );
                } else {
                    fru[id].cfg.read_f( fru[id].cfg.eeprom_id, multirec_off, &rec_buff[0], rec_len, 0 );
                }
                if ( !((calculate_chksum( &rec_buff[0], rec_len ) == rec_chksum)) ) {
                    /* Wrong checksum */
                    printf("[FRU][%s] Error in MULTIRECORD AREA integrity check!\n", id == FRU_AMC ? "AMC" : "RTM");
                    return 0;
                } else {
                    total_len += rec_len;
                    multirec_off += rec_len;
                }
            }
        } while(eol == 0);
    }

    if (fru_size) {
        *fru_size = total_len;
    }

    printf("[FRU][%s] FRU info is healthy!\n", id == FRU_AMC ? "AMC" : "RTM");

    vPortFree(rec_buff);
    return 1;
}

size_t fru_read( uint8_t id, uint8_t *rx_buff, uint16_t offset, size_t len )
{
    uint16_t i;
    uint16_t j = offset;

    size_t ret_val = 0;

    if ( id >= FRU_COUNT ) {
        return 0;
    }

    /*
     * Read runtime FRU info that is auto-generated
     * when there is no valid FRU info in the EEPROM
     */
    if ( fru[id].runtime ) {
        for ( i = 0; i < len; i++, j++ ) {
            if ( j < fru[id].fru_size ) {
                rx_buff[i] = fru[id].buffer[j];
            } else {
                rx_buff[i] = 0xFF;
            }
        }
        ret_val = i;

    /*
     *  Read EEPROM FRU info
     */
    } else {
        ret_val = fru[id].cfg.read_f( fru[id].cfg.eeprom_id, offset, rx_buff, len, 10 );
    }
    return ret_val;
}

size_t fru_write( uint8_t id, uint8_t *tx_buff, uint16_t offset, size_t len )
{
    size_t ret_val = 0;

    if ( id >= FRU_COUNT ) {
        return 0;
    }

    if ( fru[id].runtime ) {
        memcpy( &fru[id].buffer[offset], tx_buff, len );
        ret_val = len;
    } else {
        ret_val = fru[id].cfg.write_f( fru[id].cfg.eeprom_id, offset, tx_buff, len, 0 );
    }
    return ret_val;
}

/* IPMI Handlers */
IPMI_HANDLER(ipmi_storage_get_fru_info, NETFN_STORAGE, IPMI_GET_FRU_INVENTORY_AREA_INFO_CMD, ipmi_msg * req, ipmi_msg * rsp )
{
    uint8_t len = rsp->data_len = 0;

    uint8_t id = req->data[0];

    if ( id < FRU_COUNT ) {
        rsp->data[len++] = fru[id].fru_size & 0xFF;
        rsp->data[len++] = (fru[id].fru_size & 0xFF00) >> 8;
        rsp->data[len++] = 0x00; /* Device accessed by bytes */

        rsp->completion_code = IPMI_CC_OK;
    } else {
        rsp->completion_code = IPMI_CC_INV_DATA_FIELD_IN_REQ;
    }

    rsp->data_len = len;
}

IPMI_HANDLER(ipmi_storage_read_fru_data_cmd, NETFN_STORAGE, IPMI_READ_FRU_DATA_CMD, ipmi_msg * req, ipmi_msg * rsp )
{
    uint32_t offset;
    uint8_t len = rsp->data_len = 0;
    uint8_t fru_id = req->data[0];

    /* Count byte on the request is "1" based */
    uint8_t count = req->data[3];

    /* Consider header, count, data and checksum */
    if ( (count + IPMB_RESP_HEADER_LENGTH + 2) > IPMI_MSG_MAX_LENGTH ) {
        rsp->completion_code = IPMI_CC_CANT_RET_NUM_REQ_BYTES;
        return;
    }

    offset = (req->data[2] << 8) | (req->data[1]);

    count = fru_read( fru_id, &(rsp->data[len+1]), offset, count );

    /*
     *  If count == 0, it may indicate that the fru_read function
     *  failed somehow.
     */

    if (count == 0) {
        rsp->completion_code = IPMI_CC_UNSPECIFIED_ERROR;
        return ;
    }

    rsp->data[len++] = count;

    rsp->data_len = len + count;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_storage_write_fru_data_cmd, NETFN_STORAGE, IPMI_WRITE_FRU_DATA_CMD, ipmi_msg * req, ipmi_msg * rsp )
{
    uint8_t len = rsp->data_len = 0;
    uint16_t offset =  (req->data[2] << 8) | (req->data[1]);
    uint8_t id = req->data[0];
    uint8_t count;

    rsp->completion_code = IPMI_CC_OK;

    /* Use signed comparison here in case offset + data_len < 3 */
    if ((offset + req->data_len - 3) < (int16_t)fru[id].fru_size) {
        /* Write data to the FRU */
        count = fru_write( id, &req->data[3], offset, req->data_len - 3);

        /* Count written (1 based) */
        rsp->data[len++] = count +1;
    } else {
        /* Count written (1 based) */
        rsp->data[len++] = 0;
        rsp->completion_code = IPMI_CC_PARAM_OUT_OF_RANGE;
    }
    rsp->data_len = len;
}
