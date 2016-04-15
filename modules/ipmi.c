/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
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

/* C Standard includes */
#include "string.h"

/* Project includes */
#include "FreeRTOS.h"
#include "ipmi.h"
#include "pin_mapping.h"
#include "task_priorities.h"
#include "led.h"
#include "payload.h"

/* Local variables */
QueueHandle_t ipmi_rxqueue = NULL;

volatile const t_req_handler_record *ipmiEntries = (t_req_handler_record *) &_ipmi_handlers;
volatile const t_req_handler_record *ipmiEntries_end = (t_req_handler_record *) &_eipmi_handlers;

void IPMITask( void * pvParameters )
{
    ipmi_msg req_received;
    ipmi_msg response;
    ipmb_error error_code;
    t_req_handler req_handler = (t_req_handler) 0;

    for ( ;; ) {

        if( xQueueReceive( ipmi_rxqueue, &req_received , portMAX_DELAY ) == pdFALSE) {
	    /* Should no return pdFALSE */
            configASSERT(pdFALSE);
            continue;
        }

        req_handler = (t_req_handler) 0;
        req_handler = ipmi_retrieve_handler(req_received.netfn, req_received.cmd);

        if (req_handler != 0) {

            response.completion_code = IPMI_CC_UNSPECIFIED_ERROR;
            response.data_len = 0;

            /* Call user-defined function, give request data and retrieve required response */
	    /* WARNING: Since IPMI task have a high priority, this handler function should not wait other tasks to unblock */
            req_handler(&req_received, &response);

            error_code = ipmb_send_response(&req_received, &response);

            /* In case of error during IPMB response, the MMC may wait for a
               new command from the MCH. Check this for debugging purposes
               only. */
            configASSERT( (error_code == ipmb_error_success) );

        } else {
            /* If there is no function handler, use data from received
               message to send "invalid command" response (IPMI table 5-2,
               page 44). */

            response.completion_code = IPMI_CC_INV_CMD;
            response.data_len = 0;
            error_code = ipmb_send_response(&req_received, &response);

            configASSERT((error_code == ipmb_error_success));
        }
    }
}

/* Initializes the IPMI Dispatcher:
 * -> Initializes the IPMB Layer
 * -> Registers the RX queue for incoming requests
 * -> Creates the IPMI task
 */
TaskHandle_t TaskIPMI_Handle;

void ipmi_init ( void )
{
    ipmb_init();
    ipmb_register_rxqueue( &ipmi_rxqueue );
    xTaskCreate( IPMITask, (const char*)"IPMI Dispatcher", 100, ( void * ) NULL, tskIPMI_PRIORITY, &TaskIPMI_Handle );
}

/*!
 * @brief Finds a handler associated with a given netfunction and command.
 *
 * @param netfn 8-bit network function code
 * @param cmd 8-bit command code
 *
 * @return Pointer to the function which will handle this command, as defined in the netfn handler list.
 */
t_req_handler ipmi_retrieve_handler(uint8_t netfn, uint8_t cmd)
{
    t_req_handler handler = 0;
    t_req_handler_record * p_ptr = (t_req_handler_record *) ipmiEntries;

    while (p_ptr < ipmiEntries_end) {
        if( (p_ptr->netfn == netfn) &&   \
            (p_ptr->cmd == cmd)) {
            handler = p_ptr->req_handler;
            break;
        }
	p_ptr++;
    }

    return handler;
}

ipmb_error ipmi_event_send( sensor_t * sensor, uint8_t assert_deassert, uint8_t *evData, uint8_t length)
{
    ipmi_msg evt;
    uint8_t data_len = 0;

    evt.dest_LUN = 0;
    evt.netfn = NETFN_SE;
    evt.cmd = IPMI_PLATFORM_EVENT_CMD;

    evt.data[data_len++] = IPMI_EVENT_MESSAGE_REV;
    evt.data[data_len++] = GET_SENSOR_TYPE(sensor);
    evt.data[data_len++] = sensor->num;
    evt.data[data_len++] = assert_deassert | (GET_EVENT_TYPE_CODE(sensor) & 0x7F);
    evt.data[data_len++] = (length >= 1)? evData[0] : 0xFF;
    evt.data[data_len++] = (length >= 2)? evData[1] : 0xFF;
    evt.data[data_len++] = (length >= 3)? evData[2] : 0xFF;

    evt.data_len = data_len;

    return (ipmb_send_request( &evt ));
}


/*!
 * @brief Handler for GET Device ID command as in IPMI v2.0 section 20.1 for
 * more information.
 *
 * @param req pointer to request message
 *
 * @param rsp pointer to response message
 *
 * @return
 */
IPMI_HANDLER(ipmi_get_device_id,  NETFN_APP, IPMI_GET_DEVICE_ID_CMD, ipmi_msg *req, ipmi_msg* rsp)
{
    int len = rsp->data_len = 0;

    rsp->completion_code = IPMI_CC_OK;

    /*! @todo: several bits of hardcoded information. Organize it so it
      makes more sense and is easier to modify. */

    rsp->data[len++] = 0x0A; /* Dev ID */
    rsp->data[len++] = 0x02; /* Dev Rev */
    rsp->data[len++] = 0x05; /* Dev FW Rev UPPER */
    rsp->data[len++] = 0x50; /* Dev FW Rev LOWER */
    rsp->data[len++] = 0x02; /* IPMI Version 2.0 */
    rsp->data[len++] = 0x3B; /* Dev Support */
    rsp->data[len++] = 0x5A; /* Manufacturer ID LSB */
    rsp->data[len++] = 0x31; /* Manufacturer ID MSB */
    rsp->data[len++] = 0x00; /* ID MSB */
    rsp->data[len++] = 0x01; /* Product ID LSB */
    rsp->data[len++] = 0x01; /* Product ID MSB */

    rsp->data_len = len;
}

/*!
 * @brief Handler for GET Device GUID command
 *
 * @param req pointer to request message
 *
 * @param rsp pointer to response message
 *
 * @return
 */
IPMI_HANDLER(ipmi_get_device_guid,  NETFN_APP, IPMI_GET_DEVICE_GUID_CMD, ipmi_msg *req, ipmi_msg* rsp)
{
    int len = rsp->data_len = 0;

    rsp->completion_code = IPMI_CC_OK;

    /* TODO: Generate GUID - Globally Unique ID */
    memset( &rsp->data[len], 0x00, 16);

    rsp->data_len = 16;
}

/*!
 * @brief handler for GET Properties request. To be called by IPMI
 *  request handler, it must obey the predefined function signature
 *  and protocol. Check IPMI 2.0 table 3-11 for more information.
 *
 * @param[in] request Request to be handled and answered. Unused in
 *  this particular function.
 *
 * @return ipmi_msg Message with data, data length and completion code.
 */
IPMI_HANDLER(ipmi_picmg_get_properties, NETFN_GRPEXT,IPMI_PICMG_CMD_GET_PROPERTIES, ipmi_msg *req, ipmi_msg *rsp )
{
    int len = rsp->data_len = 0;
    rsp->completion_code = IPMI_CC_OK;

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;
    rsp->data[len++] = IPMI_EXTENSION_VERSION;
    /* MMCs must report MAX_FRU_ID and FRU_DEVICE_ID both as 0 - AMC.0 Table 3-1 */
    rsp->data[len++] = MAX_FRU_ID;
    rsp->data[len++] = FRU_DEVICE_ID;
    rsp->data_len = len;
}

IPMI_HANDLER(ipmi_picmg_cmd_set_amc_port_state, NETFN_GRPEXT, IPMI_PICMG_CMD_SET_AMC_PORT_STATE, ipmi_msg *req, ipmi_msg *rsp)
{
    rsp->completion_code = IPMI_CC_OK;
    rsp->data[rsp->data_len++] = IPMI_PICMG_GRP_EXT;
}

/* Compatibility with Vadatech */
IPMI_HANDLER(ipmi_get_device_locator_record, NETFN_GRPEXT, IPMI_PICMG_CMD_GET_DEVICE_LOCATOR_RECORD, ipmi_msg * req, ipmi_msg * rsp )
{
    uint8_t len = rsp->data_len = 0;

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;
    rsp->data[len++] = 0x00;
    rsp->data[len++] = 0x00;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

