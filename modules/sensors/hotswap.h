/*
 *   hotswap.c
 *
 *   AFCIPMI  --
 *
 *   Copyright (C) 2015
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

#define DEBOUNCE_TIME 250


/* Module handle sensor status */
#define HOT_SWAP_STATE_HANDLE_CLOSED            0x00
#define HOT_SWAP_STATE_HANDLE_OPENED            0x01
#define HOT_SWAP_STATE_QUIESCED                 0x02
#define HOT_SWAP_STATE_BP_FAIL                  0x03
#define HOT_SWAP_STATE_BP_SDOWN                 0x04
#define HOT_SWAP_STATE_URTM_PRSENT              0x05
#define HOT_SWAP_STATE_URTM_ABSENT              0x06
#define HOT_SWAP_STATE_URTM_COMPATIBLE          0x07
#define HOT_SWAP_STATE_URTM_INCOMPATIBLE        0x08

#define HOTSWAP_MODULE_HANDLE_CLOSED_MASK       0x01
#define HOTSWAP_MODULE_HANDLE_OPEN_MASK         0x02
#define HOTSWAP_QUIESCED_MASK                   0x04
#define HOTSWAP_BACKEND_PWR_FAILURE_MASK        0x08
#define HOTSWAP_BACKEND_PWR_SHUTDOWN_MASK       0x10

TaskHandle_t vTaskHotSwap_Handle;

extern TickType_t getTickDifference(TickType_t current_time, TickType_t start_time);
void vTaskHotSwap( void *Parameters );
void hotswap_init( void );
