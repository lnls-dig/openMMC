
/*
 * fru.h
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
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

#ifndef IPMI_FRU_H_
#define IPMI_FRU_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ipmb.h"

#define FRU_SIZE 512

typedef struct __attribute__((__packed__)) fru_common_header {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
uint8_t	:4,			/* Common Header Format Version
				   7:4 - reserved, write as 0000b */
	format_version:4;	/* 3:0 - format version number = 1h
				   for this specification. */
#else
    uint8_t	format_version:4,
	:4;
#endif
    uint8_t	int_use_offset;		/* Internal Use Area Starting Offset
					   (in multiples of 8 bytes). 00h
					   indicates that this area is not
					   present. */
    uint8_t	chassis_info_offset;	/* Chassis Info Area Starting
					   Offset (in multiples of 8 bytes). 00h
					   indicates that this area is not
					   present. */
    uint8_t	board_offset;		/* Board Area Starting Offset (in
					   multiples of 8 bytes). 00h indicates
					   that this area is not present. */
    uint8_t	product_info_offset;	/* Product Info Area Starting
					   Offset (in multiples of 8 bytes).
					   00h indicates that this area is not
					   present. */
    uint8_t	multirecord_offset;	/* MultiRecord Area Starting Offset
					   (in multiples of 8 bytes). 00h
					   indicates that this area is not
					   present. */
    uint8_t	pad;			/* PAD, write as 00h */
    uint8_t	checksum;		/* Common Header Checksum (zero checksum) */
} fru_common_header_t;


/* AMC Table 3-16 AdvancedMC Point-to-Point Connectivity record */
typedef struct __attribute__((__packed__)) amc_p2p_conn_record {
	uint8_t	record_type_id;	/* Record Type ID. For all records defined
				   in this specification a value of C0h (OEM)
				   shall be used. */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	uint8_t eol:1,		/* [7:7] End of list. Set to one for the last record */
          reserved:3,	/* [6:4] Reserved, write as 0h.*/
          version:4;	/* [3:0] record format version (2h for this definition) */
#else
	uint8_t	version:4,
		reserved:3,
		eol:1;
#endif
	uint8_t	record_len;	/* Record Length. # of bytes following rec cksum */
	uint8_t	record_cksum;	/* Record Checksum. Holds the zero checksum of
				   the record. */
	uint8_t	header_cksum;	/* Header Checksum. Holds the zero checksum of
				   the header. */
	uint8_t	manuf_id[3];	/* Manufacturer ID. LS Byte first. Write as the
				   three byte ID assigned to PICMG®. For this
				   specification, the value 12634 (00315Ah) shall
				   be used. */
	uint8_t	picmg_rec_id;	/* PICMG Record ID. For the AMC Point-to-Point
				   Connectivity record, the value 19h must be used  */
	uint8_t	rec_fmt_ver;	/* Record Format Version. For this specification,
				   the value 0h shall be used. */
	uint8_t	oem_guid_count;	/* OEM GUID Count. The number, n, of OEM GUIDs
				   defined in this record. */
//TODO	OEM_GUID oem_guid_list[n];
				/* A list 16*n bytes of OEM GUIDs. */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	uint8_t	record_type:1,	/* [7] Record Type – 1 AMC Module, 0 On-Carrier device */
		:3,		/* [6:4] Reserved; write as 0h */
		conn_dev_id:4;	/* [3:0] Connected-device ID if Record Type = 0,
				   Reserved, otherwise. */
#else
	uint8_t	conn_dev_id:4,
		:3,
		record_type:1;
#endif
	uint8_t	ch_descr_count;	/* AMC Channel Descriptor Count. The number, m,
				   of AMC Channel Descriptors defined in this record. */
//TODO	AMC_CHANNEL_DESCR ch_descr[m];
				/* AMC Channel Descriptors. A variable length
				   list of m three-byte AMC Channel Descriptors,
				   each defining the Ports that make up an AMC
				   Channel (least significant byte first).*/
//TODO	AMC_LINK_DESCR link_desrc[p];
				/* AMC Link Descriptors. A variable length list
				   of p five-byte AMC Link Descriptors (Least
				   significant byte first) (see Table 3-19, “AMC
				   Link Descriptor”, Table 3-20, “AMC Link Designator”,
				   and Table 3-21, “AMC Link Type”) totaling 5 * p
				   bytes in length. The value of p and the length
				   of the list are implied by Record Length, since
				   the list is at the end of this record.
				   Each AMC Link Descriptor details one type of
				   point-to-point protocol supported by the
				   referenced Ports. */
} amc_p2p_conn_record_t;

/* Table 3-10 Module Current Requirements record */
typedef struct __attribute__((__packed__)) module_current_requirements_record {
	uint8_t	rec_type_id;	/* Record Type ID. For all records
				   defined in this specification,
				   a value of C0h (OEM) must be used. */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	uint8_t	end_list:1,	/* [7] – End of List. Set to one for
				   the last record */
		:3,		/* [6:4] – Reserved, write as 0h */
		rec_format:4;	/* [3:0] – Record format version
				   (= 2h for this definition) */
#else
	uint8_t	rec_format:4,
			:3,
			end_list:1;
#endif
	uint8_t	rec_length;	/* Record Length */
	uint8_t	rec_cksum;	/* Record Checksum. Holds the zero
				   checksum of the record. */
	uint8_t	hdr_cksum;	/* Header Checksum. Holds the zero
				   checksum of the header. */
	uint8_t	manuf_id_lsb;	/* Manufacturer ID. Least significant
				   byte first. Write as the three byte ID
				   assigned to PICMG. For this specification
				   the value 12634 (00315Ah) must be used. */
	uint8_t	manuf_id_midb;
	uint8_t	manuf_id_msb;
	uint8_t	picmg_rec_id;	/* PICMG Record ID. For the Module Power Descriptor
				   table, the value 16h must be used. */
	uint8_t	rec_fmt_ver;	/* Record Format Version. For this specification,
				   the value 0h must be used. */
	uint8_t	curr_draw;	/* Current Draw. This field holds the Payload
				   Power (PWR) requirement of the Module given
				   as current requirement in units of 0.1A at 12V.
				   (This equals the value of the power in W
				   divided by 1.2.) */
} module_current_requirements_record_t;

typedef struct __attribute__((__packed__)) multirecord_area_header {
	uint8_t	record_type_id;	/* Record Type ID. For all records defined
				   in this specification a value of C0h (OEM)
				   shall be used. */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	uint8_t 	eol:1,		/* [7:7] End of list. Set to one for the last record */
	      	reserved:3,	/* [6:4] Reserved, write as 0h.*/
		version:4;	/* [3:0] record format version (2h for this definition) */
#else
	uint8_t	version:4,
		reserved:3,
		eol:1;
#endif
	uint8_t	record_len;	/* Record Length. */
	uint8_t	record_cksum;	/* Record Checksum. Holds the zero checksum of
				   the record. */
	uint8_t	header_cksum;	/* Header Checksum. Holds the zero checksum of
				   the header. */
	uint8_t	manuf_id[3];	/* Manufacturer ID. LS Byte first. Write as the
				   three byte ID assigned to PICMG®. For this
				   specification, the value 12634 (00315Ah) shall
				   be used. */
	uint8_t	picmg_rec_id;	/* PICMG Record ID. */
	uint8_t	rec_fmt_ver;	/* Record Format Version. For this specification,
				   the value 0h shall be used. */
} multirecord_area_header_t;

void fru_read_to_buffer(char *buff, int offset, int length);
void fru_read_common_header(fru_common_header_t * header);
void ipmi_storage_get_fru_info ( ipmi_msg * req, ipmi_msg * rsp);
void ipmi_storage_read_fru_data_cmd ( ipmi_msg * req, ipmi_msg * rsp);

#endif /* IPMI_FRU_H_ */
