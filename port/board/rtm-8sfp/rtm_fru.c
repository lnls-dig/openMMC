/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Julian Mendez  <julian.mendez@cern.ch>
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

/*! @file rtm_fru.c
 * Creation of RTM FRU information binary
 * Based on Julian Mendez implementation for CERN MMC
 */

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "utils.h"
#include "rtm_fru.h"

void rtm_fru_header_build( uint8_t * fru_buffer )
{
    t_rtm_fru_common_header * hdr = (t_rtm_fru_common_header *) fru_buffer;

    hdr->format_version = 0x01;
#ifdef RTM_INTERNAL_USE_AREA_ENABLE
    hdr->int_use_offset = INTERNAL_USE_AREA_OFFSET/8;
#endif
#ifdef RTM_CHASSIS_INFO_AREA_ENABLE
    hdr->chassis_info_offset = CHASSIS_INFO_AREA_OFFSET/8;
#endif
#ifdef RTM_BOARD_INFO_AREA_ENABLE
    hdr->board_info_offset = BOARD_INFO_AREA_OFFSET/8;
#endif
#ifdef RTM_PRODUCT_INFO_AREA_ENABLE
    hdr->product_info_offset = PRODUCT_INFO_AREA_OFFSET/8;
#endif
#ifdef RTM_MULTIRECORD_AREA_ENABLE
    hdr->multirecord_offset = MULTIRECORD_AREA_OFFSET/8;
#endif
    hdr->checksum = calculate_chksum((uint8_t *) hdr, sizeof(t_rtm_fru_common_header));
}

void rtm_board_info_area_build( uint8_t * fru_buffer )
{
#ifdef RTM_BOARD_INFO_AREA_ENABLE
    t_board_area_format_hdr * board_ptr = (t_board_area_format_hdr *) fru_buffer;
    board_area_format_hdr * board_info = &(board_ptr->data);
    /* Record Format version */
    board_info->format_version = 0x01;

    /* Record Length */
    board_info->len = BOARD_INFO_AREA_SIZE/8;

    /* Language Code */
    board_info->lang_code = LANG_CODE;

    /* Manufacturing date/time - LS Byte first */
    board_info->mfg_time[0] = 0x00;
    board_info->mfg_time[1] = 0x00;
    board_info->mfg_time[2] = 0x00;

    /* Board Manufacturer */
    board_info->manuf_type = 0x03;
    board_info->manuf_len = (strlen(RTM_BOARD_MANUFACTURER) & 0x3F);
#ifdef RTM_BOARD_MANUFACTURER
    strncpy((char*)&(board_info->manuf), RTM_BOARD_MANUFACTURER, strlen(RTM_BOARD_MANUFACTURER));
#endif

    /* Board Product Name */
    board_info->prod_name_type = 0x03;
    board_info->prod_name_len = (strlen(RTM_BOARD_NAME) & 0x3F);
#ifdef RTM_BOARD_NAME
    strncpy((char*) &(board_info->prod_name), RTM_BOARD_NAME, strlen(RTM_BOARD_NAME));
#endif

    /* Board Serial Number */
    board_info->ser_num_type = 0x03;
    board_info->ser_num_len = (strlen(RTM_BOARD_SN) & 0x3F);
#ifdef RTM_BOARD_SN
    strncpy((char*) &(board_info->ser_num), RTM_BOARD_SN, strlen(RTM_BOARD_SN));
#endif

    /* Board Part Number */
    board_info->part_num_type = 0x03;
    board_info->part_num_len = (strlen(RTM_BOARD_PN) & 0x3F);
#ifdef RTM_BOARD_PN
    strncpy((char*) &(board_info->part_num), RTM_BOARD_PN, strlen(RTM_BOARD_PN));
#endif

    /* FRU File ID */
    board_info->fru_file_id_type = 0x03;
    board_info->fru_file_id_len = (strlen(RTM_FRU_FILE_ID) & 0x3F);
#ifdef RTM_FRU_FILE_ID
    strncpy((char*) &(board_info->fru_file_id), RTM_FRU_FILE_ID, strlen(RTM_FRU_FILE_ID));
#endif

    /* No more info fields (End of record) */
    board_info->end_of_record = 0xC1;

    /* Checksum */
    board_ptr->checksum = calculate_chksum((uint8_t *)board_info, RTM_BOARD_INFO_AREA_SIZE);
#endif
}

void rtm_product_info_area_build( uint8_t * fru_buffer )
{
#ifdef PRODUCT_INFO_AREA_ENABLE
    t_product_area_format_hdr * product_ptr = (t_product_area_format_hdr *) fru_buffer;
    product_area_format_hdr * product_info = &(product_ptr->data);

/* Format Version */
    product_info->format_version = 1;

/* Record size in 8 bytes */
    product_info->len = PRODUCT_INFO_AREA_SIZE/8;

/* Language code */
    product_info->lang_code = LANG_CODE;

/* Product Manufacturer */
    product_info->manuf_name_type = 0x3;
    product_info->manuf_name_len = (strlen(PRODUCT_MANUFACTURER) & 0x3F);
#ifdef PRODUCT_MANUFACTURER
    strncpy((char*) &(product_info->manuf_name), PRODUCT_MANUFACTURER, strlen(PRODUCT_MANUFACTURER));
#endif

/* Product Name */
    product_info->prod_name_type = 0x03;
    product_info->prod_name_len = (strlen(PRODUCT_NAME) & 0x3F);
#ifdef PRODUCT_NAME
    strncpy((char*) &(product_info->prod_name), PRODUCT_NAME, strlen(PRODUCT_NAME));
#endif

/* Product Part/Model Number type/length */
    product_info->prod_part_model_num_type = 0x03;
    product_info->prod_part_model_num_len = (strlen(PRODUCT_PN) & 0x3F);
#ifdef PRODUCT_PN
    strncpy((char*) &(product_info->prod_part_model), PRODUCT_PN, strlen(PRODUCT_PN));
#endif

/* Product Version */
    product_info->prod_version_type = 0x03;
    product_info->prod_version_len = (strlen(PRODUCT_VERSION) & 0x3F);
#ifdef PRODUCT_VERSION
    strncpy((char*) &(product_info->prod_version), PRODUCT_VERSION, strlen(PRODUCT_VERSION));
#endif

/* Product Serial Num */
    product_info->prod_serial_num_type = 0x03;
    product_info->prod_serial_num_len = (strlen(PRODUCT_SN) & 0x3F);
#ifdef PRODUCT_SN
    strncpy((char*) &(product_info->prod_serial_num), PRODUCT_SN, strlen(PRODUCT_SN));
#endif

/* Product Serial Num */
    product_info->asset_tag_type = 0x03;
    product_info->asset_tag_len = (strlen(PRODUCT_ASSET_TAG) & 0x3F);
#ifdef PRODUCT_ASSET_TAG
    strncpy((char*) &(product_info->asset_tag), PRODUCT_ASSET_TAG, strlen(PRODUCT_ASSET_TAG));
#endif

/* FRU File ID */
    product_info->fru_file_id_type = 0x03;
    product_info->fru_file_id_len = (strlen(FRU_FILE_ID) & 0x3F);
#ifdef FRU_FILE_ID
    strncpy((char*) &(product_info->fru_file_id), FRU_FILE_ID, strlen(FRU_FILE_ID));
#endif

    product_info->end_of_record = 0xC1;

    product_ptr->checksum = calculate_chksum((uint8_t *)product_info, PRODUCT_INFO_AREA_SIZE);
#endif
}

void rtm_module_current_record_build( uint8_t * fru_buffer )
{
#ifdef MODULE_CURRENT_RECORD

    t_module_current_record * module_current_record = (t_module_current_record *) fru_buffer;

    /* Record Type ID */
    module_current_record->hdr.record_type_id = 0xC0;    //OEM Record
    module_current_record->hdr.eol = 0x00;    //Last Record
    module_current_record->hdr.version = 0x02;    //Record format version
    module_current_record->hdr.record_len = sizeof(t_module_current_record)-sizeof(t_multirecord_area_header);        //Record length
    module_current_record->manuf_id[0] = 0x5A;
    module_current_record->manuf_id[1] = 0x31;
    module_current_record->manuf_id[2] = 0x00;   //Manufacturer ID (PICMG)
    module_current_record->picmg_rec_id = 0x16;    //PICMG Record ID (19h - AMC Point to point connectivity)
    module_current_record->rec_fmt_ver = 0x00;    //Record format version (0x00 for this version)
    /* Current Draw */
    module_current_record->current = MODULE_CURRENT_RECORD;
    /* Checksums */

    /* Record Checksum */
    module_current_record->hdr.record_cksum = calculate_chksum( ((uint8_t *)module_current_record)+sizeof(t_multirecord_area_header), module_current_record->hdr.record_len);
    /* Header Checksum */
    module_current_record->hdr.header_cksum = calculate_chksum( (uint8_t *)&(module_current_record->hdr), sizeof(t_multirecord_area_header));
#endif
}

void rtm_zone3_compatibility_record_build( uint8_t * fru_buffer )
{
#ifdef ZONE3_COMPATIBILITY_REC_ENABLE
    zone3_compatibility_rec_t * zone3_compat = (zone3_compatibility_rec_t *) fru_buffer;

    zone3_compat->hdr.record_type_id = 0xC0;
    zone3_compat->hdr.eol = 0x01;
    zone3_compat->hdr.version = 0x02;
    zone3_compat->hdr.record_len = sizeof(zone3_compatibility_rec_t)-sizeof(t_multirecord_area_header);
    zone3_compat->manuf_id[0] = 0x5A;
    zone3_compat->manuf_id[1] = 0x31;
    zone3_compat->manuf_id[2] = 0x00;
    zone3_compat->picmg_rec_id = 0x30;
    zone3_compat->rec_fmt_ver = 0x01;

    zone3_compat->interface_id_type = 0x03;
    zone3_compat->user_manuf_id[0] = 0x00;
    zone3_compat->user_manuf_id[1] = 0x00;
    zone3_compat->user_manuf_id[2] = 0x00;

    zone3_compat->compat_designator[0] = (COMPATIBILITY_CODE & 0x000000FF) >> 0;
    zone3_compat->compat_designator[1] = (COMPATIBILITY_CODE & 0x0000FF00) >> 8;
    zone3_compat->compat_designator[2] = (COMPATIBILITY_CODE & 0x00FF0000) >> 16;
    zone3_compat->compat_designator[3] = (COMPATIBILITY_CODE & 0xFF000000) >> 24;

    zone3_compat->hdr.record_cksum = calculate_chksum( ((uint8_t *)zone3_compat)+sizeof(zone3_compatibility_rec_t), zone3_compat->hdr.record_len );
    zone3_compat->hdr.header_cksum = calculate_chksum( (uint8_t *)&(zone3_compat->hdr), sizeof(t_multirecord_area_header) );
#endif
}


void rtm_fru_init( void )
{
    /* Zero initialize the buffer */
    memset(&rtm_fru_data[0], 0, (sizeof(rtm_fru_data)/sizeof(rtm_fru_data[0])));

    /* Populate the fru_buffer */
    rtm_fru_header_build(&rtm_fru_data[RTM_COMMON_HEADER_OFFSET]);
    rtm_board_info_area_build(&rtm_fru_data[RTM_BOARD_INFO_AREA_OFFSET]);
    rtm_product_info_area_build(&rtm_fru_data[RTM_PRODUCT_INFO_AREA_OFFSET]);
    rtm_module_current_record_build(&rtm_fru_data[RTM_MODULE_CURRENT_RECORD_OFFSET]);
    rtm_zone3_compatibility_record_build(&fru_data[RTM_ZONE3_COMPATIBILITY_REC_OFFSET]);
}
