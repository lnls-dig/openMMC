/*
 * ipmi.c
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

/* C Standard includes */
#include "string.h"

/* Project includes */
#include "ipmi.h"
#include "pin_mapping.h"
#include "led.h"
#include "task_priorities.h"

/* Local variables */
QueueHandle_t ipmi_rxqueue = NULL;

struct req_param_struct{
    ipmi_msg req_received;
    t_req_handler req_handler;
};

extern t_req_handler_record handlers[MAX_HANDLERS];
extern void ipmi_se_set_receiver ( ipmi_msg *req, ipmi_msg *rsp);
void IPMITask ( void * pvParameters )
{
    ipmi_msg req_received;
    t_req_handler req_handler = (t_req_handler) 0;

    for ( ;; ){
        /* Received request and handler function must be allocated
           dynamically so they can be passed to the dynamically-created
           handler tasks. These tasks must also free the memory after use */

        if( xQueueReceive( ipmi_rxqueue, &req_received , portMAX_DELAY ) == pdFALSE){
            configASSERT(pdFALSE);
            continue;
        }

        req_handler = (t_req_handler) 0;
        req_handler = ipmi_retrieve_handler(req_received.netfn, req_received.cmd);

        if (req_handler != 0){

            /* TODO: create unique name for each created task, probably
               related to netfn and command */
            struct req_param_struct *req_param = pvPortMalloc(sizeof(struct req_param_struct));

            if (req_param != NULL) {
                req_param->req_received = req_received;
                req_param->req_handler = req_handler;
            } else {
                /* TODO: handle this problem */
            }

            if ( xTaskCreate(IPMI_handler_task ,(const char*)"IPMB_handler_task", configMINIMAL_STACK_SIZE*2, req_param, tskIPMI_HANDLERS_PRIORITY,  (TaskHandle_t *) NULL ) != pdTRUE ){
                configASSERT(0);
                /* TODO: handle this problem */
            }

        }else{
            ipmb_error error_code;
            ipmi_msg response;
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


/*!
 * This task is created dynamically each time there is an IPMI request
 * demanding response. It receives a pointer to a struct contaning
 * both the function handler and the request to treated, and must free
 * this struct memory and itself before finishing.
 *
 * @param pvParameters pointer to req_param_struct contaning
 * req_handler and req_received. This is dynamically allocated memory
 * and should be freed before quitting.
 */
void IPMI_handler_task( void * pvParameters){
    struct req_param_struct * req_param = (struct req_param_struct *) pvParameters;
    ipmi_msg response;
    ipmb_error response_error;

    response.completion_code = IPMI_CC_UNSPECIFIED_ERROR;
    response.data_len = 0;
    /* Call user-defined function, give request data and retrieve required response */
    req_param->req_handler(&(req_param->req_received), &response);

    response_error = ipmb_send_response(&(req_param->req_received), &response);

    /* In case of error during IPMB response, the MMC may wait for a
       new command from the MCH. Check this for debugging purposes
       only. */
    configASSERT( response_error == ipmb_error_success );

    vPortFree(req_param);

    vTaskDelete(NULL);
}

/* Initializes the IPMI Dispatcher:
 * -> Initializes the IPMB Layer
 * -> Registers the RX queue for incoming requests
 * -> Creates the IPMI task
 */
void ipmi_init ( void )
{
    ipmb_init();
    ipmb_register_rxqueue( &ipmi_rxqueue );
    xTaskCreate( IPMITask, (const char*)"IPMI Dispatcher", configMINIMAL_STACK_SIZE*4, ( void * ) NULL, tskIPMI_PRIORITY, ( TaskHandle_t * ) NULL );
}


/*!
 * @brief Finds a handler associated with a given netfunction and command.
 *
 * @param netfn 8-bit network function code
 * @param cmd 8-bit command code
 *
 * @return Pointer to the function which will handle this command, as defined in the netfn handler list.
 */
t_req_handler ipmi_retrieve_handler(uint8_t netfn, uint8_t cmd){
    uint8_t cur_handler;
    t_req_handler handler = 0;

    for(cur_handler = 0; cur_handler < MAX_HANDLERS; cur_handler++)
    {

        if( (handlers[cur_handler].netfn == netfn) && \
            (handlers[cur_handler].cmd == cmd))
        {
            handler = handlers[cur_handler].req_handler;
            break;
        }
    }

    return handler;
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
void ipmi_app_get_device_id ( ipmi_msg *req, ipmi_msg * rsp ){
    int len = rsp->data_len = 0;

    rsp->completion_code = IPMI_CC_OK;

    /*! @todo: several bits of hardcoded information. Organize it so it
      makes more sense and is easier to modify. */

    rsp->data[len++] = 0x0A; /* Dev ID */
    rsp->data[len++] = 0x02; /* Dev Rev */
    rsp->data[len++] = 0x05; /* Dev FW Rev UPPER */
    rsp->data[len++] = 0x50; /* Dev FW Rev LOWER */
    rsp->data[len++] = 0x02; /* IPMI Version 2.0 */
    rsp->data[len++] = 0x1F; /* Dev Support */
    rsp->data[len++] = 0x5A; /* Manufacturer ID LSB */
    rsp->data[len++] = 0x31; /* Manufacturer ID MSB */
    rsp->data[len++] = 0x00; /* ID MSB */
    rsp->data[len++] = 0x01; /* Product ID LSB */
    rsp->data[len++] = 0x01; /* Product ID MSB */

    rsp->data_len = len;

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
void ipmi_picmg_get_properties ( ipmi_msg *req, ipmi_msg *rsp )
{
    int len = rsp->data_len = 0;
    rsp->completion_code = IPMI_CC_OK;

    /* Hardcoded response according to the  */
    rsp->data[len++] = IPMI_PICMG_GRP_EXT;
    rsp->data[len++] = IPMI_EXTENSION_VERSION;
    rsp->data[len++] = MAX_FRU_ID;
    rsp->data[len++] = FRU_DEVICE_ID;
    rsp->data_len = len;
}


/*!
 * @brief Handler for "Set FRU LED State"" request. Check IPMI 2.0
 * table 3-31 for more information.
 *
 * @param[in] req Pointer to request struct to be handled and answered. Contains
 * which LED should be set, how it should be set and other commands.
 *
 * @param[out] rsp Pointer to response struct to be modified with the message
 *
 * @return void
 */
#include "led.h"
void ipmi_picmg_set_led ( ipmi_msg *req, ipmi_msg *rsp )
{
    led_error error;
    const LED_activity_desc_t * pLEDact;
    LED_activity_desc_t LEDact;
    pLEDact = &LEDact;
    /* We use this pointer assignment, so we can also set it to NULL if we need */

    switch (req->data[3]) {
    case 0x00:
        /* OFF override */
        pLEDact = &LED_Off_Activity;
        break;
    case 0xFF:
        /* ON override */
        pLEDact = &LED_On_Activity;
        break;
    case 0xFB:
        /* Lamp Test */
        /*! @todo Put the lamp test as a higher priority action, not a type of override */
        LEDact.action = LED_ACTV_BLINK;
        LEDact.initstate = LED_ON_STATE;
        /* On duration in 100ms units */
        LEDact.delay_init = req->data[4] * 100;
        /* Set the toggle delay to 0, so we know its a "single-shot" descriptor, so the LED module should revert to its override/local_control state later */
        LEDact.delay_tog = 0;
        break;
    case 0xFC:
        /* Local state */
	pLEDact = NULL;
        break;
    case 0xFD:
    case 0xFE:
        /* Reserved */
        break;
    default:
        /* Blink Override */
        LEDact.action = LED_ACTV_BLINK;
        LEDact.initstate = LED_ON_STATE;
        /* On duration in 10ms units */
        LEDact.delay_init = req->data[4] / 10;
        /* Off duration in 10ms units*/
        LEDact.delay_tog = req->data[3] / 10;
        break;
    }

    /* If this function does not block, we can't assure we have programmed the LED correctly, but if it does, there's the risk that this task won't kill itself and we'll run out of heap space */
    error = LED_update( req->data[2], pLEDact );

    switch (error) {
    case led_success:
        rsp->completion_code = IPMI_CC_OK;
        break;
    case led_invalid_argument:
        rsp->completion_code = IPMI_CC_INV_DATA_FIELD_IN_REQ;
        break;
    case led_unspecified_error:
        rsp->completion_code = IPMI_CC_UNSPECIFIED_ERROR;
        break;
    }
    rsp->data_len = 0;
    rsp->data[rsp->data_len++] = IPMI_PICMG_GRP_EXT;
}

void ipmi_picmg_set_amc_port( ipmi_msg *req, ipmi_msg *rsp)
{
    rsp->completion_code = IPMI_CC_OK;
    rsp->data[rsp->data_len++] = IPMI_PICMG_GRP_EXT;
}

#include "payload.h"
void ipmi_picmg_fru_control( ipmi_msg *req, ipmi_msg *rsp)
{
    payload_send_message(PAYLOAD_MESSAGE_QUIESCED);
    rsp->completion_code = IPMI_CC_OK;
    rsp->data[rsp->data_len++] = IPMI_PICMG_GRP_EXT;
}
