/*
 * fru.c
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

#include "fru.h"
#include "ipmi.h"

/*
 * fru generated with external tool
 * mainly it says x4 pci-express Gen1
 * without this MCH will not enable FCLK
 */
#define FRU_AREA_SIZE 136

const char default_fru[136]/* __attribute__ ((section (".FRU")))*/ = {
    /* Common Header */
    0x01, 0x00, 0x00, 0x01, 0x07, 0x0D, 0x00, 0xEA,
    /* Board Info */
    0x01, 0x06, 0x19, 0x00, 0x00, 0x00,
        /* Board Manufacturer */
        0xC8, 0x43, 0x72, 0x65, 0x6F, 0x74, 0x65, 0x63, 0x68,
        /* Board Product Name */
        0xC3, 0x41, 0x46, 0x43,
        /* Board Serial Number */
        0xC9, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31,
        /* Board Part Number */
        0xC5, 0x41, 0x46, 0x43, 0x76, 0x32,
        /* FRU File ID */
        0xC7, 0x43, 0x6F, 0x72, 0x65, 0x46, 0x52, 0x55,
    /* End of Board Info Area */
    0xC1, 0x00, 0x00, 0x00, 0xAF,

    /* Product Info Area */
    0x01, 0x06, 0x19,
        /* Manufacturer Name */
        0xC3, 0x47, 0x53, 0x49,
        /* Product Name */
        0xC3, 0x41, 0x46, 0x43,
        /* Product Part/Model Number */
        0xC5, 0x30, 0x30, 0x30, 0x30, 0x31,
        /* Product Version */
        0xC2, 0x76, 0x33,
        /* Product Serial Number */
        0xC5, 0x30, 0x30, 0x30, 0x30, 0x31,
        /* Asset Tag */
        0xC6, 0x4E, 0x6F, 0x20, 0x74, 0x61, 0x67,
        /* FRU File ID */
        0xC7, 0x43, 0x6F, 0x72, 0x65, 0x46, 0x52, 0x55,
    /* End of Product Info Area*/
    0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF9,

    /* MultiRecord Area */
    /* Current Requirement Area */
    0xC0, 0x02, 0x06, 0x1E, 0x1A,
        /* Manufacturer ID (00315A for PICMG) LSB first */
        0x5A, 0x31, 0x00,
        /* PICMG Record ID */
        0x16, 0x00,
        /* Current Draw (in 100mA units) */
        0x41, /* 6,5 A */

    /* AMC Point-to-Point Connectivity Area */
    0xC0, 0x82, 0x10, 0xBB, 0xF3,
        /* Manufacturer ID (00315A for PICMG) LSB first */
        0x5A, 0x31, 0x00,
        /* PICMG Record ID */
        0x19, 0x00,
        /* OEM GUID Count */
        0x00,
        /* AMC Module */
        0x80,
        /* AMC Channel Descriptor Count */
        0x01,
        /* AMC Channel Descriptor */
        0x04, 0x00, 0xF0,
        /* AMC Link Descriptors */
        0x00, 0x2F, 0x00, 0x00, 0xFD
};

// @todo: add support for writing/readint to external memory

void fru_read_to_buffer(char *buff, int offset, int length) {
    int i;
    int j = offset;

    for (i = 0; i<length; i++, j++ ) {
        if (j < sizeof(default_fru)) {
            buff[i] = default_fru[j];
        } else {
            buff[i] = 0xFF;
        }
    }
}

void fru_read_common_header(fru_common_header_t * header) {
    fru_read_to_buffer( (char *) header, 0, sizeof(fru_common_header_t));
}

void ipmi_storage_get_fru_inventory( ipmi_msg * req, ipmi_msg * rsp )
{
    uint8_t len = rsp->data_len = 0;
    rsp->data[len++] = FRU_AREA_SIZE & 0xFF;
    rsp->data[len++] = FRU_AREA_SIZE & 0xFF00;
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
