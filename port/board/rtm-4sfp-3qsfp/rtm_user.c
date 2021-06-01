/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015-2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2021  Wojciech Ruclo <wojciech.ruclo@creotech.pl>
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

/* Project includes */
#include "port.h"
#include "rtm.h"
#include "pca9554.h"
#include "pin_mapping.h"
#include "hotswap.h"
#include "i2c_mapping.h"
#include "fru.h"
#include "utils.h"
#include "led.h"
#include "uart_debug.h"
#include "rtm_user.h"
#include "board_led.h"
#include "payload.h"
#include "tca9539.h"

bool rtm_initialized = false;

/* RTM Management functions */
void rtm_enable_payload_power(void)
{
    // Enable RTM P12V0
    tca9539_output_pin_set(CHIP_ID_TCA9539_PM, 1, 6, true);
    vTaskDelay(100);

    // Enable RTM power
    pca9554_write_pin(CHIP_ID_RTM_PCA9554, RTM_GPIO_DCDC_ENABLEn, 0);

    // Verify POWER GOOD
    uint8_t power_good_n = 0x1;

    vTaskDelay(50);
    pca9554_read_pin(CHIP_ID_RTM_PCA9554, RTM_GPIO_POWER_GOOD, &power_good_n);

    if (power_good_n) {
        printf("[RTM] POWER GOOD error.\n");
        payload_send_message(FRU_RTM, PAYLOAD_MESSAGE_QUIESCE);
    }
}

void rtm_disable_payload_power(void)
{
    // Disable RTM power
    pca9554_write_pin(CHIP_ID_RTM_PCA9554, RTM_GPIO_DCDC_ENABLEn, 1);

    // Disable RTM P12V0
    tca9539_output_pin_set(CHIP_ID_TCA9539_PM, 1, 6, false);
}

uint8_t rtm_get_hotswap_handle_status(uint8_t *state)
{
    static uint8_t falling, rising;
    uint8_t pin_read;

    if (rtm_initialized) {
        if (pca9554_read_pin(CHIP_ID_RTM_PCA9554, RTM_GPIO_HOTSWAP_HANDLE, &pin_read) == 0) {
            return false;
        }

        falling = (falling << 1) | !pin_read | 0x80;
        rising = (rising << 1) | pin_read | 0x80;

        if ((falling == 0xFF) || (rising == 0xFF)) {
            *state = pin_read;
            return true;
        }
    }
    return false;
}

void rtm_check_presence(uint8_t *status)
{
    if (gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS))) {
        *status = HOTSWAP_STATE_URTM_ABSENT;
    }
    else {
        *status = HOTSWAP_STATE_URTM_PRSENT;
    }
}

void rtm_hardware_init(void)
{
    // Enable RTM P3V3_MP
    tca9539_output_pin_set(CHIP_ID_TCA9539_PM, 1, 1, true);
    vTaskDelay(200);

    // Enable RTM I2C
    gpio_set_pin_state(PIN_PORT(GPIO_RTM_I2C_EN), PIN_NUMBER(GPIO_RTM_I2C_EN), true);

    // Set GPIO expander direction
    pca9554_set_port_dir(CHIP_ID_RTM_PCA9554, 0x11);

    rtm_initialized = true;
}

void rtm_hardware_close(void)
{
    // Disable RTM P3V3_MP
    tca9539_output_pin_set(CHIP_ID_TCA9539_PM, 1, 1, false);

    rtm_initialized = false;
}

bool rtm_compatibility_check(void)
{
    uint8_t i;
    size_t rec_sz[2];
    uint8_t *z3_compat_recs[2] = { NULL, NULL };
    uint8_t cmn_hdr[8], multirec_hdr[10];
    uint8_t multirec_off;
    bool z3rec_found;

    for (i = 0; i < 2; i++) {
        /* Read FRU Common Header */
        fru_read(i, cmn_hdr, 0, 8);
        /* The offsets are divided by 8 in the common header */
        multirec_off = cmn_hdr[5] * 8;

        do {
            /* Read Multirecord header */
            fru_read(i, multirec_hdr, multirec_off, 10);

            if (multirec_hdr[8] == 0x30) {
                z3rec_found = true;
                break;
            }
            /* Advance the offset pointer, adding the record length field to it */
            multirec_off += multirec_hdr[2] + 5;

        } while ((multirec_hdr[1] >> 7) != 1);

        if (z3rec_found) {
            /* Read the Zone3 Compatibility Record, including the Multirecord header */
            rec_sz[i] = multirec_hdr[2] + 5;
            z3_compat_recs[i] = pvPortMalloc(rec_sz[i]);
            fru_read(i, z3_compat_recs[i], multirec_off, rec_sz[i]);
        }
    }

    if (!cmpBuffs(z3_compat_recs[0], rec_sz[0], z3_compat_recs[1], rec_sz[1])) {
        return true;
    }

    vPortFree(z3_compat_recs[0]);
    vPortFree(z3_compat_recs[1]);

    return false;
}

bool rtm_quiesce(void)
{
    // Disable RTM power
    return pca9554_write_pin(CHIP_ID_RTM_PCA9554, RTM_GPIO_DCDC_ENABLEn, 1);
}

void rtm_ctrl_led(uint8_t id, uint8_t state)
{
    uint8_t pca_pin;

    switch (id) {
    case LED_BLUE:
        pca_pin = RTM_GPIO_LED_BLUE;
        break;

    case LED1:
        pca_pin = RTM_GPIO_LED_RED;
        break;

    case LED2:
        pca_pin = RTM_GPIO_LED_GREEN;
        break;

    default:
        return;
    }

    if (rtm_initialized)
        pca9554_write_pin(CHIP_ID_RTM_PCA9554, pca_pin, state);
}

uint8_t rtm_read_led(uint8_t id)
{
    uint8_t pca_pin, stat;

    switch (id) {
    case LED_BLUE:
        pca_pin = RTM_GPIO_LED_BLUE;
        break;

    case LED1:
        pca_pin = RTM_GPIO_LED_RED;
        break;

    case LED2:
        pca_pin = RTM_GPIO_LED_GREEN;
        break;

    default:
        return 1;
    }

    if (rtm_initialized)
        pca9554_read_pin(CHIP_ID_RTM_PCA9554, pca_pin, &stat);

    return stat;
}
