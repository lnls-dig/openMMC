/*
 * payload.c
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015  Piotr Miedzik <P.Miedzik@gsi.de>
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

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "port.h"
#include "payload.h"
#include "sdr.h"
#include "board_version.h"
#include "pin_mapping.h"
#include "ipmi.h"
#include "task_priorities.h"
#include "adn4604.h"

/* payload states
 *   0 - no power
 *   1 - power switching on
 *       Power Up sequence
 *
 *   2 - power good wait
 *       Since power supply switching
 *       Until detect power good
 *
 *   3 - power good
 *       Here you can configure devices such as clock crossbar and others
 *       We have to reset pin state program b
 *
 *   4 - fpga booting
 *       Since DCDC converters initialization
 *       Until FPGA DONE signal
 *       about 30 sec
 *
 *   5 - fpga working
 *
 *   6 - power switching off
 *       Power-off sequence
 *
 *   7 - power QUIESCED
 *       It continues until a power outage on the line 12v
 *       or for 30 seconds (???)
 *
 * 255 - power fail
 */


void setDC_DC_ConvertersON(bool on) {
    bool _on = on;

    /* @todo: check vadj relationship */
    bool _on_fmc1 = false | on;
    bool _on_fmc2 = false | on;

    gpio_set_pin_state( GPIO_EN_FMC1_PVADJ_PORT, GPIO_EN_FMC1_PVADJ_PIN, _on_fmc1);
    gpio_set_pin_state( GPIO_EM_FMC1_P12V_PORT, GPIO_EM_FMC1_P12V_PIN, _on_fmc1);
    gpio_set_pin_state( GPIO_EN_FMC1_P3V3_PORT, GPIO_EN_FMC1_P3V3_PIN, _on_fmc1);

    gpio_set_pin_state( GPIO_EN_FMC2_PVADJ_PORT, GPIO_EN_FMC2_PVADJ_PIN, _on_fmc2);
    gpio_set_pin_state( GPIO_EM_FMC2_P12V_PORT, GPIO_EM_FMC2_P12V_PIN, _on_fmc2);
    gpio_set_pin_state( GPIO_EN_FMC2_P3V3_PORT, GPIO_EN_FMC2_P3V3_PIN, _on_fmc2);


    gpio_set_pin_state( GPIO_EN_P1V0_PORT, GPIO_EN_P1V0_PIN, _on);
    gpio_set_pin_state( GPIO_EN_P1V8_PORT, GPIO_EN_P1V8_PIN, _on); // <- this one causes problems if not switched off before power loss
    gpio_set_pin_state( GPIO_EN_P1V2_PORT, GPIO_EN_P1V2_PIN, _on);
    gpio_set_pin_state( GPIO_EN_1V5_VTT_PORT, GPIO_EN_1V5_VTT_PIN, _on);
    gpio_set_pin_state( GPIO_EN_P3V3_PORT, GPIO_EN_P3V3_PIN, _on);
}

void initializeDCDC() {
    setDC_DC_ConvertersON(false);
    gpio_set_pin_dir( GPIO_EN_P1V2_PORT, GPIO_EN_P1V2_PIN, true);
    gpio_set_pin_dir( GPIO_EN_P1V8_PORT, GPIO_EN_P1V8_PIN, true);

    gpio_set_pin_dir( GPIO_EN_FMC2_P3V3_PORT, GPIO_EN_FMC2_P3V3_PIN, true);
    gpio_set_pin_dir( GPIO_EN_FMC2_PVADJ_PORT, GPIO_EN_FMC2_PVADJ_PIN, true);
    gpio_set_pin_dir( GPIO_EM_FMC2_P12V_PORT, GPIO_EM_FMC2_P12V_PIN, true);

    gpio_set_pin_dir( GPIO_EM_FMC1_P12V_PORT, GPIO_EM_FMC1_P12V_PIN, true);
    gpio_set_pin_dir( GPIO_EN_FMC1_P3V3_PORT, GPIO_EN_FMC1_P3V3_PIN, true);
    gpio_set_pin_dir( GPIO_EN_FMC1_PVADJ_PORT,  GPIO_EN_FMC1_PVADJ_PIN, true);

    gpio_set_pin_dir( GPIO_EN_P3V3_PORT, GPIO_EN_P3V3_PIN, true);
    gpio_set_pin_dir( GPIO_EN_1V5_VTT_PORT, GPIO_EN_1V5_VTT_PIN, true);
    gpio_set_pin_dir( GPIO_EN_P1V0_PORT, GPIO_EN_P1V0_PIN, true);

    gpio_set_pin_dir( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, true);
}


QueueHandle_t queue_payload_handle = 0;

void payload_send_message(uint8_t msg){
    if (queue_payload_handle == 0)  return;

    xQueueSend(queue_payload_handle, &msg, (TickType_t) 0);

}

TaskHandle_t vTaskPayload_Handle;
void payload_init( void )
{
    xTaskCreate(vTaskPayload, "Payload", configMINIMAL_STACK_SIZE*2, NULL, tskPAYLOAD_PRIORITY, &vTaskPayload_Handle);
    queue_payload_handle = xQueueCreate(16, sizeof(uint8_t));

    initializeDCDC();
    /* Initialize one of the FMC's DCDC so we can measure when the Payload Power is present */
    gpio_set_pin_state( GPIO_EM_FMC1_P12V_PORT, GPIO_EM_FMC1_P12V_PIN, true);
}

void vTaskPayload(void *pvParmeters)
{
    payload_state state = PAYLOAD_NO_POWER;
    payload_state new_state = PAYLOAD_STATE_NO_CHANGE;

    uint8_t P12V_good = 0;
    uint8_t P1V0_good = 0;
    uint8_t FPGA_boot_DONE = 0;
    uint8_t QUIESCED_req = 0;

    ipmi_msg pmsg;
    int data_len = 0;

    uint8_t current_message;

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, true);

    for ( ;; ) {
        new_state = state;

        /* Read all messages from the queue */
        while(xQueueReceive(queue_payload_handle, &current_message, (TickType_t) 0 )) {
            switch (current_message) {
            case PAYLOAD_MESSAGE_P12GOOD:
                P12V_good = 1;
                break;
            case PAYLOAD_MESSAGE_P12GOODn:
                P12V_good = 0;
                break;
            case PAYLOAD_MESSAGE_PGOOD:
                P1V0_good = 1;
                break;
            case PAYLOAD_MESSAGE_PGOODn:
                P1V0_good = 0;
                break;
            case PAYLOAD_MESSAGE_QUIESCED:
                QUIESCED_req = 1;
                break;
            }
        }

        FPGA_boot_DONE = gpio_read_pin( GPIO_DONE_B_PORT, GPIO_DONE_B_PIN);
        P1V0_good = gpio_read_pin( GPIO_PGOOD_P1V0_PORT,GPIO_PGOOD_P1V0_PIN);

        switch(state) {
        case PAYLOAD_NO_POWER:
            if (P12V_good == 1) {
                new_state = PAYLOAD_SWITCHING_ON;
	    }
            QUIESCED_req = 0;
            break;

        case PAYLOAD_SWITCHING_ON:
            setDC_DC_ConvertersON(true);
            new_state = PAYLOAD_POWER_GOOD_WAIT;
            break;

        case PAYLOAD_POWER_GOOD_WAIT:
            if (QUIESCED_req) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if (P1V0_good == 1) {
                new_state = PAYLOAD_STATE_FPGA_SETUP;
            }
            break;

        case PAYLOAD_STATE_FPGA_SETUP:

	    adn4604_setup();

	    /* Pulse PROGRAM_B pin low to reset the FPGA */
	    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, false);
            gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, true);
            new_state = PAYLOAD_FPGA_BOOTING;
            break;

        case PAYLOAD_FPGA_BOOTING:
            if (QUIESCED_req == 1) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if (FPGA_boot_DONE) {
                new_state = PAYLOAD_FPGA_WORKING;
	    }
	    break;

        case PAYLOAD_FPGA_WORKING:
            if (QUIESCED_req == 1) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if (P12V_good == 0) {
		QUIESCED_req = 0;
                new_state = PAYLOAD_NO_POWER;
            }
            break;

        case PAYLOAD_SWITCHING_OFF:
            setDC_DC_ConvertersON(false);
	    sensor_array[HOT_SWAP_SENSOR].data->comparator_status |= HOT_SWAP_STATE_QUIESCED;
            pmsg.dest_LUN = 0;
            pmsg.netfn = NETFN_SE;
            pmsg.cmd = IPMI_PLATFORM_EVENT_CMD;
            pmsg.data[data_len++] = 0x04;
            pmsg.data[data_len++] = 0xf2;
            pmsg.data[data_len++] = HOT_SWAP_SENSOR;
            pmsg.data[data_len++] = 0x6f;
            pmsg.data[data_len++] = HOT_SWAP_QUIESCED; // hot swap state
            pmsg.data_len = data_len;
	    if ( ipmb_send_request( &pmsg ) == ipmb_error_success ) {
		QUIESCED_req = 0;
		new_state = PAYLOAD_NO_POWER;
	    }
            break;

        default:
            break;
        }

        state = new_state;
        vTaskDelayUntil( &xLastWakeTime, PAYLOAD_BASE_DELAY );
    }
}


