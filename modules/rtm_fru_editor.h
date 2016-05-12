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

#include "rtm_user_fru.h"

typedef struct {
#ifdef BF_MS_FIRST
uint8_t   :4,                     /* Common Header Format Version
                                     7:4 - reserved, write as 0000b */
        format_version:4;       /* 3:0 - format version number = 1h
                                   for this specification. */
#else
    uint8_t       format_version:4,
        :4;
#endif
    uint8_t       int_use_offset;         /* Internal Use Area Starting Offset
                                             (in multiples of 8 bytes). 00h
                                             indicates that this area is not
                                             present. */
    uint8_t       chassis_info_offset;    /* Chassis Info Area Starting
                                             Offset (in multiples of 8 bytes). 00h
                                             indicates that this area is not
                                             present. */
    uint8_t       board_info_offset;           /* Board Area Starting Offset (in
                                                  multiples of 8 bytes). 00h indicates
                                                  that this area is not present. */
    uint8_t       product_info_offset;    /* Product Info Area Starting
                                             Offset (in multiples of 8 bytes).
                                             00h indicates that this area is not
                                             present. */
    uint8_t       multirecord_offset;     /* MultiRecord Area Starting Offset
                                             (in multiples of 8 bytes). 00h
                                             indicates that this area is not
                                             present. */
    uint8_t       pad;                    /* PAD, write as 00h */
    uint8_t       checksum;               /* Common Header Checksum (zero checksum) */
} t_rtm_fru_common_header;


typedef struct {
    uint8_t       record_type_id; /* Record Type ID. For all records defined
                                     in this specification a value of C0h (OEM)
                                     shall be used. */
#ifdef BF_MS_FIRST
    uint8_t       eol:1,          /* [7:7] End of list. Set to one for the last record */
        reserved:3,     /* [6:4] Reserved, write as 0h.*/
        version:4;      /* [3:0] record format version (2h for this definition) */
#else
    uint8_t       version:4,
        reserved:3,
        eol:1;
#endif
    uint8_t       record_len;     /* Record Length. */
    uint8_t       record_cksum;   /* Record Checksum. Holds the zero checksum of
                                     the record. */
    uint8_t       header_cksum;   /* Header Checksum. Holds the zero checksum of
                                     the header. */
} t_rtm_multirecord_area_header;


typedef struct {
#ifdef BF_MS_FIRST
uint8_t   :4,                     /* Chassis Info Area Format Version
                                     7:4 - reserved, write as 0000b */
        format_version:4;       /* 3:0 - format version number = 1h
                                   for this specification. */
#else
    uint8_t       format_version:4,
        :4;
#endif

    uint8_t       len;                    /* Chassis Info Area Length (in multiples of 8 bytes) */
    uint8_t       type;                   /* Chassis Type (enumeration) */
    uint8_t       part_num_type:4,       /* Chassis Part Number type/length */
        part_num_len:4;
#ifdef RTM_CHASSIS_PN
    uint8_t       part_num[strlen(RTM_CHASSIS_PN)-1];           /*- Chassis Part Number bytes */
#endif
    uint8_t       serial_num_type:4,     /*- Chassis Serial Number type/length */
        serial_num_len:4;
#ifdef RTM_CHASSIS_SN
    uint8_t       serial_num[strlen(RTM_CHASSIS_SN)-1];         /*- Chassis Serial Number bytes */
#endif
#ifdef RTM_CHASSIS_CUSTOM_DATA
    uint8_t       custom_data[sizeof(RTM_CHASSIS_CUSTOM_DATA)-1];       /*- Custom Chassis Info fields, if any. */
    /*  Each field must be preceeded with type/length byte.
        C1h - (type/length byte encoded to indicate no more info fields).
        00h - any remaining unused space */
#endif
} rtm_fru_chassis_info_area_hdr;

typedef struct {
    rtm_fru_chassis_info_area_hdr data;
    uint8_t padding[PADDING_SIZE(rtm_fru_chassis_info_area_hdr)];
    uint8_t checksum;                /* - Chassis Info Checksum (zero checksum) */
} t_rtm_fru_chassis_info_area_hdr;

typedef struct rtm_board_area_format_hdr {
#ifdef BF_MS_FIRST
uint8_t   :4,                     /* Board Area Format Version
                                     7:4 - reserved, write as 0000b */
        format_version:4;               /* 3:0 - format version number = 1h
                                           for this specification. */
#else
    uint8_t       format_version:4,
        :4;
#endif
    uint8_t       len;                    /* Board Area Length (in multiples
                                             of 8 bytes) */
    uint8_t       lang_code;              /* Language Code */
    uint8_t       mfg_time[3];            /*  Mfg. Date / Time
                                              Number of minutes from 0:00 hrs 1/1/96.
                                              LSbyte first (little endian) */

    uint8_t        manuf_len:6,           // - Board Manufacturer type/length byte
        manuf_type:2;
#ifdef RTM_BOARD_MANUFACTURER
    uint8_t        manuf[sizeof(RTM_BOARD_MANUFACTURER)-1];               // Board Manufacturer bytes
#endif

    uint8_t        prod_name_len:6,       // Board Product Name type/length byte
        prod_name_type:2;
#ifdef RTM_BOARD_NAME
    uint8_t        prod_name[sizeof(RTM_BOARD_NAME)-1];           // Board Product Name bytes
#endif

    uint8_t        ser_num_len:6,          // Serial Number type/length byte
        ser_num_type:2;
#ifdef RTM_BOARD_SN
    uint8_t        ser_num[sizeof(RTM_BOARD_SN)-1];              // Board Serial Number bytes
#endif

    uint8_t        part_num_len:6,         // Board Part Number type/length byte
        part_num_type:2;
#ifdef RTM_BOARD_PN
    uint8_t        part_num[sizeof(RTM_BOARD_PN)-1];            // Board Part Number bytes
#endif
    uint8_t        fru_file_id_len:6,     // FRU File ID type/length byte
        fru_file_id_type:2;
#ifdef RTM_FRU_FILE_ID
    uint8_t        fru_file_id[sizeof(RTM_FRU_FILE_ID)-1];         // FRU File ID bytes
/*   The FRU File version field is a pre-defined field
     provided as a manufacturing aid for verifying the file that was used
     during manufacture or field update to load the FRU information. The
     content is manufacturer-specific. This field is also provided in the
     Product Info area. Either or both fields may be ‘null’. */
#endif
#ifdef RTM_BOARD_CUSTOM_DATA
    uint8_t        custom_data[sizeof(RTM_BOARD_CUSTOM_DATA)];        // Additional custom Mfg. Info fields.
/*   Defined by manufacturing. Each
     field must be preceded by a type/length byte
     C1h;                         - (type/length byte encoded to indicate no more info fields). */
#endif
    uint8_t        end_of_record;
} rtm_board_area_format_hdr;

typedef struct {
    rtm_board_area_format_hdr data;
    uint8_t padding[PADDING_SIZE(rtm_board_area_format_hdr)];
    uint8_t checksum;              /* Board Area Checksum (zero checksum) */
} t_rtm_board_area_format_hdr;

typedef struct rtm_product_area_format_hdr {
#ifdef BF_MS_FIRST
uint8_t   :4,                     /* Product Area Format Version
                                     7:4 - reserved, write as 0000b */
        format_version:4;               /* 3:0 - format version number = 1h
                                           for this specification. */
#else
    uint8_t       format_version:4,
        :4;
#endif
    uint8_t       len;                    /* Product Area Length (in multiples
                                             of 8 bytes) */
    uint8_t       lang_code;              /* Language Code */

    uint8_t        manuf_name_len:6,      // Manufacturer Name type/length byte
        manuf_name_type:2;
#ifdef RTM_PRODUCT_MANUFACTURER
    uint8_t        manuf_name[sizeof(RTM_PRODUCT_MANUFACTURER)-1];          // Manufacturer Name bytes
#endif

    uint8_t        prod_name_len:6,       // Product Name type/length byte
        prod_name_type:2;
#ifdef RTM_PRODUCT_NAME
    uint8_t        prod_name[sizeof(RTM_PRODUCT_NAME)-1];           // Product Name bytes
#endif

    uint8_t        prod_part_model_num_len:6,     // Product Part/Model Number type/length byte
        prod_part_model_num_type:2;
#ifdef RTM_PRODUCT_PN
    uint8_t        prod_part_model[sizeof(RTM_PRODUCT_PN)-1];     // Product Part/Model Number bytes
#endif

    uint8_t        prod_version_len:6,    // Product Version type/length byte
        prod_version_type:2;
#ifdef RTM_PRODUCT_VERSION
    uint8_t        prod_version[sizeof(RTM_PRODUCT_VERSION)-1];        // Product Version bytes
#endif

    uint8_t        prod_serial_num_len:6, // Product Serial Number type/length byte
        prod_serial_num_type:2;
#ifdef RTM_PRODUCT_SN
    uint8_t        prod_serial_num[sizeof(RTM_PRODUCT_SN)-1];     // Product Serial Number bytes
#endif

    uint8_t        asset_tag_len:6,       // Asset Tag type/length byte
        asset_tag_type:2;
#ifdef RTM_PRODUCT_ASSET_TAG
    uint8_t        asset_tag[sizeof(RTM_PRODUCT_ASSET_TAG)-1];           // Asset Tag
#endif

    uint8_t        fru_file_id_len:6,     // FRU File ID type/length byte
        fru_file_id_type:2;
#ifdef RTM_FRU_FILE_ID
    uint8_t        fru_file_id[sizeof(RTM_FRU_FILE_ID)-1];         // FRU File ID bytes.
#endif
/*   The FRU File version field is a predefined field
     provided as a manufacturing aid for verifying the file that was used
     during manufacture or field update to load the FRU information. The
     content is manufacturer-specific. This field is also provided in the
     Board Info area. Either or both fields may be ‘null’.*/
#ifdef RTM_PRODUCT_CUSTOM_DATA
    uint8_t        custom_data[sizeof(RTM_PRODUCT_CUSTOM_DATA)];        /* Custom product info area fields,
                                                                       if any (must be preceded with type/length byte) */
#endif
    uint8_t        end_of_record;

} rtm_product_area_format_hdr;

typedef struct {
    rtm_product_area_format_hdr data;
    uint8_t padding[PADDING_SIZE(rtm_product_area_format_hdr)];
    uint8_t checksum;               // Product Info Area Checksum (zero checksum)
} t_rtm_product_area_format_hdr;

typedef struct {
    t_rtm_multirecord_area_header hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                               three byte ID assigned to PICMG®. For this
                               specification, the value 12634 (00315Ah) shall
                               be used. */
    uint8_t picmg_rec_id;   /* PICMG Record ID. */
    uint8_t rec_fmt_ver;    /* Record Format Version. For this specification,
                               the value 0h shall be used. */
    uint8_t current;
} t_rtm_module_current_record;

typedef struct {
    t_rtm_multirecord_area_header hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                               three byte ID assigned to PICMG®. For this
                               specification, the value 12634 (00315Ah) shall
                               be used. */
    uint8_t picmg_rec_id;   /* PICMG Record ID. */
    uint8_t rec_fmt_ver;    /* Record Format Version. For Zone 3 descriptor the value 0x01 shall be used. */
    uint8_t interface_id_type;
    uint8_t user_manuf_id[3];
    uint8_t compat_designator[4];
} rtm_zone3_compatibility_rec_t;

/* Move to user code area */

/* FRU Format */
#define RTM_COMMON_HEADER_SIZE                      sizeof(t_rtm_fru_common_header)

#define RTM_INTERNAL_USE_AREA_SIZE                  0

#ifdef RTM_CHASSIS_INFO_AREA_ENABLE
#define RTM_CHASSIS_INFO_AREA_SIZE                  (sizeof(RTM_FRU_CHASSIS_AREA_FORMAT_HDR))
#else
#define RTM_CHASSIS_INFO_AREA_SIZE                  0
#endif

#ifdef RTM_BOARD_INFO_AREA_ENABLE
#define RTM_BOARD_INFO_AREA_SIZE                    (sizeof(t_rtm_board_area_format_hdr))
#else
#define RTM_BOARD_INFO_AREA_SIZE                    0
#endif

#ifdef RTM_PRODUCT_INFO_AREA_ENABLE
#define RTM_PRODUCT_INFO_AREA_SIZE                  (sizeof(t_rtm_product_area_format_hdr))
#else
#define RTM_PRODUCT_INFO_AREA_SIZE                  0
#endif

#ifdef RTM_MODULE_CURRENT_RECORD
#define RTM_MODULE_CURRENT_RECORD_SIZE              (sizeof(t_rtm_module_current_record))
#else
#define RTM_MODULE_CURRENT_RECORD_SIZE              0
#endif

#ifdef RTM_ZONE3_COMPATIBILITY_REC_ENABLE
#define RTM_ZONE3_COMPATIBILITY_REC_SIZE            (sizeof(rtm_zone3_compatibility_rec_t))
#else
#define RTM_ZONE3_COMPATIBILITY_REC_SIZE            0
#endif

#ifdef RTM_MULTIRECORD_AREA_ENABLE
#define RTM_MULTIRECORD_AREA_SIZE                   ( RTM_MODULE_CURRENT_RECORD_SIZE + \
                                                  RTM_ZONE3_COMPATIBILITY_REC_SIZE )
#else
#define RTM_MULTIRECORD_AREA_SIZE                   0
#endif

#define RTM_COMMON_HEADER_OFFSET                    0
#define RTM_INTERNAL_USE_AREA_OFFSET                ( RTM_COMMON_HEADER_OFFSET + RTM_COMMON_HEADER_SIZE )
#define RTM_CHASSIS_INFO_AREA_OFFSET                ( RTM_INTERNAL_USE_AREA_OFFSET + RTM_INTERNAL_USE_AREA_SIZE )
#define RTM_BOARD_INFO_AREA_OFFSET                  ( RTM_CHASSIS_INFO_AREA_OFFSET + RTM_CHASSIS_INFO_AREA_SIZE )
#define RTM_PRODUCT_INFO_AREA_OFFSET                ( RTM_BOARD_INFO_AREA_OFFSET + RTM_BOARD_INFO_AREA_SIZE )
#define RTM_MULTIRECORD_AREA_OFFSET                 ( RTM_PRODUCT_INFO_AREA_OFFSET + RTM_PRODUCT_INFO_AREA_SIZE )
#define RTM_MODULE_CURRENT_RECORD_OFFSET            ( RTM_MULTIRECORD_AREA_OFFSET )
#define RTM_ZONE3_COMPATIBILITY_REC_OFFSET          ( RTM_MODULE_CURRENT_RECORD_OFFSET + RTM_MODULE_CURRENT_RECORD_SIZE )

#define RTM_FRU_SIZE                                ( RTM_MULTIRECORD_AREA_OFFSET + RTM_MULTIRECORD_AREA_SIZE )
