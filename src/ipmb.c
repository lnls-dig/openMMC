/*
 * ipmb.c
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

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Project includes */
#include "i2c.h"
#include "ipmb.h"
#include "board_defs.h"

QueueHandle_t ipmb_txqueue = NULL;
QueueHandle_t client_queue = NULL;

void IPMB_Task ( void *pvParameters )
{
    /* Declare these structs as static so they're zero-initialized */
    static ipmi_msg_cfg msg_cfg;
    static ipmi_msg_cfg msg_cfg_sent_req;
    static ipmi_msg_cfg msg_cfg_recv_req;

    for ( ;; ) {
        /* Checks if there's any message to be sent */
        if ( xQueueReceive( ipmb_txqueue, &msg_cfg, 0 ) ) {
            if ( msg_cfg.ipmi_msg.msg.netfn && 0x01 ) {
                /* We're sending a response */
                /* Match with previous request */
                if ( msg_cfg.ipmi_msg.msg.seq == msg_cfg_recv_req.ipmi_msg.msg.seq ) {
                    /* TODO: check if the response was built in time, comparing the timeout value with the matching request */
                    if ( msg_cfg.retries < IPMB_MAX_RETRIES ) {
                        if ( xI2CWrite( IPMB_I2C, msg_cfg.ipmi_msg.msg.dest_addr, &msg_cfg.ipmi_msg.msg.dest_addr+1, sizeof(msg_cfg.ipmi_msg.msg)-1 ) != i2c_err_SUCCESS) {
                            /* Message couldn't be transmitted right now, increase retry counter and try again later */
                            msg_cfg.retries++;
                        xQueueSendToFront( ipmb_txqueue, &msg_cfg, 0 );
                        }
                    }
                }
                /* If we're here, either the message has been successfully transmitted or we've exhausted our options to send it, give up */
                /* TODO: Find a way to pass the error to the client */
            } else {
                /* We're sending a request */
                if ( msg_cfg.retries == 0 ) {
                    /* Get the time when the message is first sent */
                    //msg_cfg.timestamp = get_timestamp;
                }
                if ( msg_cfg.retries < IPMB_MAX_RETRIES ) {
                    if ( xI2CWrite( IPMB_I2C, msg_cfg.ipmi_msg.msg.dest_addr, &msg_cfg.ipmi_msg.msg.dest_addr+1, sizeof(msg_cfg.ipmi_msg.msg)-1 ) != i2c_err_SUCCESS ) {
                    msg_cfg.retries++;
                    xQueueSendToFront( ipmb_txqueue, &msg_cfg, 0 );
                    } else {
                        /* Request was successfully sent, keep a copy here for future comparison */
                        msg_cfg_sent_req = msg_cfg;
                    }
                }
                /* If we're here, either the message has been successfully transmitted or we've exhausted our options to send it, give up */
            }
        }

        /* Checks if there's any incoming messages */
        if ( xI2CSlaveTransfer( IPMB_I2C, msg_cfg.ipmi_msg.buffer, 0 ) == i2c_err_SUCCESS ) {
            /* Perform a checksum test on the message, if it doesn't pass, just ignore it. We have no way to know if we're the one who should receive it */
            if ( ipmb_assert_chksum( &msg_cfg.ipmi_msg ) == ipmb_err_success) {
                if ( msg_cfg.ipmi_msg.msg.netfn && 0x1 ) {
                    /* The received message is a response, check the matching request */
                    if ( msg_cfg.ipmi_msg.msg.seq == msg_cfg_sent_req.ipmi_msg.msg.seq ) {
                        /* Seq number checking is enough to match the messages */
                        ipmb_notify_client ( msg_cfg );
                    }
                    /* If we received a response that doesn't match a previously sent request, just discard it */
                } else {
                    /* The message is a request */
                    /* TODO: check if this is a repeated request (same SEQ), in this case just ignore this message, since it'll be responded shortly (I hope) */
                    if ( msg_cfg_recv_req.ipmi_msg.msg.seq != msg_cfg_sent_req.ipmi_msg.msg.seq ) {
                        /* Start counting the time, so we know if our response will be built in time */
                        //msg_cfg.timestamp = get_timestamp;
                        /* Save the message to pair with the future response */
                        msg_cfg_recv_req = msg_cfg;
                        /* Notify the client about the new request */
                        ipmb_notify_client ( msg_cfg );
                    }
                }
            }
        }
    }
}

void ipmb_init ( void )
{
    vI2CInit( IPMB_I2C, I2C_Mode_IPMB );
    ipmb_txqueue = xQueueCreate( IPMB_TXQUEUE_LEN, sizeof(ipmi_msg_cfg) );
    xTaskCreate( IPMB_Task, (const char*)"IPMB", configMINIMAL_STACK_SIZE*2, ( void * ) NULL, IPMB_TASK_PRIORITY, ( TaskHandle_t * ) NULL );
}

ipmb_err ipmb_send ( uint8_t netfn, uint8_t cmd, uint8_t * data, uint8_t data_len )
{
    static ipmi_msg_cfg cfg_pkt;

    /* Builds the message according to the IPMB specification */
    cfg_pkt.ipmi_msg.msg.dest_addr = MCH_ADDRESS;
    cfg_pkt.ipmi_msg.msg.netfn = netfn;
    cfg_pkt.ipmi_msg.msg.dest_LUN = 0;
    cfg_pkt.ipmi_msg.msg.hdr_chksum = ipmb_calculate_checksum(&cfg_pkt.ipmi_msg, IPMI_HEADER_LENGTH);
    cfg_pkt.ipmi_msg.msg.src_addr = ulCFG_MMC_GA();
//    cfg_pkt.ipmi_msg.msg.seq = get_current_seq;
    cfg_pkt.ipmi_msg.msg.src_LUN = 0;
    cfg_pkt.ipmi_msg.msg.cmd = cmd;
    memcpy(cfg_pkt.ipmi_msg.msg.data, data, data_len);
    cfg_pkt.ipmi_msg.msg.msg_chksum = ipmb_calculate_checksum(&cfg_pkt.ipmi_msg, IPMI_MSG_MAX_LENGTH);
    cfg_pkt.caller_task = xTaskGetCurrentTaskHandle();

    /* Blocks here until is able put message in tx queue */
    xQueueSend( ipmb_txqueue, &cfg_pkt, portMAX_DELAY);

    /* Use this notification just to block the function while the response does not arrive */
    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) == pdTRUE ){
        return ipmb_err_success;
    }
    /* Should not be here, return failure */
    return ipmb_err_failure;
}

ipmb_err ipmb_notify_client ( ipmi_msg_cfg msg_cfg )
{
    configASSERT( client_queue );
    /* Sends only the ipmi msg, not the control struct */
    if ( xQueueSend( client_queue, &msg_cfg.ipmi_msg, CLIENT_NOTIFY_TIMEOUT ) ) {
        xTaskNotifyGive( msg_cfg.caller_task );
        return ipmb_err_success;
    }
    return ipmb_err_timeout;
}

ipmb_err ipmb_register_rxqueue ( QueueHandle_t queue )
{
    configASSERT( queue );

    client_queue = queue;
    return ipmb_err_success;
}

uint8_t ipmb_calculate_chksum ( ipmi_pkt * pkt, uint8_t range )
{
    configASSERT( pkt );
    uint8_t chksum = 0;
    for ( uint8_t i = 0; i < range; i++ ) {
        /* We must not include the header chksum when calculating the message final chksum */
        if ( &(pkt->buffer[i]) == &(pkt->msg.hdr_chksum) ) {
            continue;
        }
        chksum -= pkt->buffer[i];
    }
    return chksum;
}

ipmb_err ipmb_assert_chksum ( ipmi_pkt * pkt )
{
    /* Debug assert */
    configASSERT( pkt );
    uint8_t header_chksum = pkt->msg.hdr_chksum;
    uint8_t msg_chksum = pkt->msg.msg_chksum;
    uint8_t calc_header_chksum = ipmb_calculate_chksum( pkt, IPMI_HEADER_LENGTH );
    uint8_t calc_msg_chksum = ipmb_calculate_chksum( pkt, IPMI_MSG_MAX_LENGTH );
    if ( header_chksum == calc_header_chksum ) {
        if ( msg_chksum == calc_msg_chksum ) {
            return ipmb_err_success;
        }
        return ipmb_err_hdr_chksum;
    }
    return ipmb_err_msg_chksum;
}
