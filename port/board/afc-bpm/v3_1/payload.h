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

/**
 * @file afc/v3_1/payload.h
 * @brief Payload control module definitions for AFCv3.1
 *
 * @ingroup AFC_V3_1_PAYLOAD
 */

/**
 * @defgroup AFC_V3_1_PAYLOAD AFCv3.1 Payload Control
 * @ingroup AFC_V3_1
 * @{
 */

#ifndef PAYLOAD_H_
#define PAYLOAD_H_

#include "event_groups.h"

/**
 * @brief Payload state machine state numbers
 */
extern enum {
    PAYLOAD_NO_POWER = 0,
    PAYLOAD_POWER_GOOD_WAIT,
    PAYLOAD_STATE_FPGA_SETUP,
    PAYLOAD_FPGA_ON,
    PAYLOAD_SWITCHING_OFF,
    PAYLOAD_QUIESCED,
    PAYLOAD_MAX_STATES
} payload_state;

/**
 * @defgroup AFC_V3_1_PAYLOAD_MSG Payload Messages Codes
 * @ingroup AFC_V3_1_PAYLOAD
 * @{
 */
#define PAYLOAD_MESSAGE_COLD_RST        (1 << 0)
#define PAYLOAD_MESSAGE_WARM_RST        (1 << 1)
#define PAYLOAD_MESSAGE_REBOOT          (1 << 2)
#define PAYLOAD_MESSAGE_QUIESCE         (1 << 3)
#define PAYLOAD_MESSAGE_RTM_READY       (1 << 9)
/**
 * @}
 */

/**
 * @brief Payload task unblock delay
 */
#define PAYLOAD_BASE_DELAY 100

/**
 * @brief Payload task handle variable
 */
extern TaskHandle_t vTaskPayload_Handle;

/**
 * @brief Sends a message to the payload task
 *
 * This function basically sets a flag that the Payload task reads and advances (or not) on the state machine
 *
 * @param fru_id Target FRU ID (0:AMC 1:RTM)
 * @param msg Message to send, using @ref AFC_V3_1_PAYLOAD_MSG definitions
 */
void payload_send_message( uint8_t fru_id, EventBits_t msg );

/**
 * @brief Payload Control task
 *
 * @param pvParameters Pointer to buffer holding parameters passed to task upon initialization
 */
void vTaskPayload( void *pvParameters );

/**
 * @brief Creates Payload Control task and initializes the board's needed hardware
 */
void payload_init( void );

#ifdef MODULE_HPM

#define PAYLOAD_HPM_PAGE_SIZE    256

uint8_t payload_hpm_prepare_comp( void );
uint8_t payload_hpm_upload_block( uint8_t * block, uint16_t size );
uint8_t payload_hpm_finish_upload( uint32_t image_size );
uint8_t payload_hpm_get_upgrade_status( void );
uint8_t payload_hpm_activate_firmware( void );
#endif

#endif /* IPMI_PAYLOAD_H_ */

/**
 * @}
 */
