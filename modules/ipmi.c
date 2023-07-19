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

/**
 * @file ipmi.c
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief IPMI module implementation
 * @ingroup IPMI
 */

/* C Standard includes */
#include "string.h"

/* Project includes */
#include "FreeRTOS.h"
#include "ipmi.h"
#include "port.h"
#include "task_priorities.h"
#include "led.h"
#include "payload.h"
#include "uart_debug.h"

/* Local variables */
/**
 * @brief Queue that holds the incoming IPMI messages
 */
QueueHandle_t ipmi_rxqueue = NULL;

/**
 * @brief IPMI handlers list start address
 */
volatile const t_req_handler_record *ipmiEntries = (t_req_handler_record *) &_ipmi_handlers;

/**
 * @brief IPMI handlers list end address
 */
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
#if 0
        printf(" IPMI Message Received: \n ");
        printf(" \tNETFn: 0x%X\tCMD: 0x%X\t Data: ", req_received.netfn, req_received.cmd);
        for (int i=0; i < req_received.data_len; i++) {
            printf("0x%X ", req_received.data[i]);
        }
        printf("\n");
#endif
        req_handler = (t_req_handler) 0;
        req_handler = ipmi_retrieve_handler(req_received.netfn, req_received.cmd);

        if (req_handler != 0) {

            response.completion_code = IPMI_CC_UNSPECIFIED_ERROR;
            response.data_len = 0;

            /// Call user-defined function, give request data and retrieve required response
            /** @warning Since IPMI task have a high priority, this handler function should not wait other tasks to unblock */
            req_handler(&req_received, &response);

            error_code = ipmb_send_response(&req_received, &response);

            /** In case of error during IPMB response, the MMC may wait for a
               new command from the MCH. Check this for debugging purposes
               only. */
            configASSERT( (error_code == ipmb_error_success) );

        } else {
            /** If there is no function handler, use data from received
             *  message to send "invalid command" response (IPMI table 5-2,
             *  page 44). */

            response.completion_code = IPMI_CC_INV_CMD;
            response.data_len = 0;
            error_code = ipmb_send_response(&req_received, &response);

            configASSERT((error_code == ipmb_error_success));
        }
    }
}

TaskHandle_t TaskIPMI_Handle;

void ipmi_init ( void )
{
    ipmb_init();
    ipmb_register_rxqueue( &ipmi_rxqueue );
    xTaskCreate( IPMITask, (const char*)"IPMI Dispatcher", 256, ( void * ) NULL, tskIPMI_PRIORITY, &TaskIPMI_Handle );
}

/**
 * @brief Finds a handler associated with a given netfunction and command.
 *
 * @param[in] netfn 8-bit network function code
 * @param[in] cmd 8-bit command code
 *
 * @return Pointer to the function which will handle this command, as defined in the netfn handler list.
 */
t_req_handler ipmi_retrieve_handler( uint8_t netfn, uint8_t cmd )
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

/**
 * @brief Sends an event message via IPMB interface
 *
 * @param[in] sensor Pointer to sensor structure defined in sensor.h
 * @param[in] assert_deassert Flag to indicate an (de)assertion event
 * @param[in] evData Data buffer holding the event data, size indicated by \p length
 * @param[in] length Lenght of \p evData buffer
 *
 * @return ipmb_error return value from #ipmb_send_request
 *
 * @see ipmb_send_request()
 */
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

/**
 * @brief Handler for GET Device ID command as in IPMI v2.0 section 20.1 for
 * more information.
 *
 * @param[in]  req Pointer to request IPMI message
 *
 * @param[out] rsp Pointer to response IPMI message
 *
 * @return None
 */
IPMI_HANDLER(ipmi_get_device_id,  NETFN_APP, IPMI_GET_DEVICE_ID_CMD, ipmi_msg *req, ipmi_msg* rsp)
{
    int len = rsp->data_len = 0;

    rsp->completion_code = IPMI_CC_OK;

    rsp->data[len++] = IPMI_APP_DEV_ID; /* Dev ID */
    rsp->data[len++] = IPMI_APP_DEV_REV; /* Dev Rev */
    rsp->data[len++] = IPMI_APP_DEV_FW_REV_UPPER; /* Dev FW Rev UPPER */
    rsp->data[len++] = IPMI_APP_DEV_FW_REV_LOWER; /* Dev FW Rev LOWER */
    rsp->data[len++] = IPMI_APP_IPMI_VERSION; /* IPMI Version 2.0 */
    rsp->data[len++] = IPMI_APP_DEV_SUP; /* Dev Support */
    rsp->data[len++] = IPMI_APP_MANUF_LSB; /* Manufacturer ID LSB */
    rsp->data[len++] = IPMI_APP_MANUF_MIDB; /* Manufacturer ID MIDB */
    rsp->data[len++] = IPMI_APP_MANUF_MSB; /* Manufacturer ID MSB */
    rsp->data[len++] = IPMI_APP_PROD_ID_LSB; /* Product ID LSB */
    rsp->data[len++] = IPMI_APP_PROD_ID_MSB; /* Product ID MSB */

    rsp->data_len = len;
}

/**
 * @brief Handler for GET Device GUID command
 *
 * @param[in]  req pointer to request message
 *
 * @param[out] rsp pointer to response message
 *
 * @return None
 */
IPMI_HANDLER(ipmi_get_device_guid,  NETFN_APP, IPMI_GET_DEVICE_GUID_CMD, ipmi_msg *req, ipmi_msg* rsp)
{
    int len = rsp->data_len = 0;

    rsp->completion_code = IPMI_CC_OK;

    /* TODO: Generate GUID - Globally Unique ID */
    memset( &rsp->data[len], 0x00, 16);

    rsp->data_len = 16;
}

/**
 * @brief Handler for GET Properties request. To be called by IPMI
 *  request handler, it must obey the predefined function signature
 *  and protocol. Check IPMI 2.0 table 3-11 for more information.
 *
 * @param[in] req Request to be handled and answered. Unused in
 *  this particular function.
 *
 * @param[out] rsp Message with data, data length and completion code.
 */
IPMI_HANDLER(ipmi_picmg_get_properties, NETFN_GRPEXT,IPMI_PICMG_CMD_GET_PROPERTIES, ipmi_msg *req, ipmi_msg *rsp )
{
    int len = rsp->data_len = 0;
    rsp->completion_code = IPMI_CC_OK;

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;
    rsp->data[len++] = IPMI_EXTENSION_VERSION;
    /* MMCs must report MAX_FRU_ID and FRU_DEVICE_ID both as 0 (if uRTM is implemented, MAX_FRU_ID must be set to 1) - AMC.0 Table 3-1 */
    rsp->data[len++] = MAX_FRU_ID;
    rsp->data[len++] = FRU_DEVICE_ID;
    rsp->data_len = len;
}

/**
 * @brief Handler for Set AMC Port state request.
 * Check IPMI 2.0 table 3-11 for more information.
 *
 * @param[in] req Request to be handled and answered. Unused in
 *  this particular function.
 * @param[out] rsp Message with data, data length and completion code.
 */
IPMI_HANDLER(ipmi_picmg_cmd_set_amc_port_state, NETFN_GRPEXT, IPMI_PICMG_CMD_SET_AMC_PORT_STATE, ipmi_msg *req, ipmi_msg *rsp)
{
    rsp->completion_code = IPMI_CC_OK;
    rsp->data[rsp->data_len++] = IPMI_PICMG_GRP_EXT;
}

IPMI_HANDLER(ipmi_get_device_locator_record, NETFN_GRPEXT, IPMI_PICMG_CMD_GET_DEVICE_LOCATOR_RECORD, ipmi_msg * req, ipmi_msg * rsp )
{
    uint8_t len = rsp->data_len = 0;

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;
    rsp->data[len++] = 0x00;
    rsp->data[len++] = 0x00;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_picmg_cmd_fru_control, NETFN_GRPEXT, IPMI_PICMG_CMD_FRU_CONTROL, ipmi_msg *req, ipmi_msg *rsp)
{
    uint8_t len = rsp->data_len = 0;
    uint8_t fru_id = req->data[1];
    uint8_t fru_ctl = req->data[2];

    rsp->completion_code = IPMI_CC_OK;

    switch (fru_ctl) {
    case FRU_CTLCODE_COLD_RST:
        payload_send_message(fru_id, PAYLOAD_MESSAGE_COLD_RST);
        break;
    case FRU_CTLCODE_WARM_RST:
        payload_send_message(fru_id, PAYLOAD_MESSAGE_WARM_RST);
        break;
    case FRU_CTLCODE_REBOOT:
        payload_send_message(fru_id, PAYLOAD_MESSAGE_REBOOT);
        break;
    case FRU_CTLCODE_QUIESCE:
        payload_send_message(fru_id, PAYLOAD_MESSAGE_QUIESCE);
        break;
    default:
        rsp->completion_code = IPMI_CC_INV_DATA_FIELD_IN_REQ;
        break;
    }

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;
    rsp->data_len = len;
}

IPMI_HANDLER(ipmi_picmg_cmd_get_fru_control_capabilities, NETFN_GRPEXT, IPMI_PICMG_CMD_FRU_CONTROL_CAPABILITIES, ipmi_msg *req, ipmi_msg *rsp)
{
    uint8_t len = rsp->data_len = 0;

    rsp->data[len++] = IPMI_PICMG_GRP_EXT;

    /* FRU Control Capabilities Mask:
     * [7:4] Reserved
     * [3] - Capable of issuing a diagnostic interrupt
     * [2] - Capable of issuing a graceful reboot
     * [1] - Capable of issuing a warm reset */
    rsp->data[len++] = 0x06; /* Graceful reboot and Warm reset */
    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_custom_cmd_get_free_heap, NETFN_CUSTOM, IPMI_CUSTOM_CMD_MMC_GET_FREE_HEAP, ipmi_msg *req, ipmi_msg *rsp)
{
    uint8_t len = rsp->data_len = 0;

    uint32_t free_heap = xPortGetFreeHeapSize();

    rsp->data[len++] = free_heap & 0xFF;
    rsp->data[len++] = (free_heap >> 8) & 0xFF;
    rsp->data[len++] = (free_heap >> 16) & 0xFF;
    rsp->data[len++] = (free_heap >> 24) & 0xFF;

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}
