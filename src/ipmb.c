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

static QueueHandle_t ipmb_txqueue;
static QueueHandle_t ipmb_rxqueue;

void IPMB_Task ( void *pvParameters )
{

    ipmi_msg_cfg_t msg_cfg;

    for ( ;; ) {
        /* Checks if there's any message to be sent */
        if ( xQueueReceive( ipmb_txqueue, &msg_cfg, 0 ) ){
            /* This function blocks here until the message is successfully sent or an error returns */
            if (msg_cfg.msg.netfn && 0x01) {
                /* We're sending a response, no need to keep a copy of the message here */
                xI2CWrite( IPMB_I2C, msg_cfg.ipmi_msg.msg.dest_addr, &msg_cfg.ipmi_msg.msg.netfn, sizeof(msg_cfg.ipmi_msg.msg)-1 );
                /* TODO: Find a way to pass the error to the client */
            } else {
                if (msg_cfg.retries == 0) {
                    /* Get the time when the message is first sent */
                    nsg_cfg.timestamp = get_timestamp;
                }
                if (msg_cfg.retries < IPMB_MAX_RETRIES) {
                    xI2CWrite( IPMB_I2C, msg_cfg.ipmi_msg.msg.dest_addr, &msg_cfg.ipmi_msg.msg.netfn, sizeof(msg_cfg.ipmi_msg.msg)-1 );
                    msg_cfg.retries++;
                    xQueueSendToFront( ipmb_txqueue, &msg_cfg, 0 );
                }
            }
        }

        /* Checks if there's any incoming messages */
        if ( xI2CSlaveTransfer( IPMB_I2C, msg_cfg.buffer, 0 ) == err_SUCCESS ){
            /* Handle the received message and notify the client */
            /* Is it a response? */
            if (msg_cfg.msg.netfn && 0x1) {
                /* Match with previous request and verify timeout */
            } else {
                /* Copy request to client task */
                /* Maybe keep the request in the queue, so we can match our response */
            }
        }
    }
}

ipmb_error ipmb_init ( void )
{
    vI2CInit( IPMB_I2C, I2C_Mode_IPMB );
    ipmb_txqueue = xQueueCreate( IPMB_TXQUEUE_LEN, sizeof() );
    ipmb_rxqueue = xQueueCreate( IPMB_RXQUEUE_LEN, sizeof() );
    xTaskCreate( ipmb_task, (const char*)"IPMB", configMINIMAL_STACK_SIZE*2, ( void * ) NULL, IPMB_TASK_PRIORITY, ( TaskHandle_t * ) NULL );
}

ipmb_error ipmb_send ( uint8_t netfn, uint8_t cmd, uint8_t * data )
{
    static ipmi_pkt temp;

    /* Builds the message according to the IPMB specification */
    temp.msg.dest_addr = MCH_ADDRESS;
    temp.msg.netfn = netfn;
    temp.msg.dest_LUN = 0;
    temp.msg.hdr_chksum = ipmb_header_checksum(&msg);
    temp.msg.src_addr = get_own_slave_addr;
    temp.msg.seq = get_current_seq;
    temp.msg.src_LUN = get_own_LUN;
    temp.msg.cmd = cmd;
    /* Maybe use memcpy to transfer the data rather than just setting the pointer */
    temp.msg.data = data;
    temp.msg.msg_chksum = ipmb_msg_checksum(&msg);

    /* Blocks here until is able put message in tx queue */
    xQueueSend( txqueue_handle, &temp, portMAX_DELAY);
    
    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) == pdTRUE ){
        /* return error */
    }
    /* Should not be here */
    return ipmb_err_failure;
}

ipmb_error ipmb_register_task ( TaskHandle_t handle )
{
    ipmb_cfg.client_task = handle;
}
