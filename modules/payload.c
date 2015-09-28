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

void payload_init( void )
{
    xTaskCreate(vTaskPayload, "Payload", configMINIMAL_STACK_SIZE, NULL, tskPAYLOAD_PRIORITY, (TaskHandle_t *) NULL);
    queue_payload_handle = xQueueCreate(16, sizeof(uint8_t));

    initializeDCDC();
}

extern I2C_Mutex i2c_mutex_array[2];

void vTaskPayload(void *pvParmeters)
{
    payload_state state = PAYLOAD_NO_POWER;
    payload_state new_state = PAYLOAD_STATE_NO_CHANGE;
    I2C_ID_T i2c_bus_id;

    TickType_t xDelay = 10;

    uint8_t P12V_good = 0;
    uint8_t P1V0_good = 0;
    uint8_t FPGA_boot_DONE = 0;
    uint8_t QUIESCED_req = 0;

    uint8_t current_message;

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, true);

    for ( ;; ) {
        new_state = state;
        xDelay = 10;

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
                xDelay = 0;
            }
            QUIESCED_req = 0;
            break;

        case PAYLOAD_SWITCHING_ON:
            xDelay = 10;
            //gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, true);
            setDC_DC_ConvertersON(true);
            new_state = PAYLOAD_POWER_GOOD_WAIT;
	    break;

        case PAYLOAD_POWER_GOOD_WAIT:
            if (P1V0_good == 1) {
                xDelay = 1000;
                new_state = PAYLOAD_STATE_FPGA_SETUP;
            }
            break;

	case PAYLOAD_STATE_FPGA_SETUP:
            // @todo: things like reconfiguring clock crossbar
            //gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, false);

            //vTaskDelayUntil( &xLastWakeTime, 1000 );
#ifdef ADN_CFG
	    if (afc_i2c_take_by_chipid(CHIP_ID_ADN, NULL, &i2c_bus_id, 100 ) == pdTRUE) {
                adn4604_setup(i2c_bus_id);
                afc_i2c_give(i2c_bus_id);
            }
#endif
            xDelay = 0;
            //xDelay = 1000;
	    // gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, true);
            new_state = PAYLOAD_FPGA_BOOTING;
            break;

        case PAYLOAD_FPGA_BOOTING:
            if (QUIESCED_req == 1) {
                new_state = PAYLOAD_SWITCHING_OFF;
                xDelay = 0;
            } else if (FPGA_boot_DONE) {
                new_state = PAYLOAD_FPGA_WORKING;
                xDelay = 0;
            }
            QUIESCED_req = 0;
            break;

	case PAYLOAD_FPGA_WORKING:
            if (QUIESCED_req == 1) {
                new_state = PAYLOAD_SWITCHING_OFF;
                xDelay = 0;
            } else if (P12V_good == 0) {
                new_state = PAYLOAD_POWER_FAIL;
            }
            QUIESCED_req = 0;
            break;

	case PAYLOAD_SWITCHING_OFF:
            xDelay = 0;
            QUIESCED_req = 0;
            new_state = PAYLOAD_QUIESCED;

            setDC_DC_ConvertersON(false);
            do_quiesced(FRU_CTLCODE_QUIESCE);
            // @todo: emit QUIESCED event
            break;

	case PAYLOAD_QUIESCED:
            if (P12V_good == 0) {
                new_state = PAYLOAD_NO_POWER;
            }
            QUIESCED_req = 0;
            break;

	case PAYLOAD_POWER_FAIL:
            QUIESCED_req = 0;
            break;

	default:
            break;
        }

        // wait only if no
        if (xDelay != 0) {
            vTaskDelayUntil( &xLastWakeTime, xDelay );
	}
        state = new_state;
    }
}
