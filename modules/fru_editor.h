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

#ifndef FRU_EDITOR_H_
#define FRU_EDITOR_H_

#include "FreeRTOS.h"
#include "string.h"
#include "ipmi.h"

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
#define PLL_PRSNT               (0x02)
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
#define UNUSED_PORT             0x1F

#define current_in_ma(curr)    (uint8_t)(curr/100)

/* FMC Module Size */
#define FMC_SINGLE_WIDTH        (0x00)
#define FMC_DOUBLE_WIDTH        (0x01)

/* FMC Connector Size */
#define FMC_CONN_LPC            (0x00)
#define FMC_CONN_HPC            (0x01)
#define FMC_CONN_NOT_FITTED     (0x03)

/* FMC Clock Direction */
#define MEZZANINE_TO_CARRIER    (0x00)
#define CARRIER_TO_MEZZANINE    (0x01)


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
} fru_common_header_t;

typedef struct fru_multirecord_area_header {
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
    uint8_t       record_chksum;   /* Record Checksum. Holds the zero checksum of
                                      the record. */
    uint8_t       header_chksum;   /* Header Checksum. Holds the zero checksum of
                                      the header. */
} fru_multirecord_area_header_t;


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
} fru_internal_use_area_t;

typedef struct {
    fru_multirecord_area_header_t hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                               three byte ID assigned to PICMG®. For this
                               specification, the value 12634 (00315Ah) shall
                               be used. */
    uint8_t picmg_rec_id;   /* PICMG Record ID. */
    uint8_t rec_fmt_ver;    /* Record Format Version. For this specification,
                               the value 0h shall be used. */
    uint8_t current;
} fru_module_current_record_t;

typedef struct __attribute__ ((__packed__)) {
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
} amc_channel_descriptor_t;

typedef struct __attribute__ ((__packed__)) {
    /* LSB First */
#ifdef BF_MS_FIRST
    uint8_t reserved:6,
        assymetric_match:2;
    uint8_t link_grouping_id;
    uint16_t link_type_ext:4,
        link_type:8,
        lane_bit_flag:4;
    uint8_t amc_channel_id;
#else
    uint8_t amc_channel_id;
    uint16_t lane_bit_flag:4,
        link_type:8,
        link_type_ext:4;
    uint8_t link_grouping_id;
    uint8_t assymetric_match:2,
        reserved:6;
#endif
} amc_link_descriptor_t;

typedef struct {
    amc_channel_descriptor_t channel_desc;
    amc_link_descriptor_t link_desc;
} amc_p2p_descriptor_t;

typedef struct amc_point_to_point_record {
    fru_multirecord_area_header_t hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                               three byte ID assigned to PICMG®. For this
                               specification, the value 12634 (00315Ah) shall
                               be used. */
    uint8_t picmg_rec_id;   /* PICMG Record ID. */
    uint8_t rec_fmt_ver;    /* Record Format Version. For this specification,
                               the value 0h shall be used. */
    uint8_t oem_guid_cnt;
    uint8_t oem_guid_list[0]; /* Not Implemented */
#ifdef BF_MS_FIRST
    uint8_t record_type:1,          /* [7] Record Type - 1 AMC-Module, 0 On-Carrier Device */
        reserved:3,     /* [6:4] Reserved, write as 0h.*/
        connected_dev_id:4;      /* [3:0] Connected Dev ID if Record-Type =0, reserved, otherwise */
#else
    uint8_t connected_dev_id:4,
        reserved:3,
        record_type:1;
#endif
    uint8_t amc_channel_descriptor_cnt;

    amc_p2p_descriptor_t p2p_descriptor[];
} amc_point_to_point_record_t;

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
} indirect_clock_descriptor_t;

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
} direct_clock_descriptor_t;

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
    //indirect_clock_descriptor_t indirect_descriptor; /* Not implemented */
    direct_clock_descriptor_t direct_descriptor;
} clock_config_descriptor_t;

typedef struct amc_clock_config_record {
    fru_multirecord_area_header_t hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                               three byte ID assigned to PICMG®. For this
                               specification, the value 12634 (00315Ah) shall
                               be used. */
    uint8_t picmg_rec_id;   /* PICMG Record ID. */
    uint8_t rec_fmt_ver;    /* Record Format Version. For this specification,
                               the value 0h shall be used. */
    uint8_t resource_id;
    uint8_t descriptor_cnt;

    clock_config_descriptor_t descriptors[];
} amc_clock_config_record_t;


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
    },

#define INDIRECT_CLOCK_CONNECTION(id, activation_control, pll_use, clock_source_receiver, indirect_id) \
    {   .clock_id = id,                                                 \
            .clock_activation_control = activation_control,             \
            .indirect_clock_descriptor_cnt = 1,                         \
            .direct_clock_descriptor_cnt = 0,                           \
            .indirect_descriptor = { .pll_connection = pll_use,         \
                                     .clock_assymetric_match = clock_source_receiver, \
                                     .dependent_clock_id = indirect_id, \
        }                                                               \
    },

#define GENERIC_POINT_TO_POINT_RECORD(id, port0, port1, port2, port3, protocol, extension, matches) \
    {                                                                   \
        .channel_desc = {                                               \
            .lane0 = port0,                                             \
            .lane1 = port1,                                             \
            .lane2 = port2,                                             \
            .lane3 = port3,                                             \
            .reserved = 0xF                                             \
        },                                                              \
            .link_desc = {                                              \
            .amc_channel_id = id,                                       \
            .lane_bit_flag = 0xF,                                       \
            .link_type = protocol,                                      \
            .link_type_ext = extension,                                 \
            .link_grouping_id = 0,                                      \
            .assymetric_match = matches,                                \
            .reserved = 0x3F                                            \
        }                                                               \
    },

typedef struct zone3_compatibility_rec {
    fru_multirecord_area_header_t hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                               three byte ID assigned to PICMG®. For this
                               specification, the value 12634 (00315Ah) shall
                               be used. */
    uint8_t picmg_rec_id;   /* PICMG Record ID. */
    uint8_t rec_fmt_ver;    /* Record Format Version. For Zone 3 descriptor the value 0x01 shall be used. */
    uint8_t interface_id_type;
    uint8_t user_manuf_id[3];
    uint8_t compat_designator[4];
} zone3_compatibility_rec_t;

typedef struct fmc_subtype_rec {
    fru_multirecord_area_header_t hdr;
    uint8_t manuf_id[3];    /* Manufacturer ID. LS Byte first. Write as the
                               three byte ID assigned to Vita 57.1. For this
                               specification, the value (0012A2h) shall
                               be used. */
    uint8_t version:4,
        subtype:4;
    uint8_t reserved:1,
        clk_dir:1,
        p2_conn_size:2,
        p1_conn_size:2,
        module_size:2;
    uint8_t p1_a_signals;
    uint8_t p1_b_signals;
    uint8_t p2_a_signals;
    uint8_t p2_b_signals;
    uint8_t p2_gbt:4,
        p1_gbt:4;
    uint8_t max_tck_clk;
} fmc_subtype_rec_t;

typedef struct dc_load_rec {
    fru_multirecord_area_header_t hdr;
    uint8_t output_number:4,
        reserved:4;
    uint8_t nominal_voltage[2];
    uint8_t min_spec_volt[2];
    uint8_t max_spec_volt[2];
    uint8_t ripple_noise_pkpk[2];
    uint8_t min_current_load[2];
    uint8_t max_current_load[2];
} dc_load_rec_t;

typedef struct dc_output_rec {
    fru_multirecord_area_header_t hdr;
    uint8_t output_number:4,
        reserved:3,
        standby:1;
    uint8_t nominal_voltage[2];
    uint8_t max_neg_dev[2];
    uint8_t max_pos_dev[2];
    uint8_t ripple_noise_pkpk[2];
    uint8_t min_current_draw[2];
    uint8_t max_current_draw[2];
} dc_output_rec_t;


uint8_t fru_header_build( uint8_t **buffer, size_t int_use_off, size_t chassis_off, size_t board_off, size_t product_off, size_t multirecord_off );
uint8_t board_info_area_build( uint8_t **buffer, uint8_t lang, uint32_t mfg_time, const char *manuf, const char *name, const char *sn, const char *pn, const char *file_id );
uint8_t chassis_info_area_build( uint8_t **buffer, uint8_t type, const char *pn, const char *sn, uint8_t *custom_data, size_t custom_data_sz );
uint8_t product_info_area_build( uint8_t **buffer, uint8_t lang, const char *manuf, const char *name, const char *part_model, const char *version, const char *serial, const char *asset_tag, const char *file_id );
uint8_t amc_point_to_point_record_build( uint8_t **buffer, amc_p2p_descriptor_t * p2p_desc, uint8_t desc_count );
uint8_t amc_point_to_point_clock_build( uint8_t **buffer, clock_config_descriptor_t * clk_desc, uint8_t desc_count );
uint8_t module_current_record_build( uint8_t **buffer, uint8_t current );
uint8_t zone3_compatibility_record_build( uint8_t **buffer, uint32_t manuf_id, uint32_t compat_code );
uint8_t fmc_subtype_record_build( uint8_t **buffer, uint8_t clock_dir, uint8_t module_size, uint8_t p1_conn_size, uint8_t p2_conn_size, uint8_t p1_a_count, uint8_t p1_b_count, uint8_t p2_a_count, uint8_t p2_b_count, uint8_t p1_gbt, uint8_t p2_gbt, uint8_t eol );
uint8_t dc_load_record_build( uint8_t **buffer, uint16_t nominal_volt, uint16_t min_volt, uint16_t max_volt, uint16_t ripple_noise, uint16_t min_load, uint16_t max_load, uint8_t eol );
uint8_t dc_output_record_build( uint8_t **buffer, uint16_t nominal_volt, uint16_t neg_dev, uint16_t pos_dev, uint16_t ripple_noise, uint16_t min_draw, uint16_t max_draw, uint8_t eol );

size_t amc_fru_info_build( uint8_t **buffer );
#ifdef MODULE_RTM
size_t rtm_fru_info_build( uint8_t **buffer );
#endif

#endif
