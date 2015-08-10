/*
 * ipmb.h
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
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

#ifndef IPMB_H_
#define IPMB_H_

#define IPMB_I2C                I2C0
#define IPMB_TASK_PRIORITY      1
#define IPMB_TXQUEUE_LEN        5
#define IPMB_MAX_RETRIES        3
#define CLIENT_NOTIFY_TIMEOUT   5

#define IPMI_HEADER_LENGTH      2
#define IPMI_MSG_MAX_LENGTH     32

#define MCH_ADDRESS             0x20

typedef struct ipmi_msg {
    uint8_t dest_addr;
    uint8_t netfn:6,
        dest_LUN:2;
    uint8_t hdr_chksum;
    uint8_t src_addr;
    uint8_t seq:6,
        src_LUN:2;
    uint8_t cmd;
    /* Data field has 24 bytes:
     * 32 (Max IPMI msg len) - 7 header bytes - 1 final chksum byte */
    uint8_t data[24];
    uint8_t msg_chksum;
} ipmi_msg;

/* There's a problem using a union here: when sending our messages, we'll always send 32 bytes, no matter how many data byte are actually usefull */
typedef union ipmi_pkt {
    uint8_t buffer[IPMI_MSG_MAX_LENGTH];
    ipmi_msg msg;
} ipmi_pkt;

typedef struct ipmi_msg_cfg {
    ipmi_pkt ipmi_msg;
    TaskHandle_t caller_task;
    uint8_t retries;
    uint32_t timestamp;
} ipmi_msg_cfg;

typedef enum ipmb_err {
ipmb_err_success = 0,
    ipmb_err_failure,
    ipmb_err_timeout,
    ipmb_err_hdr_chksum,
    ipmb_err_msg_chksum
} ipmb_err;

/* Function Prototypes */
void IPMB_Task ( void *pvParameters );
void ipmb_init ( void );
ipmb_err ipmb_send ( uint8_t netfn, uint8_t cmd, uint8_t * data, uint8_t data_len );
ipmb_err ipmb_register_rxqueue ( QueueHandle_t queue );
/* Maybe put these prototypes on the ipmb.c since they're "private" functions */
ipmb_err ipmb_notify_client ( ipmi_msg_cfg msg_cfg );
ipmb_err ipmb_assert_chksum ( ipmi_pkt * pkt );
uint8_t ipmb_calculate_chksum ( ipmi_pkt * pkt, uint8_t range );

#endif
