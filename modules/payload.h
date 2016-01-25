/*
 * payload.h
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

#ifndef IPMI_PAYLOAD_H_
#define IPMI_PAYLOAD_H_

typedef enum {
    PAYLOAD_NO_POWER = 0,
    PAYLOAD_SWITCHING_ON,
    PAYLOAD_POWER_GOOD_WAIT,
    PAYLOAD_STATE_FPGA_SETUP,
    PAYLOAD_FPGA_BOOTING,
    PAYLOAD_FPGA_WORKING,
    PAYLOAD_SWITCHING_OFF,
    PAYLOAD_QUIESCED,
    PAYLOAD_OFF,
    PAYLOAD_STATE_NO_CHANGE = 253,
    PAYLOAD_STATE_UNKNOWN = 254,
    PAYLOAD_POWER_FAIL = 255
} payload_state;

typedef enum {
    PAYLOAD_MESSAGE_P12GOOD,
    PAYLOAD_MESSAGE_P12GOODn,
    PAYLOAD_MESSAGE_PGOOD,
    PAYLOAD_MESSAGE_PGOODn,
    PAYLOAD_MESSAGE_COLD_RST,
    PAYLOAD_MESSAGE_WARM_RST,
    PAYLOAD_MESSAGE_REBOOT,
    PAYLOAD_MESSAGE_QUIESCED
} payload_message;

#define FRU_CTLCODE_COLD_RST          (0)       // FRU Control command cold reset code
#define FRU_CTLCODE_WARM_RST          (1)       // FRU Control command warm reset code
#define FRU_CTLCODE_REBOOT            (2)       // FRU Control command reboot code
#define FRU_CTLCODE_QUIESCE           (4)       // FRU Control command quiesce code

#define PAYLOAD_BASE_DELAY 100

void payload_send_message(uint8_t msg);
void vTaskPayload(void *pvParmeters);
void payload_init(void);

#ifdef MODULE_HPM
uint8_t payload_hpm_prepare_comp( void );
uint8_t payload_hpm_upload_block( uint8_t * block, uint16_t size );
uint8_t payload_hpm_finish_upload( uint32_t image_size );
uint8_t payload_hpm_get_upgrade_status( void );
uint8_t payload_hpm_activate_firmware( void );
#endif

#endif /* IPMI_PAYLOAD_H_ */
