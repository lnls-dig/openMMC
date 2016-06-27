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
 * @defgroup HOTSWAP Hotswap Sensor
 * @ingroup SENSORS
 */

/**
 * @file hotswap.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Hotswap sensors definitions
 *
 * @ingroup HOTSWAP
 */

#ifndef _HOTSWAP_H_
#define _HOTSWAP_H_

#include "sdr.h"
#ifdef MODULE_RTM
#include "rtm_user.h"
#endif

#define DEBOUNCE_TIME 250

/* Module handle sensor status */
#define HOTSWAP_STATE_HANDLE_CLOSED             0x00
#define HOTSWAP_STATE_HANDLE_OPENED             0x01
#define HOTSWAP_STATE_QUIESCED                  0x02
#define HOTSWAP_STATE_BP_FAIL                   0x03
#define HOTSWAP_STATE_BP_SDOWN                  0x04
#define HOTSWAP_STATE_URTM_PRSENT               0x05
#define HOTSWAP_STATE_URTM_ABSENT               0x06
#define HOTSWAP_STATE_URTM_COMPATIBLE           0x07
#define HOTSWAP_STATE_URTM_INCOMPATIBLE         0x08

#define HOTSWAP_MODULE_HANDLE_CLOSED_MASK       0x01
#define HOTSWAP_MODULE_HANDLE_OPEN_MASK         0x02
#define HOTSWAP_QUIESCED_MASK                   0x04
#define HOTSWAP_BACKEND_PWR_FAILURE_MASK        0x08
#define HOTSWAP_BACKEND_PWR_SHUTDOWN_MASK       0x10
#define HOTSWAP_URTM_PRESENT_MASK               0x20
#define HOTSWAP_URTM_ABSENT_MASK                0x40
#define HOTSWAP_URTM_COMPATIBLE_MASK            0x80

enum {
    HOTSWAP_AMC,
    HOTSWAP_RTM
};

/**
 * @brief Hotswap Task handle
 */
TaskHandle_t vTaskHotSwap_Handle;

/**
 * @brief Periodically reads the Hotswap sensor state and checks if its state has changed
 *
 * @param Parameters Pointer to parameter list passed to task upon initialization (not used here)
 */
void vTaskHotSwap( void *Parameters );

/**
 * @brief Initializes hotswap sensor informatiosn on SDR and creates #vTaskHotSwap
 *
 * @return None
 */
void hotswap_init( void );

/**
 * @brief Send a hotswap event via IPMB
 *
 * @param sensor Pointer to current Hotswap sensor structure
 * @param evt Event to be sent
 *
 * @return IPMB layer error
 * @see ipmb.c
 */
ipmb_error hotswap_send_event( sensor_t *sensor, uint8_t evt );

/**
 * @brief Sets the corresponding status bit in the Hotswap SDR readout value
 *
 * @param fru FRU ID that identifies the owner of the sensor
 * @param mask Bit position to set
 *
 * @return None
 */
void hotswap_set_mask_bit( uint8_t fru, uint8_t mask );

/**
 * @brief Clears the corresponding status bit in the Hotswap SDR readout value
 *
 * @param fru FRU ID that identifies the owner of the sensor
 * @param mask Bit position to clear
 *
 * @return None
 */
void hotswap_clear_mask_bit( uint8_t fru, uint8_t mask );

#endif
