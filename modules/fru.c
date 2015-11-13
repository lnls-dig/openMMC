/*! @file fru.c
 * Creation of FRU information binary
 * Based on Julian Mendez implementation for CERN MMC
 */

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "ipmi.h"
#include "fru.h"
#include "user_fru.h"

uint8_t fru_data[FRU_SIZE];

void fru_header_function( uint8_t * fru_buffer )
{
    t_fru_common_header * hdr = (t_fru_common_header *) fru_buffer;

    hdr->format_version = 0x01;
#ifdef INTERNAL_USE_AREA_ENABLE
    hdr->int_use_offset = INTERNAL_USE_AREA_OFFSET/8;
#endif
#ifdef CHASSIS_INFO_AREA_ENABLE
    hdr->chassis_info_offset = CHASSIS_INFO_AREA_OFFSET/8;
#endif
#ifdef BOARD_INFO_AREA_ENABLE
    hdr->board_info_offset = BOARD_INFO_AREA_OFFSET/8;
#endif
#ifdef PRODUCT_INFO_AREA_ENABLE
    hdr->product_info_offset = PRODUCT_INFO_AREA_OFFSET/8;
#endif
#ifdef MULTIRECORD_AREA_ENABLE
    hdr->multirecord_offset = MULTIRECORD_AREA_OFFSET/8;
#endif
    hdr->checksum = ipmb_calculate_chksum((uint8_t *) hdr, sizeof(t_fru_common_header));
}

void board_info_area_function( uint8_t * fru_buffer )
{
#ifdef BOARD_INFO_AREA_ENABLE
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
    board_info->manuf_len = (strlen(BOARD_MANUFACTURER) & 0x3F);
#ifdef BOARD_MANUFACTURER
    strncpy((char*)&(board_info->manuf), BOARD_MANUFACTURER, strlen(BOARD_MANUFACTURER));
#endif

    /* Board Product Name */
    board_info->prod_name_type = 0x03;
    board_info->prod_name_len = (strlen(BOARD_NAME) & 0x3F);
#ifdef BOARD_NAME
    strncpy((char*) &(board_info->prod_name), BOARD_NAME, strlen(BOARD_NAME));
#endif

    /* Board Serial Number */
    board_info->ser_num_type = 0x03;
    board_info->ser_num_len = (strlen(BOARD_SN) & 0x3F);
#ifdef BOARD_SN
    strncpy((char*) &(board_info->ser_num), BOARD_SN, strlen(BOARD_SN));
#endif

    /* Board Part Number */
    board_info->part_num_type = 0x03;
    board_info->part_num_len = (strlen(BOARD_PN) & 0x3F);
#ifdef BOARD_PN
    strncpy((char*) &(board_info->part_num), BOARD_PN, strlen(BOARD_PN));
#endif

    /* FRU File ID */
    board_info->fru_file_id_type = 0x03;
    board_info->fru_file_id_len = (strlen(FRU_FILE_ID) & 0x3F);
#ifdef FRU_FILE_ID
    strncpy((char*) &(board_info->fru_file_id), FRU_FILE_ID, strlen(FRU_FILE_ID));
#endif

    /* No more info fields (End of record) */
    board_info->end_of_record = 0xC1;

    /* Checksum */
    board_ptr->checksum = ipmb_calculate_chksum((uint8_t *)board_info, BOARD_INFO_AREA_SIZE);
#endif
}

void product_info_area_function( uint8_t * fru_buffer )
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

    product_ptr->checksum = ipmb_calculate_chksum((uint8_t *)product_info, PRODUCT_INFO_AREA_SIZE);
#endif
}

void module_current_record_function( uint8_t * fru_buffer )
{
#ifdef MODULE_CURRENT_RECORD

    t_module_current_record * module_current_record = (t_module_current_record *) fru_buffer;

    /* Record Type ID */
    module_current_record->hdr.record_type_id = 0xC0;    //OEM Record
    module_current_record->hdr.eol = 0x01;    //Last Record
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
    module_current_record->hdr.record_cksum = ipmb_calculate_chksum((uint8_t*)(module_current_record+sizeof(t_multirecord_area_header)), module_current_record->hdr.record_len);
    /* Header Checksum */
    module_current_record->hdr.header_cksum = ipmb_calculate_chksum( (uint8_t *)&(module_current_record->hdr), sizeof(t_multirecord_area_header));
#endif
}

void AMC_POINT_TO_POINT_RECORD_func( uint8_t * fru_buffer )
{

#ifdef AMC_POINT_TO_POINT_RECORD_LIST

#ifndef POINT_TO_POINT_OEM_GUID_CNT
#define POINT_TO_POINT_OEM_GUID_CNT             0
#endif

    t_amc_point_to_point_record *p2p_record = (t_amc_point_to_point_record *) fru_buffer;

    p2p_record->hdr.record_type_id = 0xC0;    //OEM Record
    p2p_record->hdr.eol = 0x00;    //Record format version
    p2p_record->hdr.version = 0x02;    //Record format version
    p2p_record->hdr.record_len = sizeof(t_amc_point_to_point_record)-sizeof(t_multirecord_area_header);

    p2p_record->manuf_id[0] = 0x5A;
    p2p_record->manuf_id[1] = 0x31;
    p2p_record->manuf_id[2] = 0x00;   //Manufacturer ID (PICMG)
    p2p_record->picmg_rec_id = 0x19;    //PICMG Record ID (19h - AMC Point to point connectivity)
    p2p_record->rec_fmt_ver = 0x00;    //Record format version (0x00 for this version)
    p2p_record->oem_guid_cnt = POINT_TO_POINT_OEM_GUID_CNT;     //OEM Guid cnt

#ifdef POINT_TO_POINT_OEM_GUID_LIST
    /* TODO: find a way to populate this list */
    POINT_TO_POINT_OEM_GUID_LIST;
#endif

    p2p_record->record_type = 0x01;    //AMC Module

    AMC_POINT_TO_POINT_RECORD_LIST;

    p2p_record->hdr.record_cksum = ipmb_calculate_chksum( (uint8_t *) (p2p_record+sizeof(t_multirecord_area_header)), p2p_record->hdr.record_len); //record checksum
    p2p_record->hdr.header_cksum = ipmb_calculate_chksum( (uint8_t *)&(p2p_record->hdr), sizeof(t_multirecord_area_header)); //Header checksum

#endif
}

void point_to_point_clock( uint8_t * fru_buffer )
{
#ifdef AMC_CLOCK_CONFIGURATION_LIST

    t_amc_clock_config_record * clock_cfg = (t_amc_clock_config_record *) fru_buffer;

    clock_cfg->hdr.record_type_id = 0xC0;
    clock_cfg->hdr.eol = 0x00;
    clock_cfg->hdr.version = 0x02;
    clock_cfg->hdr.record_len = sizeof(t_amc_clock_config_record)-sizeof(t_multirecord_area_header);
    clock_cfg->manuf_id[0] = 0x5A;
    clock_cfg->manuf_id[1] = 0x31;
    clock_cfg->manuf_id[2] = 0x00;
    clock_cfg->picmg_rec_id = 0x2D;
    clock_cfg->rec_fmt_ver = 0x00;

    clock_cfg->resource_id = 0xFF;

    clock_cfg->descriptor_cnt = AMC_CLOCK_CONFIGURATION_DESCRIPTORS_CNT;

    AMC_CLOCK_CONFIGURATION_LIST_BUILD;

    memcpy(clock_cfg->descriptor, &clock_descriptor_list, sizeof(clock_cfg->descriptor));

    clock_cfg->hdr.record_cksum = ipmb_calculate_chksum((uint8_t*)(clock_cfg+sizeof(t_multirecord_area_header)), clock_cfg->hdr.record_len);
    clock_cfg->hdr.header_cksum = ipmb_calculate_chksum((uint8_t*)&(clock_cfg->hdr), sizeof(t_multirecord_area_header));
#endif
}

void fru_init( void )
{
    /* Zero initialize the buffer */
    memset(&fru_data[0], 0, (sizeof(fru_data)/sizeof(fru_data[0])));

    /* Populate the fru_buffer */
    fru_header_function(&fru_data[COMMON_HEADER_OFFSET]);
    board_info_area_function(&fru_data[BOARD_INFO_AREA_OFFSET]);
    product_info_area_function(&fru_data[PRODUCT_INFO_AREA_OFFSET]);
    AMC_POINT_TO_POINT_RECORD_func(&fru_data[AMC_POINT_TO_POINT_RECORD_OFFSET]);
    point_to_point_clock(&fru_data[AMC_CLOCK_CONFIG_RECORD_OFFSET]);
    module_current_record_function(&fru_data[MODULE_CURRENT_RECORD_OFFSET]);
}

void fru_read_to_buffer(char *buff, int offset, int length)
{
    uint8_t i;
    uint8_t j = offset;

    for (i = 0; i<length; i++, j++ ) {
        if (j < (sizeof(fru_data)/sizeof(fru_data[0]))) {
            buff[i] = fru_data[j];
        } else {
            buff[i] = 0xFF;
        }
    }
}

void fru_read_common_header(t_fru_common_header * header) {
    fru_read_to_buffer( (char *) header, 0, sizeof(t_fru_common_header));
}

void ipmi_storage_get_fru_inventory( ipmi_msg * req, ipmi_msg * rsp )
{
    uint8_t len = rsp->data_len = 0;
    rsp->data[len++] = FRU_SIZE & 0xFF;
    rsp->data[len++] = FRU_SIZE & 0xFF00;
    rsp->data[len++] = 0x00; /* Device accessed by bytes */
    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

void ipmi_storage_read_fru_data( ipmi_msg * req, ipmi_msg * rsp )
{
    uint32_t offset;
    uint8_t len = rsp->data_len = 0;

    /* Count byte on the request is "1" based */
    uint8_t count = req->data[3];

    if ( (count-1) > IPMI_MSG_MAX_LENGTH ) {
        rsp->completion_code = IPMI_CC_CANT_RET_NUM_REQ_BYTES;
        return;
    }

    offset = (req->data[2] << 8) | (req->data[1]);

    rsp->data[len++] = count;
    fru_read_to_buffer( (char *) &(rsp->data[len]), offset, count );

    rsp->data_len = len + count;
    rsp->completion_code = IPMI_CC_OK;
}
