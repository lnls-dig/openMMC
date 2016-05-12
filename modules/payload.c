/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
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

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Project Includes */
#include "port.h"
#include "payload.h"
#include "pin_mapping.h"
#include "ipmi.h"
#include "task_priorities.h"
#include "adn4604.h"
#include "ad84xx.h"
#include "hotswap.h"
#include "utils.h"

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

static TickType_t last_time;

void EINT2_IRQHandler( void )
{
    TickType_t current_time = xTaskGetTickCountFromISR();

    /* Simple debouncing routine */
    /* If the last interruption happened in the last 200ms, this one is only a bounce, ignore it and wait for the next interruption */
    if (getTickDifference(current_time, last_time) > DEBOUNCE_TIME) {
        gpio_clr_pin(GPIO_FPGA_RESET_PORT, GPIO_FPGA_RESET_PIN);
        asm("NOP");
        gpio_set_pin(GPIO_FPGA_RESET_PORT, GPIO_FPGA_RESET_PIN);

        last_time = current_time;
    }
    /* Clear interruption flag */
    LPC_SYSCTL->EXTINT |= (1 << 2);
}

void setDC_DC_ConvertersON( bool on )
{
    bool _on = on;

    /* @todo: check vadj relationship */
    bool _on_fmc1 = false | on;
    bool _on_fmc2 = false | on;

    gpio_set_pin_state( GPIO_EN_FMC1_PVADJ_PORT, GPIO_EN_FMC1_PVADJ_PIN, _on_fmc1);
    //gpio_set_pin_state( GPIO_EN_FMC1_P12V_PORT, GPIO_EN_FMC1_P12V_PIN, _on_fmc1);
    gpio_set_pin_state( GPIO_EN_FMC1_P3V3_PORT, GPIO_EN_FMC1_P3V3_PIN, _on_fmc1);

    gpio_set_pin_state( GPIO_EN_FMC2_PVADJ_PORT, GPIO_EN_FMC2_PVADJ_PIN, _on_fmc2);
    gpio_set_pin_state( GPIO_EN_FMC2_P12V_PORT, GPIO_EN_FMC2_P12V_PIN, _on_fmc2);
    gpio_set_pin_state( GPIO_EN_FMC2_P3V3_PORT, GPIO_EN_FMC2_P3V3_PIN, _on_fmc2);


    gpio_set_pin_state( GPIO_EN_P1V0_PORT, GPIO_EN_P1V0_PIN, _on);
    gpio_set_pin_state( GPIO_EN_P1V8_PORT, GPIO_EN_P1V8_PIN, _on); // <- this one causes problems if not switched off before power loss
    gpio_set_pin_state( GPIO_EN_P1V2_PORT, GPIO_EN_P1V2_PIN, _on);
    gpio_set_pin_state( GPIO_EN_1V5_VTT_PORT, GPIO_EN_1V5_VTT_PIN, _on);
    gpio_set_pin_state( GPIO_EN_P3V3_PORT, GPIO_EN_P3V3_PIN, _on);
}

void initializeDCDC( void )
{
    setDC_DC_ConvertersON(false);
    gpio_set_pin_dir( GPIO_EN_P1V2_PORT, GPIO_EN_P1V2_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_P1V8_PORT, GPIO_EN_P1V8_PIN, OUTPUT);

    gpio_set_pin_dir( GPIO_EN_FMC2_P3V3_PORT, GPIO_EN_FMC2_P3V3_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_FMC2_PVADJ_PORT, GPIO_EN_FMC2_PVADJ_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_FMC2_P12V_PORT, GPIO_EN_FMC2_P12V_PIN, OUTPUT);

    gpio_set_pin_dir( GPIO_EN_FMC1_P12V_PORT, GPIO_EN_FMC1_P12V_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_FMC1_P3V3_PORT, GPIO_EN_FMC1_P3V3_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_FMC1_PVADJ_PORT,  GPIO_EN_FMC1_PVADJ_PIN, OUTPUT);

    gpio_set_pin_dir( GPIO_EN_P3V3_PORT, GPIO_EN_P3V3_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_1V5_VTT_PORT, GPIO_EN_1V5_VTT_PIN, OUTPUT);
    gpio_set_pin_dir( GPIO_EN_P1V0_PORT, GPIO_EN_P1V0_PIN, OUTPUT);
}


QueueHandle_t queue_payload_handle = 0;

void payload_send_message(uint8_t msg)
{
    if (queue_payload_handle) {
        xQueueSend(queue_payload_handle, &msg, (TickType_t) 0);
    }
}

TaskHandle_t vTaskPayload_Handle;

void payload_init( void )
{
    xTaskCreate(vTaskPayload, "Payload", 120, NULL, tskPAYLOAD_PRIORITY, &vTaskPayload_Handle);

    /** @todo: Use event groups instead of queues here */
    queue_payload_handle = xQueueCreate(5, sizeof(uint8_t));

    initializeDCDC();

#ifdef MODULE_DAC_AD84XX
    /* Configure the PVADJ DAC */
    dac_vadj_init();
    dac_vadj_config( 0, 25 );
    dac_vadj_config( 1, 25 );
#endif

    /* Configure FPGA reset button interruption on front panel */
    Chip_IOCON_PinMux(LPC_IOCON, GPIO_FRONT_BUTTON_PORT, GPIO_FRONT_BUTTON_PIN, IOCON_MODE_INACT, IOCON_FUNC1);
    irq_set_priority( EINT2_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1 );
    irq_enable( EINT2_IRQn );

    //if (board_info.board_version == BOARD_VERSION_AFC_V3_1) {
        /* Flash CS Mux */
        /* 0 = FPGA reads bitstream from Program memory
         * 1 = FPGA reads bitstream from User memory
         */
        gpio_set_pin_dir(0, 19, OUTPUT);
        gpio_set_pin_state(0, 19, LOW);

        /* Init_B */
        /* TODO: Check Init_b pin for error on initialization, then use it as output control */

        gpio_set_pin_dir(0, 20, OUTPUT);
        gpio_set_pin_state(0, 20, HIGH);
    //}
}

void vTaskPayload(void *pvParameters)
{
    payload_state state = PAYLOAD_NO_POWER;
    payload_state new_state = PAYLOAD_STATE_NO_CHANGE;

    uint8_t P12V_good = 0;
    uint8_t P1V0_good = 0;
    uint8_t FPGA_boot_DONE = 0;
    uint8_t QUIESCED_req = 0;

    uint8_t current_message;

    extern sensor_t * hotswap_amc_sensor;

    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, HIGH);

    for ( ;; ) {

        /* Initialize one of the FMC's DCDC so we can measure when the Payload Power is present */
        gpio_set_pin_state( GPIO_EN_FMC1_P12V_PORT, GPIO_EN_FMC1_P12V_PIN, HIGH);

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
	    case PAYLOAD_MESSAGE_COLD_RST:
		state = PAYLOAD_SWITCHING_OFF;
		break;
	    case PAYLOAD_MESSAGE_REBOOT:
		gpio_clr_pin(GPIO_FPGA_RESET_PORT, GPIO_FPGA_RESET_PIN);
		asm("NOP");
		gpio_set_pin(GPIO_FPGA_RESET_PORT, GPIO_FPGA_RESET_PIN);
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
	    hotswap_clear_mask_bit( HOTSWAP_AMC, HOTSWAP_BACKEND_PWR_SHUTDOWN_MASK );
	    hotswap_clear_mask_bit( HOTSWAP_AMC, HOTSWAP_BACKEND_PWR_FAILURE_MASK );
	    if (QUIESCED_req) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if (P1V0_good == 1) {
                new_state = PAYLOAD_STATE_FPGA_SETUP;
            }
            break;

        case PAYLOAD_STATE_FPGA_SETUP:
#ifdef MODULE_CLOCK_SWITCH
            adn4604_setup();
#endif
            new_state = PAYLOAD_FPGA_BOOTING;
            break;

        case PAYLOAD_FPGA_BOOTING:
            if (QUIESCED_req == 1 || P12V_good == 0) {
                new_state = PAYLOAD_SWITCHING_OFF;
            } else if (FPGA_boot_DONE) {
                new_state = PAYLOAD_FPGA_WORKING;
            }
            break;

        case PAYLOAD_FPGA_WORKING:
            if (QUIESCED_req == 1 || P12V_good == 0) {
                new_state = PAYLOAD_SWITCHING_OFF;
            }
            break;

        case PAYLOAD_SWITCHING_OFF:
            setDC_DC_ConvertersON(false);
	    /*
	    hotswap_set_mask_bit( HOTSWAP_AMC, HOTSWAP_BACKEND_PWR_SHUTDOWN_MASK );
	    hotswap_send_event( hotswap_amc_sensor, HOTSWAP_BACKEND_PWR_SHUTDOWN_MASK );
	    */
	    hotswap_set_mask_bit( HOTSWAP_AMC, HOTSWAP_QUIESCED_MASK );
	    if ( hotswap_send_event( hotswap_amc_sensor, HOTSWAP_QUIESCED_MASK ) == ipmb_error_success ) {
                QUIESCED_req = 0;
		/* Reset the power good flags to avoid the state machine to start over without a new read from the sensors */
		P12V_good = 0;
		P1V0_good = 0;
                new_state = PAYLOAD_NO_POWER;
		hotswap_clear_mask_bit( HOTSWAP_AMC, HOTSWAP_QUIESCED_MASK );
            }
            break;

        default:
            break;
        }

        state = new_state;
        vTaskDelayUntil( &xLastWakeTime, PAYLOAD_BASE_DELAY );
    }
}

IPMI_HANDLER(ipmi_picmg_cmd_fru_control, NETFN_GRPEXT, IPMI_PICMG_CMD_FRU_CONTROL, ipmi_msg *req, ipmi_msg *rsp)
{
    uint8_t len = rsp->data_len = 0;
    uint8_t fru_ctl = req->data[2];

    rsp->completion_code = IPMI_CC_OK;

    switch (fru_ctl) {
    case FRU_CTLCODE_COLD_RST:
	payload_send_message(PAYLOAD_MESSAGE_COLD_RST);
	break;
    case FRU_CTLCODE_WARM_RST:
	payload_send_message(PAYLOAD_MESSAGE_WARM_RST);
	break;
    case FRU_CTLCODE_REBOOT:
	payload_send_message(PAYLOAD_MESSAGE_REBOOT);
	break;
    case FRU_CTLCODE_QUIESCE:
	payload_send_message(PAYLOAD_MESSAGE_QUIESCED);
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


IPMI_HANDLER(ipmi_picmg_cmd_set_fru_activation_policy, NETFN_GRPEXT, IPMI_PICMG_CMD_SET_FRU_ACTIVATION_POLICY, ipmi_msg *req, ipmi_msg *rsp)
{
    uint8_t len = rsp->data_len = 0;

    /* FRU Activation Policy Mask */
    uint8_t fru_actv_mask = req->data[2];
    uint8_t fru_actv_bits = req->data[3];

    /* TODO: Implement FRU activation policy */
    rsp->data[len++] = IPMI_PICMG_GRP_EXT;
    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

/* HPM Functions */
#ifdef MODULE_HPM

#include "flash_spi.h"
#include "string.h"

uint8_t hpm_page[256];
uint8_t hpm_pg_index;
uint32_t hpm_page_addr;

uint8_t payload_hpm_prepare_comp( void )
{
    /* Initialize variables */
    memset(hpm_page, 0xFF, sizeof(hpm_page));
    hpm_pg_index = 0;
    hpm_page_addr = 0;

    /* Initialize flash */
    ssp_init( FLASH_SPI, FLASH_SPI_BITRATE, FLASH_SPI_FRAME_SIZE, SSP_MASTER, SSP_INTERRUPT );

    /* Prevent the FPGA from accessing the Flash to configure itself now */
    gpio_set_pin_dir( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, OUTPUT);
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, HIGH);
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, LOW);
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, HIGH);
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, LOW);

    /* Erase FLASH */
    flash_bulk_erase();

    return IPMI_CC_COMMAND_IN_PROGRESS;
}

uint8_t payload_hpm_upload_block( uint8_t * block, uint16_t size )
{
    /* TODO: Check DONE pin before accessing the SPI bus, since the FPGA may be reading it in order to boot */
    uint8_t remaining_bytes_start;

    if ( sizeof(hpm_page) - hpm_pg_index > size) {
        /* Our page is not full yet, just append the new data */
        memcpy(&hpm_page[hpm_pg_index], block, size);
        hpm_pg_index += size;

        return IPMI_CC_OK;

    } else {
        /* Complete the remaining bytes on the buffer */
        memcpy(&hpm_page[hpm_pg_index], block, (sizeof(hpm_page) - hpm_pg_index));
        remaining_bytes_start = (sizeof(hpm_page) - hpm_pg_index);

        /* Program the complete page in the Flash */
        flash_program_page( hpm_page_addr, &hpm_page[0], sizeof(hpm_page));

        hpm_page_addr += sizeof(hpm_page);

        /* Empty our buffer and reset the index */
        memset(hpm_page, 0xFF, sizeof(hpm_page));
        hpm_pg_index = 0;

        /* Save the trailing bytes */
        memcpy(&hpm_page[hpm_pg_index], block+remaining_bytes_start, size-remaining_bytes_start);

        hpm_pg_index = size-remaining_bytes_start;

        return IPMI_CC_COMMAND_IN_PROGRESS;
    }
}

uint8_t payload_hpm_finish_upload( uint32_t image_size )
{
    /* Check if the last page was already programmed */
    if (!hpm_pg_index) {
        /* Program the complete page in the Flash */
        flash_program_page( hpm_page_addr, &hpm_page[0], (sizeof(hpm_page)-hpm_pg_index));
        hpm_pg_index = 0;
        hpm_page_addr = 0;

        return IPMI_CC_COMMAND_IN_PROGRESS;
    }

    return IPMI_CC_OK;
}

uint8_t payload_hpm_get_upgrade_status( void )
{
    if (is_flash_busy()) {
        return IPMI_CC_COMMAND_IN_PROGRESS;
    } else {
        return IPMI_CC_OK;
    }
}

uint8_t payload_hpm_activate_firmware( void )
{
    /* Reset FPGA - Pulse PROGRAM_B pin */
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, LOW);
    gpio_set_pin_state( GPIO_PROGRAM_B_PORT, GPIO_PROGRAM_B_PIN, HIGH);

    return IPMI_CC_OK;
}
#endif
