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
#include "led.h"

ipmb_error ipmb_notify_client ( ipmi_msg_cfg * msg_cfg );
ipmb_error ipmb_assert_chksum ( uint8_t * buffer, uint8_t buffer_len );
uint8_t ipmb_calculate_chksum ( uint8_t * buffer, uint8_t range );
ipmb_error ipmb_encode ( uint8_t * buffer, ipmi_msg * msg );
ipmb_error ipmb_decode ( ipmi_msg * msg, uint8_t * buffer, uint8_t len );

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
    xQueueReceive( ipmb_txqueue, &current_msg_tx, portMAX_DELAY);


    if ( IS_RESPONSE(current_msg_tx.buffer) ) {
      /* We're sending a response */

      /**********************************/
      /*       Error checking  	        */
      /**********************************/

      /* Match with previous request */
      if ( current_msg_tx.buffer.seq != last_received_req.buffer.seq ) {
	xTaskNotify( current_msg_tx.caller_task, ipmb_error_invalid_req , eSetValueWithOverwrite);
	continue;
      }

      /* Check if the response was built in time, comparing the timeout value with the matching request */
      if ((xTaskGetTickCount() - last_received_req.timestamp) >= IPMB_MSG_TIMEOUT ) {
	xTaskNotify( current_msg_tx.caller_task ,ipmb_error_timeout , eSetValueWithOverwrite);
	continue;
      }

      /* See if we've already tried sending this message 3 times */
      if ( current_msg_tx.retries > IPMB_MAX_RETRIES ) {
	xTaskNotify( current_msg_tx.caller_task ,ipmb_error_failure , eSetValueWithOverwrite);
	continue;
      }

      /**********************************/
      /* Try sending the message	*/
      /**********************************/

      /* Encode the message buffer to the IPMB format */
      ipmb_encode( &ipmb_buffer_tx[0], &current_msg_tx.buffer );

      if ( xI2CWrite( IPMB_I2C, current_msg_tx.buffer.dest_addr >> 1, &ipmb_buffer_tx[1], current_msg_tx.buffer.data_len + IPMB_RESP_HEADER_LENGTH ) != i2c_err_SUCCESS ) {
	/* Message couldn't be transmitted right now, increase retry counter and try again later */
	current_msg_tx.retries++;
	xQueueSendToFront( ipmb_txqueue, &current_msg_tx, 0 );

      }else{
	/* Success case*/
	xTaskNotify( current_msg_tx.caller_task , ipmb_error_success, eSetValueWithOverwrite);
      }

      /***************************************/
      /* Sending new outgoing request        */
      /***************************************/

    }else{

      /* Get the time when the message is first sent */
      if ( current_msg_tx.retries == 0 ) {
	current_msg_tx.timestamp = xTaskGetTickCount();
      }

      ipmb_encode( &ipmb_buffer_tx[0], &current_msg_tx.buffer );

      if ( xI2CWrite( IPMB_I2C, current_msg_tx.buffer.dest_addr >> 1, &ipmb_buffer_tx[1], current_msg_tx.buffer.data_len + IPMB_REQ_HEADER_LENGTH ) != i2c_err_SUCCESS) {

	current_msg_tx.retries++;

	if( current_msg_tx.retries > IPMB_MAX_RETRIES ){
	  xTaskNotify ( current_msg_tx.caller_task, ipmb_error_failure, eSetValueWithOverwrite);
	}else{
	  xQueueSendToFront( ipmb_txqueue, &current_msg_tx, 0 );
	}

      } else {
	/* Request was successfully sent, keep a copy here for future comparison */
	last_sent_req = current_msg_tx;
	xTaskNotify ( current_msg_tx.caller_task, ipmb_error_success, eSetValueWithOverwrite);
      }
    }
  }
}



void IPMB_RXTask ( void *pvParameters )
{
  ipmi_msg_cfg current_msg_rx;
  uint8_t ipmb_buffer_rx[IPMI_MSG_MAX_LENGTH];
  uint8_t rx_len;

  for ( ;; ) {
    /* Checks if there's any incoming messages (the task remains blocked here) */
    configASSERT(ipmb_buffer_rx);
    rx_len = xI2CSlaveTransfer( IPMB_I2C, &ipmb_buffer_rx[0], portMAX_DELAY );

    if ( rx_len > 0 ) {

      /* Perform a checksum test on the message, if it doesn't pass, just ignore it.
	 Following the IPMB specs, we have no way to know if we're the one who should
	 receive it. In MicroTCA crates with star topology for IPMB, we are assured we
	 are the recipients, however, malformed messages may be safely ignored as the
	 MCMC should take care of retrying. */

      if ( ipmb_assert_chksum( ipmb_buffer_rx, rx_len ) != ipmb_error_success ) {
	continue;
      }

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

      }else {

	/* The received message is a request */
	/* Check if this is a repeated request (same SEQ), in this case just ignore
	   this message, since it'll be responded shortly (I hope) */
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

void ipmb_init ( void )
{
    vI2CInit( IPMB_I2C, I2C_Mode_IPMB );
    ipmb_txqueue = xQueueCreate( IPMB_TXQUEUE_LEN, sizeof(ipmi_msg_cfg) );
    vQueueAddToRegistry( ipmb_txqueue, "IPMB_TX_QUEUE");
    xTaskCreate( IPMB_TXTask, (const char*)"IPMB_TX", configMINIMAL_STACK_SIZE*2, ( void * ) NULL, IPMB_TXTASK_PRIORITY, ( TaskHandle_t * ) NULL );
    xTaskCreate( IPMB_RXTask, (const char*)"IPMB_RX", configMINIMAL_STACK_SIZE*2, ( void * ) NULL, IPMB_RXTASK_PRIORITY, ( TaskHandle_t * ) NULL );
}

ipmb_error ipmb_send_request ( ipmi_msg * req )
{
    ipmi_msg_cfg req_cfg;

    /* Builds the message according to the IPMB specification */

    /* Copies data from the msg struct passed by caller */
    memcpy( &(req_cfg.buffer), req, sizeof(ipmi_msg));
    /* Write necessary fields (should be garbage data by now) */
    req_cfg.buffer.dest_addr = MCH_ADDRESS;
    req_cfg.buffer.dest_LUN = 0;
    req_cfg.buffer.src_addr = get_ipmb_addr();
    req_cfg.buffer.src_LUN = 0;
    req_cfg.buffer.seq = current_seq++;
    req_cfg.caller_task = xTaskGetCurrentTaskHandle();

    /* Blocks here until is able put message in tx queue */
    if (xQueueSend( ipmb_txqueue, &req_cfg, 1) != pdTRUE ){
        return ipmb_error_failure;
    }

    /* Use this notification to block the function while the response does not arrive */
    return ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
}

ipmb_error ipmb_send_response ( ipmi_msg * req, ipmi_msg * resp )
{
    ipmi_msg_cfg resp_cfg;

    /* Builds the message according to the IPMB specification */

    /* Copies data from the response msg struct passed by caller */
    memcpy( &(resp_cfg.buffer), resp, sizeof(ipmi_msg));

    /* Write necessary fields (should be garbage data by now) */
    resp_cfg.buffer.dest_addr = req->src_addr;
    resp_cfg.buffer.netfn = req->netfn + 1;
    resp_cfg.buffer.dest_LUN = req->src_LUN;
    resp_cfg.buffer.src_addr = req->dest_addr;
    resp_cfg.buffer.seq = req->seq;
    resp_cfg.buffer.src_LUN = req->dest_LUN;
    resp_cfg.buffer.cmd = req->cmd;
    resp_cfg.caller_task = xTaskGetCurrentTaskHandle();

    /* Blocks here until is able put message in tx queue */
    if (xQueueSend( ipmb_txqueue, &resp_cfg, portMAX_DELAY) != pdTRUE ){
        return ipmb_error_failure;
    }

    /* Use this notification to block the function while the response does not arrive */
    if ( ulTaskNotifyTake( pdTRUE, 1000 ) != pdTRUE ){
        return ipmb_error_failure;
    }
    return ipmb_error_success;
}

/*! @brief Notifies the client that a new request has arrived and copies the message to its queue.
 * This function receives a message wrapped in a ipmi_msg_cfg struct and copies only the ipmi_msg
 * field to the client queue.
 * Also, if a task has registered its handle in the caller_task field, notify it.
 *
 * @param[in] msg_cfg The message that arrived, wrapped in the configuration struct ipmi_msg_cfg.
 *
 * @retval ipmb_error_success The message was successfully copied.
 * @retval ipmb_error_timeout The client_queue was full.
 */
ipmb_error ipmb_notify_client ( ipmi_msg_cfg * msg_cfg )
{
    configASSERT( client_queue );
    configASSERT( msg_cfg != NULL )
    /* Sends only the ipmi msg, not the control struct */
    if ( xQueueSend( client_queue, &(msg_cfg->buffer), CLIENT_NOTIFY_TIMEOUT ) ) {
        if ( msg_cfg->caller_task ) {
            xTaskNotifyGive( msg_cfg->caller_task );
        }
        return ipmb_error_success;
    }
    return ipmb_error_timeout;
}

ipmb_error ipmb_register_rxqueue ( QueueHandle_t * queue )
{
    configASSERT( queue != NULL );

    *queue = xQueueCreate( IPMB_CLIENT_QUEUE_LEN, sizeof(ipmi_msg) );

    /* Copies the queue handler so we know where to write */
    client_queue = *queue;
    if ( *queue ) {
        return ipmb_error_success;
    } else {
        return ipmb_error_queue_creation;
    }
}

/*! @brief Calculate the IPMB message checksum byte.
 * The cheksum byte is calculated by perfoming a simple 8bit 2's complement of the sum of all previous bytes.
 * Since we're using a unsigned int to hold the checksum value, we only need to subtract all bytes from it.
 * @param buffer Pointer to the message bytes.
 * @param range How many bytes will be used in the calculation.
 *
 * @return Checksum of the specified bytes of the buffer.
 */
uint8_t ipmb_calculate_chksum ( uint8_t * buffer, uint8_t range )
{
    configASSERT( buffer != NULL );
    uint8_t chksum = 0;
    uint8_t i;
    for ( i = 0; i < range; i++ ) {
        chksum -= buffer[i];
    }
    return chksum;
}

/*! @brief Asserts the input message checksums by comparing them with our calculated ones.
 *
 * @param buffer Pointer to the message bytes.
 * @param buffer_len Size of the message.
 *
 * @retval ipmb_error_success The message's checksum bytes are correct, therefore the message is valid.
 * @retval ipmb_error_hdr_chksum The header checksum byte is invalid.
 * @retval ipmb_error_hdr_chksum The final checksum byte is invalid.
 */
ipmb_error ipmb_assert_chksum ( uint8_t * buffer, uint8_t buffer_len )
{
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

/*! @brief Encode IPMI msg struct to a byte formatted buffer
 *
 * This function formats the ipmi_msg struct fields into a byte array, following the specification:
 *
 *| IPMB Messages  |         |          |         |        |
 *|----------------+---------+----------+---------+--------|
 *|                | REQUEST | RESPONSE | Bit Len | Byte # |
 *|----------------+---------+----------+---------+--------|
 *| Connection     | rsSA    | rqSA     |       8 |      1 |
 *| Header         | NetFN   | NetFN    |       6 |      2 |
 *|                | rsLUN   | rqLUN    |       2 |      2 |
 *|----------------+---------+----------+---------+--------|
 *| Header Chksum  | Chksum  | Chksum   |       8 |      3 |
 *|----------------+---------+----------+---------+--------|
 *|                | rqSA    | rsSA     |       8 |      4 |
 *| Callback Info  | rqSeq   | rqSeq    |       6 |      5 |
 *|                | rqLUN   | rsLUN    |       2 |      5 |
 *|----------------+---------+----------+---------+--------|
 *| Command        | CMD     | CMD      |       8 |      6 |
 *|----------------+---------+----------+---------+--------|
 *| Data           |         | CC       |       8 |      7 |
 *|                | Data    | Data     |     8*N |    7+N |
 *|----------------+---------+----------+---------+--------|
 *| Message Chksum | Chksum  | Checksum |       8 |  7+N+1 |
 *|----------------+---------+----------+---------+--------|
 *
 * @param[out] buffer Byte buffer which will hold the formatted message
 * @param[in] msg The message struct to be formatted
 *
 * @retval ipmb_error_success The message was successfully formatted
 */
ipmb_error ipmb_encode ( uint8_t * buffer, ipmi_msg * msg )
{
    configASSERT( msg );
    configASSERT( buffer );
    /* Use this variable to address the buffer dynamically */
    uint8_t i = 0;

    buffer[i++] = msg->dest_addr;
    buffer[i++] = ( ( ( msg->netfn << 2 ) & IPMB_NETFN_MASK ) | ( msg->dest_LUN & IPMB_DEST_LUN_MASK ) );
    buffer[i++] = ipmb_calculate_chksum( &buffer[0], IPMI_HEADER_CHECKSUM_POSITION );
    buffer[i++] = msg->src_addr;
    buffer[i++] = ( ( ( msg->seq << 2 ) & IPMB_SEQ_MASK ) | ( msg->src_LUN & IPMB_SRC_LUN_MASK ) );
    buffer[i++] = msg->cmd;
    buffer[i++] = msg->completion_code;
    memcpy (&buffer[i], &msg->data[0], msg->data_len);
    i += msg->data_len;
    buffer[i] = ipmb_calculate_chksum( &buffer[0], i );

    return ipmb_error_success;
}

/*! @brief Decodes a buffer and copies to its specific fields in a ipmi_msg struct
 *
 * @param[out] msg Pointer to a ipmi_msg struct which will hold the decoded message
 * @param[in] buffer Pointer to a byte array that will be decoded
 * @param[in] len Length of \p buffer
 *
 * @retval ipmb_error_success The message was successfully decoded
 */
ipmb_error ipmb_decode ( ipmi_msg * msg, uint8_t * buffer, uint8_t len )
{
    configASSERT( msg );
    configASSERT( buffer );
    /* Use this variable to address the buffer dynamically */
    uint8_t i = 0;

    msg->dest_addr = buffer[i++];
    msg->netfn = buffer[i] >> 2;
    msg->dest_LUN = ( buffer[i++] & IPMB_DEST_LUN_MASK );
    msg->hdr_chksum = buffer[i++];
    msg->src_addr = buffer[i++];
    msg->seq = buffer[i] >> 2;
    msg->src_LUN = ( buffer[i++] & IPMB_SRC_LUN_MASK );
    msg->cmd = buffer[i++];
    /* Checks if the message is a response and if so, fills the completion code field */
    if ( IS_RESPONSE( (*msg) ) ) {
        msg->completion_code = buffer[i++];
    }
    msg->data_len = len - i;
    memcpy( &msg->data[0], &buffer[i], msg->data_len);

    return ipmb_error_success;
}
