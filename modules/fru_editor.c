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

/*! @file fru.c
 * Creation of FRU information binary
 * Based on Julian Mendez implementation for CERN MMC
 */

#include <stdlib.h>
#include <string.h>

#include "fru_editor.h"
#include "uart_debug.h"
#include "utils.h"

uint8_t fru_header_build( uint8_t **buffer, size_t int_use_off, size_t chassis_off, size_t board_off, size_t product_off, size_t multirecord_off )
{
    uint8_t len = sizeof(fru_common_header_t);
    uint8_t *hdr_ptr;

    /* Allocate the needed memory region */
    hdr_ptr = pvPortMalloc(len);

    memset(hdr_ptr, 0x00 , len);

    fru_common_header_t * hdr = (fru_common_header_t *) hdr_ptr;

    hdr->format_version = 1;

    hdr->int_use_offset = int_use_off/8;
    hdr->chassis_info_offset = chassis_off/8;
    hdr->board_info_offset = board_off/8;
    hdr->product_info_offset = product_off/8;
    hdr->multirecord_offset = multirecord_off/8;

    hdr->checksum = calculate_chksum((uint8_t *) hdr, sizeof(fru_common_header_t));

    *buffer = &hdr_ptr[0];

    return len;
}

uint8_t chassis_info_area_build( uint8_t **buffer, uint8_t type, const char *pn, const char *sn, uint8_t *custom_data, size_t custom_data_sz )
{
    uint8_t i = 0;
    uint8_t len = 5 + strlen(pn) + strlen(sn) + 2 + custom_data_sz;
    uint8_t *chassis_ptr;

    /* Pad the size to align the region */
    while (len%8) {
        len++;
    }

    /* Allocate the needed memory region */
    chassis_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(chassis_ptr, 0x00, len);

    /* Record Format version */
    chassis_ptr[i++] = 0x01;

    /* Record Length */
    chassis_ptr[i++] = len/8;

    /* Chassis type (enumeration) */
    chassis_ptr[i++] = type;

    /* Board Part Number */
    chassis_ptr[i++] = (0x03 << 6) | ((strlen(pn)+1) & 0x3F);
    strcpy((char *) &chassis_ptr[i], pn);
    i += strlen(pn) + 1;

    /* Board Serial Number */
    chassis_ptr[i++] = (0x03 << 6) | ((strlen(sn)+1) & 0x3F);
    strcpy((char *) &chassis_ptr[i], sn);
    i += strlen(sn) + 1;

    /* Custom Data */
    chassis_ptr[i++] = (0x03 << 6) | (custom_data_sz & 0x3F);
    memcpy( &chassis_ptr[i], custom_data, custom_data_sz);
    i += custom_data_sz;

    /* No more info fields (End of record) */
    chassis_ptr[i++] = 0xC1;

    /* Checksum */
    chassis_ptr[len-1] = calculate_chksum( (uint8_t *)chassis_ptr, len );

    *buffer = &chassis_ptr[0];

    return len;
}

uint8_t board_info_area_build( uint8_t **buffer, uint8_t lang, uint32_t mfg_time, const char *manuf, const char *name, const char *sn, const char *pn, const char *file_id )
{
    uint8_t i = 0;
    uint8_t len = 13 + strlen(manuf) + strlen(name) + strlen(sn) + strlen(pn) + strlen(file_id) + 5;
    uint8_t *board_ptr;

    /* Pad the size to align the region */
    while (len%8) {
        len++;
    }

    /* Allocate the needed memory region */
    board_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(board_ptr, 0x00, len);

    /* Record Format version */
    board_ptr[i++] = 0x01;

    /* Record Length */
    board_ptr[i++] = len/8;

    /* Language Code */
    board_ptr[i++] = lang;

    /* Manufacturing date/time - LS Byte first */
    board_ptr[i++] = (mfg_time & 0x000000FF) >> (0);
    board_ptr[i++] = (mfg_time & 0x0000FF00) >> (8);
    board_ptr[i++] = (mfg_time & 0x00FF0000) >> (16);

    /* Board Manufacturer */
    board_ptr[i++] = (0x03 << 6) | ((strlen(manuf)+1) & 0x3F);
    strcpy((char *) &board_ptr[i], manuf);
    i += strlen(manuf) + 1;

    /* Board Product Name */
    board_ptr[i++] = (0x03 << 6) | ((strlen(name)+1) & 0x3F);
    strcpy((char *) &board_ptr[i], name);
    i += strlen(name) + 1;

    /* Board Serial Number */
    board_ptr[i++] = (0x03 << 6) | ((strlen(sn)+1) & 0x3F);
    strcpy((char *) &board_ptr[i], sn);
    i += strlen(sn) + 1;

    /* Board Part Number */
    board_ptr[i++] = (0x03 << 6) | ((strlen(pn)+1) & 0x3F);
    strcpy((char *) &board_ptr[i], pn);
    i += strlen(pn) + 1;

    /* FRU File ID */
    board_ptr[i++] = (0x03 << 6) | ((strlen(file_id)+1) & 0x3F);
    strcpy((char *) &board_ptr[i], file_id);
    i += strlen(file_id) + 1;

    /* No more info fields (End of record) */
    board_ptr[i++] = 0xC1;

    /* Checksum */
    board_ptr[len-1] = calculate_chksum( (uint8_t *)board_ptr, len );

    *buffer = &board_ptr[0];

    return len;
}

uint8_t product_info_area_build( uint8_t **buffer, uint8_t lang, const char *manuf, const char *name, const char *part_model, const char *version, const char *serial, const char *asset_tag, const char *file_id )
{
    uint8_t i = 0;
    uint8_t len = 11 + strlen(manuf) + strlen(name) + strlen(part_model) + strlen(version) + strlen(serial) + strlen(asset_tag) + strlen(file_id) + 7;
    uint8_t *product_ptr;

    /* Pad the size to align the region */
    while (len%8) {
        len++;
    }

    /* Allocate the needed memory region */
    product_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(product_ptr, 0x00, len);

    /* Format Version */
    product_ptr[i++] = 1;

    /* Record size in 8 bytes */
    product_ptr[i++] = len/8;

    /* Language code */
    product_ptr[i++] = lang;

    /* Product Manufacturer */
    product_ptr[i++] = (0x03 << 6) | ((strlen(manuf)+1) & 0x3F);
    strcpy((char *) &product_ptr[i], manuf);
    i += strlen(manuf) + 1;

    /* Product Name */
    product_ptr[i++] = (0x03 << 6) | ((strlen(name)+1) & 0x3F);
    strcpy((char *) &product_ptr[i], name);
    i += strlen(name) + 1;

    /* Product Part/Model Number type/length */
    product_ptr[i++] = (0x03 << 6) | ((strlen(part_model)+1) & 0x3F);
    strcpy((char *) &product_ptr[i], part_model);
    i += strlen(part_model) + 1;

    /* Product Version */
    product_ptr[i++] = (0x03 << 6) | ((strlen(version)+1) & 0x3F);
    strcpy((char *) &product_ptr[i], version);
    i += strlen(version) + 1;

    /* Product Serial Num */
    product_ptr[i++] = (0x03 << 6) | ((strlen(serial)+1) & 0x3F);
    strcpy((char *) &product_ptr[i], serial);
    i += strlen(serial) + 1;

    /* Product Asset Tag */
    product_ptr[i++] = (0x03 << 6) | ((strlen(asset_tag)+1) & 0x3F);
    strcpy((char *) &product_ptr[i], asset_tag);
    i += strlen(asset_tag) + 1;

    /* FRU File ID */
    product_ptr[i++] = (0x03 << 6) | ((strlen(file_id)+1) & 0x3F);
    strcpy((char *) &product_ptr[i], file_id);
    i += strlen(file_id) + 1;

    product_ptr[i++] = 0xC1;

    /* Checksum */
    product_ptr[len-1] = calculate_chksum( product_ptr, len );

    *buffer = &product_ptr[0];

    return len;
}

uint8_t module_current_record_build( uint8_t **buffer, uint8_t current )
{
    uint8_t len = sizeof(fru_module_current_record_t);
    uint8_t *current_ptr;

    /* Allocate the needed memory region */
    current_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(current_ptr, 0x00, len);

    fru_module_current_record_t *module_current = (fru_module_current_record_t *) current_ptr;

    /* Record Type ID */
    module_current->hdr.record_type_id = 0xC0;    //OEM Record
    module_current->hdr.eol = 0x00;    //End of Records
    module_current->hdr.version = 0x02;    //Record format version
    module_current->hdr.record_len = len-sizeof(fru_multirecord_area_header_t);        //Record length
    module_current->manuf_id[0] = 0x5A;
    module_current->manuf_id[1] = 0x31;
    module_current->manuf_id[2] = 0x00;   //Manufacturer ID (PICMG)
    module_current->picmg_rec_id = 0x16;    //PICMG Record ID (19h - AMC Point to point connectivity)
    module_current->rec_fmt_ver = 0x00;    //Record format version (0x00 for this version)
    /* Current Draw */
    module_current->current = current;
    /* Checksums */

    /* Record Checksum */
    module_current->hdr.record_chksum = calculate_chksum( ((uint8_t *)module_current)+sizeof(fru_multirecord_area_header_t), module_current->hdr.record_len);
    /* Header Checksum */
    module_current->hdr.header_chksum = calculate_chksum( (uint8_t *)&(module_current->hdr), sizeof(fru_multirecord_area_header_t));

    *buffer = &current_ptr[0];

    return len;
}

uint8_t amc_point_to_point_record_build( uint8_t **buffer, amc_p2p_descriptor_t * p2p_desc, uint8_t desc_count )
{
    uint8_t len = sizeof(amc_point_to_point_record_t) + sizeof(amc_p2p_descriptor_t)*desc_count;
    uint8_t *p2p_ptr;

    /* Allocate the needed memory region */
    p2p_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(p2p_ptr, 0x00, len);

    amc_point_to_point_record_t * p2p_record = (amc_point_to_point_record_t *) p2p_ptr;

    p2p_record->hdr.record_type_id = 0xC0;    //OEM Record
    p2p_record->hdr.eol = 0x00;    //Record format version
    p2p_record->hdr.version = 0x02;    //Record format version
    p2p_record->hdr.record_len = len-sizeof(fru_multirecord_area_header_t);

    p2p_record->manuf_id[0] = 0x5A;
    p2p_record->manuf_id[1] = 0x31;
    p2p_record->manuf_id[2] = 0x00;   //Manufacturer ID (PICMG)
    p2p_record->picmg_rec_id = 0x19;    //PICMG Record ID (19h - AMC Point to point connectivity)
    p2p_record->rec_fmt_ver = 0x00;    //Record format version (0x00 for this version)

    p2p_record->oem_guid_cnt = 0;     //OEM Guid cnt -> Not implemented

    p2p_record->record_type = 0x01;    //AMC Module

    p2p_record->amc_channel_descriptor_cnt = desc_count;
    /* Copy the descriptor list */
    memcpy(&p2p_record->p2p_descriptor, p2p_desc, sizeof(amc_p2p_descriptor_t)*desc_count);

    /* Record Checksum */
    p2p_record->hdr.record_chksum = calculate_chksum( ((uint8_t *)p2p_record)+sizeof(fru_multirecord_area_header_t), p2p_record->hdr.record_len);
    /* Header Checksum */
    p2p_record->hdr.header_chksum = calculate_chksum( (uint8_t *)&(p2p_record->hdr), sizeof(fru_multirecord_area_header_t));

    *buffer = &p2p_ptr[0];

    return len;
}

uint8_t amc_point_to_point_clock_build( uint8_t **buffer, clock_config_descriptor_t * clk_desc, uint8_t desc_count )
{
    uint8_t len = sizeof(amc_clock_config_record_t) + sizeof(clock_config_descriptor_t)*desc_count;
    uint8_t *clk_ptr;

    /* Allocate the needed memory region */
    clk_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(clk_ptr, 0x00, len);

    amc_clock_config_record_t * clock_cfg = (amc_clock_config_record_t *) clk_ptr;

    clock_cfg->hdr.record_type_id = 0xC0;
    clock_cfg->hdr.eol = 0x00;
    clock_cfg->hdr.version = 0x02;
    clock_cfg->hdr.record_len = len-sizeof(fru_multirecord_area_header_t);
    clock_cfg->manuf_id[0] = 0x5A;
    clock_cfg->manuf_id[1] = 0x31;
    clock_cfg->manuf_id[2] = 0x00;
    clock_cfg->picmg_rec_id = 0x2D;
    clock_cfg->rec_fmt_ver = 0x00;

    clock_cfg->resource_id = 0xFF;

    clock_cfg->descriptor_cnt = desc_count;

    memcpy(&(clock_cfg->descriptors), clk_desc, sizeof(clock_config_descriptor_t)*desc_count);

    clock_cfg->hdr.record_chksum = calculate_chksum( ((uint8_t *)clock_cfg)+sizeof(fru_multirecord_area_header_t), clock_cfg->hdr.record_len );
    clock_cfg->hdr.header_chksum = calculate_chksum( (uint8_t *)&(clock_cfg->hdr), sizeof(fru_multirecord_area_header_t) );

    *buffer = &clk_ptr[0];

    return len;
}

uint8_t zone3_compatibility_record_build( uint8_t **buffer, uint32_t manuf_id, uint32_t compat_code )
{
    uint8_t len = sizeof(zone3_compatibility_rec_t);
    uint8_t *z3_ptr;

    /* Allocate the needed memory region */
    z3_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(z3_ptr, 0x00, len);

    zone3_compatibility_rec_t *zone3_compat = (zone3_compatibility_rec_t *) z3_ptr;

    zone3_compat->hdr.record_type_id = 0xC0;
    zone3_compat->hdr.eol = 0x01;
    zone3_compat->hdr.version = 0x02;
    zone3_compat->hdr.record_len = len-sizeof(fru_multirecord_area_header_t);
    zone3_compat->manuf_id[0] = 0x5A;
    zone3_compat->manuf_id[1] = 0x31;
    zone3_compat->manuf_id[2] = 0x00;
    zone3_compat->picmg_rec_id = 0x30;
    zone3_compat->rec_fmt_ver = 0x01;

    zone3_compat->interface_id_type = 0x03;
    zone3_compat->user_manuf_id[0] = (manuf_id >> 0) & 0xFF;
    zone3_compat->user_manuf_id[1] = (manuf_id >> 8) & 0xFF;
    zone3_compat->user_manuf_id[2] = (manuf_id >> 16) & 0xFF;

    zone3_compat->compat_designator[0] = (compat_code & 0x000000FF) >> 0;
    zone3_compat->compat_designator[1] = (compat_code & 0x0000FF00) >> 8;
    zone3_compat->compat_designator[2] = (compat_code & 0x00FF0000) >> 16;
    zone3_compat->compat_designator[3] = (compat_code & 0xFF000000) >> 24;

    zone3_compat->hdr.record_chksum = calculate_chksum( ((uint8_t *)zone3_compat)+sizeof(fru_multirecord_area_header_t), zone3_compat->hdr.record_len );
    zone3_compat->hdr.header_chksum = calculate_chksum( (uint8_t *)&(zone3_compat->hdr), sizeof(fru_multirecord_area_header_t) );

    *buffer = &z3_ptr[0];

    return len;
}

/* FMC MultiRecords */

uint8_t fmc_subtype_record_build( uint8_t **buffer, uint8_t clock_dir, uint8_t module_size, uint8_t p1_conn_size, uint8_t p2_conn_size, uint8_t p1_a_count, uint8_t p1_b_count, uint8_t p2_a_count, uint8_t p2_b_count, uint8_t p1_gbt, uint8_t p2_gbt, uint8_t eol )
{
    uint8_t len = sizeof(fmc_subtype_rec_t);
    uint8_t *fmc_ptr;

    /* Allocate the needed memory region */
    fmc_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(fmc_ptr, 0x00, len);

    fmc_subtype_rec_t *fmc_subtype = (fmc_subtype_rec_t *) fmc_ptr;

    /* Record Type ID */
    fmc_subtype->hdr.record_type_id = 0xFA;    //OEM FMC Record
    fmc_subtype->hdr.eol = eol;    //End of Records
    fmc_subtype->hdr.version = 0x02;    //Record format version
    fmc_subtype->hdr.record_len = len-sizeof(fru_multirecord_area_header_t);        //Record length
    fmc_subtype->manuf_id[0] = 0xA2;
    fmc_subtype->manuf_id[1] = 0x12;
    fmc_subtype->manuf_id[2] = 0x00;   //Manufacturer ID (PICMG)

    fmc_subtype->version = 0;
    fmc_subtype->subtype = 0;

    fmc_subtype->reserved = 0;
    fmc_subtype->clk_dir = clock_dir; /* Carrier to Mezzanine */
    fmc_subtype->p2_conn_size = p2_conn_size; /* Not fitted for single width mezzanines */
    fmc_subtype->p1_conn_size = p1_conn_size; /* HPC Connector */
    fmc_subtype->module_size = module_size; /* Single width */

    /* P1 Connector signals count */
    fmc_subtype->p1_a_signals = p1_a_count; /* 102 */
    fmc_subtype->p1_b_signals = p1_b_count; /* 38 */

    /* P2 Connector signals count */
    fmc_subtype->p2_a_signals = p2_a_count; /* 0 */
    fmc_subtype->p2_b_signals = p2_b_count; /* 0 */

    /* GBT Transceivers count */
    fmc_subtype->p1_gbt = p1_gbt; /* 4 */
    fmc_subtype->p2_gbt = p2_gbt; /* 0 */

    /* Checksums */

    /* Record Checksum */
    fmc_subtype->hdr.record_chksum = calculate_chksum( ((uint8_t *)fmc_subtype)+sizeof(fru_multirecord_area_header_t), fmc_subtype->hdr.record_len);
    /* Header Checksum */
    fmc_subtype->hdr.header_chksum = calculate_chksum( (uint8_t *)&(fmc_subtype->hdr), sizeof(fru_multirecord_area_header_t));

    *buffer = &fmc_ptr[0];

    return len;
}

uint8_t dc_load_record_build( uint8_t **buffer, uint16_t nominal_volt, uint16_t min_volt, uint16_t max_volt, uint16_t ripple_noise, uint16_t min_load, uint16_t max_load, uint8_t eol )
{
    uint8_t len = sizeof(dc_load_rec_t);
    uint8_t *dc_load_ptr;

    /* Allocate the needed memory region */
    dc_load_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(dc_load_ptr, 0x00, len);

    dc_load_rec_t *dc_load = (dc_load_rec_t *) dc_load_ptr;

    dc_load->hdr.record_type_id = 0x02;
    dc_load->hdr.eol = eol;
    dc_load->hdr.version = 0x02;
    dc_load->hdr.record_len = len-sizeof(fru_multirecord_area_header_t);

    dc_load->reserved = 0;
    dc_load->output_number = 0;
    dc_load->nominal_voltage[0] = nominal_volt & 0xFF;
    dc_load->nominal_voltage[1] = (nominal_volt >> 8) & 0xFF;

    dc_load->min_spec_volt[0] = min_volt & 0xFF;
    dc_load->min_spec_volt[1] = (min_volt >> 8) & 0xFF;
    dc_load->max_spec_volt[0] = max_volt & 0xFF;
    dc_load->max_spec_volt[1] = (max_volt >> 8) & 0xFF;

    dc_load->ripple_noise_pkpk[0] = ripple_noise & 0xFF;
    dc_load->ripple_noise_pkpk[1] = (ripple_noise >> 8) & 0xFF;

    dc_load->min_current_load[0] = min_load & 0xFF;
    dc_load->min_current_load[1] = (min_load >> 8) & 0xFF;

    dc_load->max_current_load[0] = max_load & 0xFF;
    dc_load->max_current_load[1] = (max_load >> 8) & 0xFF;

    dc_load->hdr.record_chksum = calculate_chksum( ((uint8_t *)dc_load)+sizeof(fru_multirecord_area_header_t), dc_load->hdr.record_len );
    dc_load->hdr.header_chksum = calculate_chksum( (uint8_t *)&(dc_load->hdr), sizeof(fru_multirecord_area_header_t) );

    *buffer = &dc_load_ptr[0];

    return len;
}

uint8_t dc_output_record_build( uint8_t **buffer, uint16_t nominal_volt, uint16_t neg_dev, uint16_t pos_dev, uint16_t ripple_noise, uint16_t min_draw, uint16_t max_draw, uint8_t eol )
{
    uint8_t len = sizeof(dc_output_rec_t);
    uint8_t *dc_output_ptr;

    /* Allocate the needed memory region */
    dc_output_ptr = pvPortMalloc(len);

    /* Clear the buffer */
    memset(dc_output_ptr, 0x00, len);

    dc_output_rec_t *dc_output = (dc_output_rec_t *) dc_output_ptr;

    dc_output->hdr.record_type_id = 0x02;
    dc_output->hdr.eol = eol;
    dc_output->hdr.version = 0x02;
    dc_output->hdr.record_len = len-sizeof(fru_multirecord_area_header_t);

    dc_output->reserved = 0;
    dc_output->output_number = 0;

    dc_output->nominal_voltage[0] = nominal_volt & 0xFF;
    dc_output->nominal_voltage[1] = (nominal_volt >> 8) & 0xFF;

    dc_output->max_neg_dev[0] = neg_dev & 0xFF;
    dc_output->max_neg_dev[1] = (neg_dev >> 8) & 0xFF;
    dc_output->max_pos_dev[0] = pos_dev & 0xFF;
    dc_output->max_pos_dev[1] = (pos_dev >> 8) & 0xFF;

    dc_output->ripple_noise_pkpk[0] = ripple_noise & 0xFF;
    dc_output->ripple_noise_pkpk[1] = (ripple_noise >> 8) & 0xFF;

    dc_output->min_current_draw[0] = min_draw & 0xFF;
    dc_output->min_current_draw[1] = (min_draw >> 8) & 0xFF;

    dc_output->max_current_draw[0] = max_draw & 0xFF;
    dc_output->max_current_draw[1] = (max_draw >> 8) & 0xFF;

    dc_output->hdr.record_chksum = calculate_chksum( ((uint8_t *)dc_output)+sizeof(fru_multirecord_area_header_t), dc_output->hdr.record_len );
    dc_output->hdr.header_chksum = calculate_chksum( (uint8_t *)&(dc_output->hdr), sizeof(fru_multirecord_area_header_t) );

    *buffer = &dc_output_ptr[0];

    return len;
}
