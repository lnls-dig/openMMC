/*
 * fru.h
 *
 */
#ifndef FRU_H
#define FRU_H

#include "FreeRTOS.h"
#include "string.h"
#include "user_fru.h"
#include "ipmi.h"

#define MAX_FRU_SIZE            2048

/** E-Keying */
//Link type
#define PCIE                    (0x02)
#define PCIE_ADV_SWITCHING      (0x03)
#define ETHERNET                (0x05)
#define SERIAL_RAPID_IO         (0x06)
#define STORAGE                 (0x07)

//Link type extension
#define NO_EXT                  (0x00)

//PCIE ext
#define GEN1_NO_SSC             (0x00)
#define GEN1_SSC                (0x01)
#define GEN2_NO_SSC             (0x02)
#define GEN2_SSC                (0x03)

//Ethernet ext
#define BASE_1G_BX              (0x00)
#define BASE_10G_BX4            (0x01)

//Serial rapid io ext
#define MBAUD_1250              (0x00)
#define MBAUD_2500              (0x01)
#define MBAUD_3125              (0x02)
#define MBAUD_5000              (0x03)
#define MBAUD_6250              (0x04)

/** Clock configuration */
//Clocks ID
#define TCLKA                   (1)
#define TCLKB                   (2)
#define TCLKC                   (3)
#define TCLKD                   (4)
#define FCLKA                   (5)

//PLL
#define PLL                     (0x02)
#define NO_PLL                  (0x00)

//Activation
#define APP                     (0x01)
#define CIPMC                   (0x00)

//Direction
#define RECEIVER                (0x00)
#define SOURCE                  (0x01)

//Family
#define UNSPEC_FAMILY           (0x00)
#define SONET                   (0x01)
#define SDH                     (0x01)
#define PDH                     (0x01)
#define PCI_RESERVED            (0x02)

//Conversion
#define KHz(v)                  (v*1000)
#define MHz(v)                  (v*1000000)
#define GHz(v)                  (v*1000000000)

//Matching
#define EXACT_MATCHES           (0x00)
#define MATCHES_01              (0x01)
#define MATCHES_10              (0x02)

#define PORT(n)                 n

#define current_in_ma(curr)                     (uint8_t)(curr/100);
#define PADDING_SIZE(x) (7-(sizeof(x)%8))

typedef struct fru_common_header {
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
} t_fru_common_header;


typedef struct multirecord_area_header {
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
} t_multirecord_area_header;


typedef struct fru_internal_use_area {
#ifdef BF_MS_FIRST
uint8_t   :4,                     /* Internal Use Format Version
                                     7:4 - reserved, write as 0000b */
        format_version:4;       /* 3:0 - format version number = 1h
                                   for this specification. */
#else
    uint8_t       format_version:4,
        :4;
#endif
    uint8_t       data;                   /* Internal use data - One or more bytes
                                             defined and formatted as needed for
                                             management controller or other device
                                             that owns/uses this area. */
} FRU_INTERNAL_USE_AREA;

typedef struct fru_chassis_info_area_hdr{
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
#ifdef CHASSIS_PN
    uint8_t       part_num[strlen(CHASSIS_PN)-1];           /*- Chassis Part Number bytes */
#endif
    uint8_t       serial_num_type:4,     /*- Chassis Serial Number type/length */
        serial_num_len:4;
#ifdef CHASSIS_SN
    uint8_t       serial_num[strlen(CHASSIS_SN)-1];         /*- Chassis Serial Number bytes */
#endif
#ifdef CHASSIS_CUSTOM_DATA
    uint8_t       custom_data[sizeof(CHASSIS_CUSTOM_DATA)-1];       /*- Custom Chassis Info fields, if any. */
    /*  Each field must be preceeded with type/length byte.
        C1h - (type/length byte encoded to indicate no more info fields).
        00h - any remaining unused space */
#endif
} fru_chassis_info_area_hdr;

typedef struct {
    fru_chassis_info_area_hdr data;
    uint8_t padding[PADDING_SIZE(fru_chassis_info_area_hdr)];
    uint8_t checksum;                /* - Chassis Info Checksum (zero checksum) */
} t_fru_chassis_info_area_hdr;

typedef struct board_area_format_hdr {
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
#ifdef BOARD_MANUFACTURER
    uint8_t        manuf[sizeof(BOARD_MANUFACTURER)-1];               // Board Manufacturer bytes
#endif

    uint8_t        prod_name_len:6,       // Board Product Name type/length byte
        prod_name_type:2;
#ifdef BOARD_NAME
    uint8_t        prod_name[sizeof(BOARD_NAME)-1];           // Board Product Name bytes
#endif

    uint8_t        ser_num_len:6,          // Serial Number type/length byte
        ser_num_type:2;
#ifdef BOARD_SN
    uint8_t        ser_num[sizeof(BOARD_SN)-1];              // Board Serial Number bytes
#endif

    uint8_t        part_num_len:6,         // Board Part Number type/length byte
        part_num_type:2;
#ifdef BOARD_PN
    uint8_t        part_num[sizeof(BOARD_PN)-1];            // Board Part Number bytes
#endif
    uint8_t        fru_file_id_len:6,     // FRU File ID type/length byte
        fru_file_id_type:2;
#ifdef FRU_FILE_ID
    uint8_t        fru_file_id[sizeof(FRU_FILE_ID)-1];         // FRU File ID bytes
/*   The FRU File version field is a pre-defined field
     provided as a manufacturing aid for verifying the file that was used
     during manufacture or field update to load the FRU information. The
     content is manufacturer-specific. This field is also provided in the
     Product Info area. Either or both fields may be ‘null’. */
#endif
#ifdef BOARD_CUSTOM_DATA
    uint8_t        custom_data[sizeof(BOARD_CUSTOM_DATA)];        // Additional custom Mfg. Info fields.
/*   Defined by manufacturing. Each
     field must be preceded by a type/length byte
     C1h;                         - (type/length byte encoded to indicate no more info fields). */
#endif
    uint8_t        end_of_record;
} board_area_format_hdr;

typedef struct {
    board_area_format_hdr data;
    uint8_t padding[PADDING_SIZE(board_area_format_hdr)];
    uint8_t checksum;              /* Board Area Checksum (zero checksum) */
} t_board_area_format_hdr;

typedef struct product_area_format_hdr {
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
#ifdef PRODUCT_MANUFACTURER
    uint8_t        manuf_name[sizeof(PRODUCT_MANUFACTURER)-1];          // Manufacturer Name bytes
#endif

    uint8_t        prod_name_len:6,       // Product Name type/length byte
        prod_name_type:2;
#ifdef PRODUCT_NAME
    uint8_t        prod_name[sizeof(PRODUCT_NAME)-1];           // Product Name bytes
#endif

    uint8_t        prod_part_model_num_len:6,     // Product Part/Model Number type/length byte
        prod_part_model_num_type:2;
#ifdef PRODUCT_PN
    uint8_t        prod_part_model[sizeof(PRODUCT_PN)-1];     // Product Part/Model Number bytes
#endif

    uint8_t        prod_version_len:6,    // Product Version type/length byte
        prod_version_type:2;
#ifdef PRODUCT_VERSION
    uint8_t        prod_version[sizeof(PRODUCT_VERSION)-1];        // Product Version bytes
#endif

    uint8_t        prod_serial_num_len:6, // Product Serial Number type/length byte
        prod_serial_num_type:2;
#ifdef PRODUCT_SN
    uint8_t        prod_serial_num[sizeof(PRODUCT_SN)-1];     // Product Serial Number bytes
#endif

    uint8_t        asset_tag_len:6,       // Asset Tag type/length byte
        asset_tag_type:2;
#ifdef PRODUCT_ASSET_TAG
    uint8_t        asset_tag[sizeof(PRODUCT_ASSET_TAG)-1];           // Asset Tag
#endif

    uint8_t        fru_file_id_len:6,     // FRU File ID type/length byte
        fru_file_id_type:2;
#ifdef FRU_FILE_ID
    uint8_t        fru_file_id[sizeof(FRU_FILE_ID)-1];         // FRU File ID bytes.
#endif
/*   The FRU File version field is a predefined field
     provided as a manufacturing aid for verifying the file that was used
     during manufacture or field update to load the FRU information. The
     content is manufacturer-specific. This field is also provided in the
     Board Info area. Either or both fields may be ‘null’.*/
#ifdef PRODUCT_CUSTOM_DATA
    uint8_t        custom_data[sizeof(PRODUCT_CUSTOM_DATA)];        /* Custom product info area fields,
                                                                       if any (must be preceded with type/length byte) */
#endif
    uint8_t        end_of_record;

} product_area_format_hdr;

typedef struct t_product_area_format_hdr {
    product_area_format_hdr data;
    uint8_t padding[PADDING_SIZE(product_area_format_hdr)];
    uint8_t checksum;               // Product Info Area Checksum (zero checksum)
} t_product_area_format_hdr;

typedef struct module_current_record {
    t_multirecord_area_header hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                                     three byte ID assigned to PICMG®. For this
                                     specification, the value 12634 (00315Ah) shall
                                     be used. */
    uint8_t picmg_rec_id;   /* PICMG Record ID. */
    uint8_t rec_fmt_ver;    /* Record Format Version. For this specification,
                                     the value 0h shall be used. */
    uint8_t current;
} t_module_current_record;

typedef struct __attribute__ ((__packed__)) amc_channel_descriptor {
#ifdef BF_MS_FIRST
    uint8_t reserved:4;
    uint8_t lane3:5;
    uint8_t lane2:5;
    uint8_t lane1:5;
    uint8_t lane0:5;
#else
    uint8_t lane0:5;
    uint8_t lane1:5;
    uint8_t lane2:5;
    uint8_t lane3:5;
    uint8_t reserved:4;
#endif
} t_amc_channel_descriptor;

typedef struct __attribute__ ((__packed__)) amc_link_descriptor {
    /* LSB First */
#ifdef BF_MS_FIRST
    uint8_t reserved:6,
	assymetric_match:2;
    uint8_t link_grouping_id;
    uint16_t link_type_ext:4,
	link_type:12;
    uint8_t amc_channel_id;
#else
    uint8_t amc_channel_id;
    uint16_t link_type:12,
        link_type_ext:4;
    uint8_t link_grouping_id;
    uint8_t assymetric_match:2,
	reserved:6;
#endif
} t_amc_link_descriptor;


typedef struct amc_point_to_point_record {
    t_multirecord_area_header hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                                     three byte ID assigned to PICMG®. For this
                                     specification, the value 12634 (00315Ah) shall
                                     be used. */
    uint8_t picmg_rec_id;   /* PICMG Record ID. */
    uint8_t rec_fmt_ver;    /* Record Format Version. For this specification,
                                     the value 0h shall be used. */
    uint8_t oem_guid_cnt;
#ifdef POINT_TO_POINT_OEM_GUID_LIST
#if (POINT_TO_POINT_OEM_GUID_CNT == 0)
#error "The POINT_TO_POINT_OEM_GUID_LIST size cannot be 0 if defined"
#endif
    uint8_t oem_guid_list[16*POINT_TO_POINT_OEM_GUID_CNT];
#endif
#ifdef BF_MS_FIRST
    uint8_t connected_dev_id:4,
        reserved:3,
        record_type:1;
#else
    uint8_t record_type:1,          /* [7] Record Type - 1 AMC-Module, 0 On-Carrier Device */
        reserved:3,     /* [6:4] Reserved, write as 0h.*/
        connected_dev_id:4;      /* [3:0] Connected Dev ID if Record-Type =0, reserved, otherwise */
#endif
    uint8_t amc_channel_descriptor_cnt;

#ifdef AMC_POINT_TO_POINT_RECORD_CNT
    t_amc_channel_descriptor amc_channel_descriptor[AMC_POINT_TO_POINT_RECORD_CNT];
    t_amc_link_descriptor amc_link_descriptor[AMC_POINT_TO_POINT_RECORD_CNT];
#endif
} t_amc_point_to_point_record;

#define AMC_POINT_TO_POINT_RECORD_BUILD

typedef struct indirect_clock_descriptor {
#ifdef BF_MS_FIRST
uint8_t reserved:6,
    pll_connection:1,
    clock_assymetric_match:1;
#else
uint8_t clock_assymetric_match:1,
    pll_connection:1,
    reserved:6;
#endif
uint8_t dependent_clock_id;
} t_indirect_clock_descriptor;

typedef struct direct_clock_descriptor {
#ifdef BF_MS_FIRST
    uint8_t reserved:6,
        pll_connection:1,
        clock_assymetric_match:1;
#else
    uint8_t clock_assymetric_match:1,
        pll_connection:1,
        reserved:6;
#endif
    uint8_t clock_family;
    uint8_t accuracy_level;
    uint8_t clock_frequency[4];
    uint8_t clock_minimum_frequency[4];
    uint8_t clock_maximum_frequency[4];
} t_direct_clock_descriptor;

typedef struct clock_config_descriptor {
    uint8_t clock_id;
#ifdef BF_MS_FIRST
    uint8_t reserved:7,
        clock_activation_control:1;
#else
    uint8_t clock_activation_control:1,
        reserved:7;
#endif
    uint8_t indirect_clock_descriptor_cnt;
    uint8_t direct_clock_descriptor_cnt;
/* TODO: Implement indirect clock descriptors */
//    t_indirect_clock_descriptor indirect_descriptor;//[INDIRECT_CLOCK_DESCRIPTOR_CNT];
    t_direct_clock_descriptor direct_descriptor;
} t_clock_config_descriptor;

typedef struct amc_clock_config_record {
    t_multirecord_area_header hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                                     three byte ID assigned to PICMG®. For this
                                     specification, the value 12634 (00315Ah) shall
                                     be used. */
    uint8_t picmg_rec_id;   /* PICMG Record ID. */
    uint8_t rec_fmt_ver;    /* Record Format Version. For this specification,
                                     the value 0h shall be used. */
    uint8_t resource_id;
    uint8_t descriptor_cnt;
#ifdef AMC_CLOCK_CONFIGURATION_DESCRIPTORS_CNT
    t_clock_config_descriptor descriptor[AMC_CLOCK_CONFIGURATION_DESCRIPTORS_CNT];
#endif
} t_amc_clock_config_record;


#define DIRECT_CLOCK_CONNECTION(id, activation_control, pll_use, clock_source_receiver, family, accuracy, freq_Hz, min_Hz, max_Hz) \
    {   .clock_id = id,                                                 \
            .clock_activation_control = activation_control,             \
            .indirect_clock_descriptor_cnt = 0,                         \
            .direct_clock_descriptor_cnt = 1,                           \
            .direct_descriptor = { .pll_connection = pll_use,           \
                                   .clock_assymetric_match = clock_source_receiver, \
                                   .clock_family = family,              \
                                   .accuracy_level = accuracy,          \
                                   .clock_frequency = { (uint8_t)(((uint32_t)freq_Hz) & 0xFF), (uint8_t)((((uint32_t)freq_Hz) & 0xFF00) >> 8), (uint8_t)((((uint32_t)freq_Hz) & 0xFF0000) >> 16), (uint8_t)((((uint32_t)freq_Hz) & 0xFF000000) >> 24) }, \
                                   .clock_minimum_frequency = { (uint8_t)(((uint32_t)min_Hz) & 0xFF), (uint8_t)((((uint32_t)min_Hz) & 0xFF00) >> 8), (uint8_t)((((uint32_t)min_Hz) & 0xFF0000) >> 16), (uint8_t)((((uint32_t)min_Hz) & 0xFF000000) >> 24) }, \
                                   .clock_maximum_frequency = { (uint8_t)(((uint32_t)max_Hz) & 0xFF), (uint8_t)((((uint32_t)max_Hz) & 0xFF00) >> 8), (uint8_t)((((uint32_t)max_Hz) & 0xFF0000) >> 16), (uint8_t)((((uint32_t)max_Hz) & 0xFF000000) >> 24) } } \
    }

#define AMC_CLOCK_CONFIGURATION_LIST_BUILD                              \
    t_clock_config_descriptor clock_descriptor_list[] = { AMC_CLOCK_CONFIGURATION_LIST }

#define GENERIC_POINT_TO_POINT_RECORD(id, port0, port1, port2, port3, protocol, extension, matches) \
    p2p_record->amc_channel_descriptor_cnt++;                           \
    p2p_record->amc_channel_descriptor[id].lane0 = port0;               \
    p2p_record->amc_channel_descriptor[id].lane1 = port1;               \
    p2p_record->amc_channel_descriptor[id].lane2 = port2;               \
    p2p_record->amc_channel_descriptor[id].lane3 = port3;               \
    p2p_record->amc_channel_descriptor[id].reserved = 0xF;              \
    p2p_record->amc_link_descriptor[id].amc_channel_id = id;            \
    p2p_record->amc_link_descriptor[id].link_type = (protocol<<4)|0xF;  \
    p2p_record->amc_link_descriptor[id].link_type_ext = extension;      \
    p2p_record->amc_link_descriptor[id].link_grouping_id = 0;           \
    p2p_record->amc_link_descriptor[id].assymetric_match = matches;     \
    p2p_record->amc_link_descriptor[id].reserved = 0x3F


/* Move to user code area */

/* FRU Format */
#define COMMON_HEADER_SIZE                      sizeof(t_fru_common_header)

#define INTERNAL_USE_AREA_SIZE                  0

#ifdef CHASSIS_INFO_AREA_ENABLE
#define CHASSIS_INFO_AREA_SIZE                  (sizeof(FRU_CHASSIS_AREA_FORMAT_HDR))
#else
#define CHASSIS_INFO_AREA_SIZE                  0
#endif

#ifdef BOARD_INFO_AREA_ENABLE
#define BOARD_INFO_AREA_SIZE                    (sizeof(t_board_area_format_hdr))
#else
#define BOARD_INFO_AREA_SIZE                    0
#endif

#ifdef PRODUCT_INFO_AREA_ENABLE
#define PRODUCT_INFO_AREA_SIZE                  (sizeof(t_product_area_format_hdr))
#else
#define PRODUCT_INFO_AREA_SIZE                  0
#endif

#ifdef AMC_POINT_TO_POINT_RECORD_LIST
#define AMC_POINT_TO_POINT_RECORD_SIZE          (sizeof(t_amc_point_to_point_record))
#else
#define AMC_POINT_TO_POINT_RECORD_SIZE          0
#endif

#ifdef AMC_CLOCK_CONFIGURATION_LIST
#define AMC_CLOCK_CONFIG_RECORD_SIZE            (sizeof(t_amc_clock_config_record) )
#else
#define AMC_CLOCK_CONFIG_RECORD_SIZE            0
#endif

#ifdef MODULE_CURRENT_RECORD
#define MODULE_CURRENT_RECORD_SIZE              (sizeof(t_module_current_record))
#else
#define MODULE_CURRENT_RECORD_SIZE              0
#endif

#ifdef MULTIRECORD_AREA_ENABLE
#define MULTIRECORD_AREA_SIZE                   ( AMC_POINT_TO_POINT_RECORD_SIZE + \
                                                  AMC_CLOCK_CONFIG_RECORD_SIZE + \
                                                  MODULE_CURRENT_RECORD_SIZE )
#else
#define MULTIRECORD_AREA_SIZE                   0
#endif


#define COMMON_HEADER_OFFSET                    0
#define INTERNAL_USE_AREA_OFFSET                ( COMMON_HEADER_OFFSET+COMMON_HEADER_SIZE )
#define CHASSIS_INFO_AREA_OFFSET                ( INTERNAL_USE_AREA_OFFSET+INTERNAL_USE_AREA_SIZE )
#define BOARD_INFO_AREA_OFFSET                  ( CHASSIS_INFO_AREA_OFFSET+CHASSIS_INFO_AREA_SIZE )
#define PRODUCT_INFO_AREA_OFFSET                ( BOARD_INFO_AREA_OFFSET+BOARD_INFO_AREA_SIZE )
#define MULTIRECORD_AREA_OFFSET                 ( PRODUCT_INFO_AREA_OFFSET+PRODUCT_INFO_AREA_SIZE )
#define AMC_POINT_TO_POINT_RECORD_OFFSET        ( MULTIRECORD_AREA_OFFSET )
#define AMC_CLOCK_CONFIG_RECORD_OFFSET          ( AMC_POINT_TO_POINT_RECORD_OFFSET + AMC_POINT_TO_POINT_RECORD_SIZE )
#define MODULE_CURRENT_RECORD_OFFSET            ( AMC_CLOCK_CONFIG_RECORD_OFFSET + AMC_CLOCK_CONFIG_RECORD_SIZE )

#define FRU_SIZE                                ( MULTIRECORD_AREA_OFFSET+MULTIRECORD_AREA_SIZE )

extern uint8_t fru_data[FRU_SIZE];

void fru_init( void );
void fru_header_build( uint8_t * fru_buffer );
void board_info_area_build( uint8_t * fru_buffer );
void product_info_area_build( uint8_t * fru_buffer );
void amc_point_to_point_record_build( uint8_t * fru_buffer );
void point_to_point_clock_build( uint8_t * fru_buffer );
void module_current_record_build( uint8_t * fru_buffer );

/* IPMI Handlers */
void fru_read_to_buffer(char *buff, uint8_t offset, uint8_t length);
void fru_read_common_header(t_fru_common_header * header);
#endif
