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
#include "pcf8574.h"
#include "pin_mapping.h"
#include "hotswap.h"
#include "i2c_mapping.h"
#include "fru.h"
#include "utils.h"
#include "led.h"
#include "uart_debug.h"
#include "payload.h"
#include "board_led.h"

extern uint8_t rtm_pm_enabled;
bool rtm_initialized = false;

/* RTM Management functions */
void rtm_enable_payload_power(void)
{
    if (!gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS))) {

        // Enable RTM P12V0
        gpio_set_pin_state(PIN_PORT(GPIO_EN_RTM_PWR), PIN_NUMBER(GPIO_EN_RTM_PWR), 1);

        // Delay to avoid tripping of current inrush protection
        vTaskDelay(20);

        // Enable RTM power
        pcf8574_set_port_low(1 << RTM_GPIO_EN_DC_DC);

        // Verify PG
        uint8_t power_good_n = 0;

        vTaskDelay(50);
        power_good_n = pcf8574_read_pin(RTM_GPIO_POWER_GOOD);

        if (power_good_n) {
            printf("[RTM] POWER GOOD error.\n");
            payload_send_message(FRU_RTM, PAYLOAD_MESSAGE_QUIESCE);
        }

        gpio_set_pin_state(PIN_PORT(GPIO_FPGA_STATUS), PIN_NUMBER(GPIO_FPGA_STATUS), 0x1);

        pcf8574_set_port_high(1 << RTM_GPIO_LED_BLUE);
        pcf8574_set_port_low(1 << RTM_GPIO_LED_RED);
        pcf8574_set_port_low(1 << RTM_GPIO_LED_GREEN);
    }
}

void rtm_disable_payload_power(void)
{
    if (!gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS))) {

        // Disable RTM power
        pcf8574_set_port_high(1 << RTM_GPIO_EN_DC_DC);

        // Disable RTM P12V0
        gpio_set_pin_state(PIN_PORT(GPIO_EN_RTM_PWR), PIN_NUMBER(GPIO_EN_RTM_PWR), 0);

        pcf8574_set_port_low(1 << RTM_GPIO_LED_BLUE);
        pcf8574_set_port_high(1 << RTM_GPIO_LED_RED);
        pcf8574_set_port_high(1 << RTM_GPIO_LED_GREEN);
    }
}

uint8_t rtm_get_hotswap_handle_status(uint8_t *state)
{
    static uint8_t falling, rising;
    uint8_t result;

    // result = !pcf8574_read_pin(RTM_GPIO_HOTSWAP_HANDLE);
    result = gpio_read_pin(PIN_PORT(GPIO_HOT_SWAP_HANDLE), PIN_NUMBER(GPIO_HOT_SWAP_HANDLE)); // RTM HS is disabled

    falling = (falling << 1) | !result | 0x80;
    rising = (rising << 1) | result | 0x80;

    if ((falling == 0xFF) || (rising == 0xFF)) {
        *state = result;
        return true;
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
    gpio_set_pin_state(PIN_PORT(GPIO_EN_RTM_MP), PIN_NUMBER(GPIO_EN_RTM_MP), true);

    vTaskDelay(100);

    pcf8574_set_port_dir_input(1 << RTM_GPIO_HOTSWAP_HANDLE);
    pcf8574_set_port_dir_input(1 << RTM_GPIO_POWER_GOOD);

    pcf8574_set_port_low(1 << RTM_GPIO_LED_BLUE);
    pcf8574_set_port_high(1 << RTM_GPIO_LED_RED);
    pcf8574_set_port_high(1 << RTM_GPIO_LED_GREEN);

    rtm_initialized = true;
}

void rtm_hardware_close(void)
{
    // Disable RTM P3V3_MP
    gpio_set_pin_state(PIN_PORT(GPIO_EN_RTM_MP), PIN_NUMBER(GPIO_EN_RTM_MP), false);

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
    gpio_set_pin_state(PIN_PORT(GPIO_FPGA_STATUS), PIN_NUMBER(GPIO_FPGA_STATUS), 0x0);
    return true;
}

void rtm_ctrl_led(uint8_t id, uint8_t state)
{
    uint8_t pcf_pin;

    switch (id) {
    case LED_BLUE:
        pcf_pin = RTM_GPIO_LED_BLUE;
        break;
    case LED1:
        pcf_pin = RTM_GPIO_LED_RED;
        break;
    case LED2:
        pcf_pin = RTM_GPIO_LED_GREEN;
        break;
    default:
        return;
    }

    if (!gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS)) && rtm_initialized) {
        if (state == 1) {
            pcf8574_set_port_high(1 << pcf_pin);
        }
        else if (state == 0) {
            pcf8574_set_port_low(1 << pcf_pin);
        }
    }
}

uint8_t rtm_read_led(uint8_t id)
{
    uint8_t pcf_pin;

    switch (id) {
    case LED_BLUE:
        pcf_pin = RTM_GPIO_LED_BLUE;
        break;
    case LED1:
        pcf_pin = RTM_GPIO_LED_RED;
        break;
    case LED2:
        pcf_pin = RTM_GPIO_LED_GREEN;
        break;
    default:
        return 1;
    }

    if (!gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS)) && rtm_initialized) {
        return pcf8574_read_pin(pcf_pin);
    }

    return 0;
}
