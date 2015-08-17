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

/* C Standard includes */
#include "string.h"

/* Project includes */
#include "i2c.h"
#include "ipmb.h"
#include "board_defs.h"

/* Macro to check is the message is a response (odd netfn) */
#define IS_RESPONSE(msg) (msg.netfn & 0x01)

/* Local variables */
QueueHandle_t ipmb_txqueue = NULL;
QueueHandle_t client_queue = NULL;
static uint8_t current_seq;
static ipmi_msg_cfg last_sent_req;
static ipmi_msg_cfg last_received_req;

void IPMB_TXTask ( void * pvParameters )
{
    static ipmi_msg_cfg current_msg_tx;
    static uint8_t ipmb_buffer_tx[IPMI_MSG_MAX_LENGTH];

    for ( ;; ) {
        configASSERT( current_msg_tx );
        xQueueReceive( ipmb_txqueue, &current_msg_tx, portMAX_DELAY);
        if ( IS_RESPONSE(current_msg_tx.buffer) ) {
            /* We're sending a response */
            /* Match with previous request */
            if ( current_msg_tx.buffer.seq == last_received_req.buffer.seq ) {
                /* Check if the response was built in time, comparing the timeout value with the matching request */
                if ( (xTaskGetTickCount() - last_received_req.timestamp) < IPMB_MSG_TIMEOUT ) {
                    /* See if we've already tried sending this message 3 times */
                    if ( current_msg_tx.retries < IPMB_MAX_RETRIES ) {
                        /* Encode the message buffer to the IPMB format */
                        ipmb_encode( &ipmb_buffer_tx[0], &current_msg_tx.buffer );
                        if ( xI2CWrite( IPMB_I2C, current_msg_tx.buffer.dest_addr >> 1, &ipmb_buffer_tx[1], current_msg_tx.buffer.data_len + IPMB_HEADER_LENGTH ) != i2c_err_SUCCESS) {
                            /* Message couldn't be transmitted right now, increase retry counter and try again later */
                            current_msg_tx.retries++;
                            xQueueSendToFront( ipmb_txqueue, &current_msg_tx, 0 );
                        }
                        /* TODO: Find a way to pass the error to the client */
                        xTaskNotifyGive( current_msg_tx.caller_task );
                    }
                }
                /* If we're here, either the message has been successfully transmitted or we've exhausted our options to send it, give up */
            }
        }
        /* We're sending a request */
        else {
            if ( current_msg_tx.retries == 0 ) {
                /* Get the time when the message is first sent */
                current_msg_tx.timestamp = xTaskGetTickCount();
            }
            if ( current_msg_tx.retries < IPMB_MAX_RETRIES ) {
                ipmb_encode( &ipmb_buffer_tx[0], &current_msg_tx.buffer );
                if ( xI2CWrite( IPMB_I2C, current_msg_tx.buffer.dest_addr >> 1, &ipmb_buffer_tx[1], current_msg_tx.buffer.data_len + IPMB_HEADER_LENGTH ) != i2c_err_SUCCESS) {
                    current_msg_tx.retries++;
                    xQueueSendToFront( ipmb_txqueue, &current_msg_tx, 0 );
                } else {
                    /* Request was successfully sent, keep a copy here for future comparison */
                    last_sent_req = current_msg_tx;
                }
            }
            /* If we're here, either the message has been successfully transmitted or we've exhausted our options to send it, give up */
        }
    }
}

void IPMB_RXTask ( void *pvParameters )
{
    /* Declare these structs as static so they're zero-initialized */
    static ipmi_msg_cfg current_msg_rx;
    static uint8_t ipmb_buffer_rx[IPMI_MSG_MAX_LENGTH];
    uint8_t rx_len;

    for ( ;; ) {
        /* Checks if there's any incoming messages (the task remains blocked here) */
        /* TODO: Discover why when we put the timeout here as portMAX_DELAY the firmware deadlocks */
        rx_len = xI2CSlaveTransfer( IPMB_I2C, &ipmb_buffer_rx[0], 50 );
        if ( rx_len > 0 ) {
            /* Perform a checksum test on the message, if it doesn't pass, just ignore it. We have no way to know if we're the one who should receive it */
            if ( ipmb_assert_chksum( ipmb_buffer_rx, rx_len ) == ipmb_error_success ) {
                /* Maybe clear the msg_cfg struct before writing new data into it */
                ipmb_decode( &current_msg_rx.buffer, ipmb_buffer_rx, rx_len );
                if ( IS_RESPONSE(current_msg_rx.buffer ) ) {
                    /* The message is a response, check if it's been received in time */
                    if ( (xTaskGetTickCount() - last_sent_req.timestamp) < IPMB_MSG_TIMEOUT ) {
                        /* Seq number checking is enough to match the messages */
                        if ( current_msg_rx.buffer.seq == last_sent_req.buffer.seq ) {
                            ipmb_notify_client ( &current_msg_rx );
                        }
                        /* If we received a response that doesn't match a previously sent request, just discard it */
                    }
                }
                else {
                    /* The received message is a request */
                    /* Check if this is a repeated request (same SEQ), in this case just ignore this message, since it'll be responded shortly (I hope) */
                    if ( current_msg_rx.buffer.seq != last_received_req.buffer.seq ) {
                        /* Start counting the time, so we know if our response will be built in time */
                        current_msg_rx.timestamp = xTaskGetTickCount();
                        /* Save the message to pair with the future response */
                        last_received_req = current_msg_rx;
                        /* Notify the client about the new request */
                        ipmb_notify_client ( &current_msg_rx );
                    }
                }
            }
        }
    }
}

/* Initializes the IPMB Layer:
 * -> Configures the I2C Driver
 * -> Creates the TX queue for the IPMB Task
 * -> Creates both IPMB RX and IPMB TX tasks
 */
void ipmb_init ( void )
{
    vI2CInit( IPMB_I2C, I2C_Mode_IPMB );
    ipmb_txqueue = xQueueCreate( IPMB_TXQUEUE_LEN, sizeof(ipmi_msg_cfg) );
    vQueueAddToRegistry( ipmb_txqueue, "IPMB_TX_QUEUE");
    xTaskCreate( IPMB_TXTask, (const char*)"IPMB_TX", configMINIMAL_STACK_SIZE*2, ( void * ) NULL, IPMB_TXTASK_PRIORITY, ( TaskHandle_t * ) NULL );
    xTaskCreate( IPMB_RXTask, (const char*)"IPMB_RX", configMINIMAL_STACK_SIZE*2, ( void * ) NULL, IPMB_RXTASK_PRIORITY, ( TaskHandle_t * ) NULL );
}

ipmb_error ipmb_send_request ( uint8_t netfn, uint8_t cmd, uint8_t * data, uint8_t data_len )
{
    static ipmi_msg_cfg req;

    /* Builds the message according to the IPMB specification */
    req.buffer.dest_addr = MCH_ADDRESS;
    req.buffer.netfn = netfn;
    req.buffer.dest_LUN = 0;
    req.buffer.src_addr = ulCFG_MMC_GA();
    req.buffer.seq = current_seq++;
    req.buffer.src_LUN = 0;
    req.buffer.cmd = cmd;
    req.buffer.data_len = data_len;
    memcpy(req.buffer.data, data, data_len);
    req.caller_task = xTaskGetCurrentTaskHandle();

    /* Blocks here until is able put message in tx queue */
    if (xQueueSend( ipmb_txqueue, &req, 1) != pdTRUE ){
        return ipmb_error_failure;
    }

    /* Use this notification to block the function while the response does not arrive */
    /* TODO: We may use the notification value to receive an error number here */
    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) != pdTRUE ){
        return ipmb_error_failure;
    }
    return ipmb_error_success;
}

ipmb_error ipmb_send_response ( ipmi_msg * req, uint8_t * data, uint8_t data_len )
{
    static ipmi_msg_cfg resp;

    /* Builds the message according to the IPMB specification */
    resp.buffer.dest_addr = req->src_addr;
    resp.buffer.netfn = req->netfn + 1;
    resp.buffer.dest_LUN = req->src_LUN;
    resp.buffer.src_addr = req->dest_addr;
    resp.buffer.seq = req->seq;
    resp.buffer.src_LUN = req->dest_LUN;
    resp.buffer.cmd = req->cmd;
    resp.buffer.data_len = data_len;
    memcpy(resp.buffer.data, data, data_len);
    resp.caller_task = xTaskGetCurrentTaskHandle();

    /* Blocks here until is able put message in tx queue */
    if (xQueueSend( ipmb_txqueue, &resp, portMAX_DELAY) != pdTRUE ){
        return ipmb_error_failure;
    }

    /* Use this notification to block the function while the response does not arrive */
    /* TODO: We may use the notification value to receive an error number here */
    if ( ulTaskNotifyTake( pdTRUE, 1000 ) != pdTRUE ){
        return ipmb_error_failure;
    }
    return ipmb_error_success;
}

ipmb_error ipmb_notify_client ( ipmi_msg_cfg * msg_cfg )
{
    configASSERT( client_queue );
    /* Sends only the ipmi msg, not the control struct */
    if ( xQueueSend( client_queue, &(msg_cfg->buffer), CLIENT_NOTIFY_TIMEOUT ) ) {
        if ( msg_cfg->caller_task ) {
            xTaskNotifyGive( msg_cfg->caller_task );
        }
        return ipmb_error_success;
    }
    return ipmb_error_timeout;
}

/* Creates and returns a queue in which the client can block to receive the incoming requests */
ipmb_error ipmb_register_rxqueue ( QueueHandle_t * queue )
{
    configASSERT( queue );

    *queue = xQueueCreate( IPMB_CLIENT_QUEUE_LEN, sizeof(ipmi_msg) );
    /* Copies the queue handler so we know where to write */
    client_queue = *queue;
    if ( *queue ) {
        return ipmb_error_success;
    } else {
        return ipmb_error_queue_creation;
    }
}

/* Calculate the message checksum performing a simple 2's complement sum up to the specified 'range' byte */
uint8_t ipmb_calculate_chksum ( uint8_t * buffer, uint8_t range )
{
    configASSERT( pkt );
    uint8_t chksum = 0;
    uint8_t i;
    for ( i = 0; i < range; i++ ) {
        chksum -= buffer[i];
    }
    return chksum;
}

/* Asserts the input message checksums by calculating them using our local functions */
ipmb_error ipmb_assert_chksum ( uint8_t * buffer, uint8_t buffer_len )
{
    /* Debug assert */
    configASSERT( buffer );

    uint8_t header_chksum = buffer[2];
    uint8_t msg_chksum = buffer[buffer_len-1];
    uint8_t calc_header_chksum = ipmb_calculate_chksum( buffer, IPMI_HEADER_CHECKSUM_POSITION );
    uint8_t calc_msg_chksum = ipmb_calculate_chksum( buffer, buffer_len-1 );
    if ( header_chksum == calc_header_chksum ) {
        if ( msg_chksum == calc_msg_chksum ) {
            return ipmb_error_success;
        }
        return ipmb_error_hdr_chksum;
    }
    return ipmb_error_msg_chksum;
}

ipmb_error ipmb_encode ( uint8_t * buffer, ipmi_msg * msg )
{
    configASSERT( msg );
    configASSERT( buffer );

    buffer[0] = msg->dest_addr;
    buffer[1] = ( ( ( msg->netfn << 2 ) & IPMB_NETFN_MASK ) | ( msg->dest_LUN & IPMB_DEST_LUN_MASK ) );
    buffer[2] = ipmb_calculate_chksum( &buffer[0], IPMI_HEADER_CHECKSUM_POSITION );
    buffer[3] = msg->src_addr;
    buffer[4] = ( ( ( msg->seq << 2 ) & IPMB_SEQ_MASK ) | ( msg->src_LUN & IPMB_SRC_LUN_MASK ) );
    buffer[5] = msg->cmd;
    memcpy (&buffer[6], &msg->data[0], msg->data_len);
    buffer[6+msg->data_len] = ipmb_calculate_chksum( &buffer[0], 6+msg->data_len );

    return ipmb_error_success;
}

ipmb_error ipmb_decode ( ipmi_msg * msg, uint8_t * buffer, uint8_t len )
{
    configASSERT( msg );
    configASSERT( buffer );

    msg->dest_addr = buffer[0];
    msg->netfn = buffer[1] >> 2;
    msg->dest_LUN = ( buffer[1] & IPMB_DEST_LUN_MASK );
    msg->hdr_chksum = buffer[2];
    msg->src_addr = buffer[3];
    msg->seq = buffer[4] >> 2;
    msg->src_LUN = ( buffer[4] & IPMB_SRC_LUN_MASK );
    msg->cmd = buffer[5];
    msg->data_len = len - 7;
    memcpy( &msg->data[0], &buffer[6], msg->data_len);

    return ipmb_error_success;
}

