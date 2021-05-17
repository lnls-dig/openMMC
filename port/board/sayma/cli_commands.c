/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
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

/* C Standard includes */
#include <stdlib.h>
#include <ctype.h>

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"

/* Project Includes */
#include "cli_commands.h"
#include "port.h"
#include "ipmi.h"
#include "task_priorities.h"
#include "payload.h"
#include "hotswap.h"
#include "utils.h"
#include "fru.h"
#include "led.h"
#include "GitSHA1.h"
#include "i2c_mapping.h"


static BaseType_t EnablePowerSupplyCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    uint8_t state;
    BaseType_t param_1_length;

    // Get state
    state = atoi(FreeRTOS_CLIGetParameter((const char *) pcCommandString, 1, &param_1_length));

    // Power supply is enabled only when P12V0 is valid
    if (state) {
        payload_send_message(FRU_AMC, PAYLOAD_MESSAGE_PPGOOD);
    } else {
        payload_send_message(FRU_AMC, PAYLOAD_MESSAGE_PPGOODn);
    }

    if (state) {
        if (gpio_read_pin(PIN_PORT(GPIO_P12V0_OK), PIN_NUMBER(GPIO_P12V0_OK))) {
            strcpy(pcWriteBuffer, "OK");
        } else {
            strcpy(pcWriteBuffer, "FAIL");
        }
    } else {
        strcpy(pcWriteBuffer, "OK");
    }

    return pdFALSE;
}

static BaseType_t GpioReadCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    char *name;
    uint8_t value;
    BaseType_t param_length;

    // Get GPIO name
    name = (char *) FreeRTOS_CLIGetParameter((const char *) pcCommandString, 1, &param_length);

    // Convert GPIO name to lower-case
    for (int i = 0; name[i]; i++) {
        name[i] = tolower((int) name[i]);
    }

    // Power supply
    if (strcmp(name, "p12v0_ok") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_P12V0_OK), PIN_NUMBER(GPIO_P12V0_OK));
    }
    else if (strcmp(name, "pgood_p5v0") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_PGOOD_P5V0), PIN_NUMBER(GPIO_PGOOD_P5V0));
    }
    else if (strcmp(name, "sdram_pgood") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_SDRAM_PGOOD), PIN_NUMBER(GPIO_SDRAM_PGOOD));
    }
    else if (strcmp(name, "p3v3_fmc_pgood") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_P3V3_FMC_PGOOD), PIN_NUMBER(GPIO_P3V3_FMC_PGOOD));
    }
    else if (strcmp(name, "ltm_pgood") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_LTM_PGOOD), PIN_NUMBER(GPIO_LTM_PGOOD));
    }
    else if (strcmp(name, "p0v95_pg") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_P0V95_PG), PIN_NUMBER(GPIO_P0V95_PG));
    }
    else if (strcmp(name, "p1v2_pg") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_P1V2_PG), PIN_NUMBER(GPIO_P1V2_PG));
    }
    else if (strcmp(name, "p0v95i_pg") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_P0V95I_PG), PIN_NUMBER(GPIO_P0V95I_PG));
    }

    // Sensors
    else if (strcmp(name, "overtemp_n") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_OVERTEMPn), PIN_NUMBER(GPIO_OVERTEMPn));
    }

    // HW ID
    else if (strcmp(name, "hw_id") == 0) {
        value = (gpio_read_pin(PIN_PORT(GPIO_HW_ID3), PIN_NUMBER(GPIO_HW_ID3)) & 0x1) << 0x3 |
                (gpio_read_pin(PIN_PORT(GPIO_HW_ID2), PIN_NUMBER(GPIO_HW_ID2)) & 0x1) << 0x2 |
                (gpio_read_pin(PIN_PORT(GPIO_HW_ID1), PIN_NUMBER(GPIO_HW_ID1)) & 0x1) << 0x1 |
                (gpio_read_pin(PIN_PORT(GPIO_HW_ID0), PIN_NUMBER(GPIO_HW_ID0)) & 0x1);
    }

    // FPGA
    else if (strcmp(name, "fpga_init_b") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_FPGA_INITB), PIN_NUMBER(GPIO_FPGA_INITB));
    }
    else if (strcmp(name, "fpga_done_b") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_FPGA_DONE_B), PIN_NUMBER(GPIO_FPGA_DONE_B));
    }

    // Front panel button
    else if (strcmp(name, "front_button") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_FRONT_BUTTON), PIN_NUMBER(GPIO_FRONT_BUTTON));
    }

    // Hot swap handle
    else if (strcmp(name, "hot_swap_handle") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_HOT_SWAP_HANDLE), PIN_NUMBER(GPIO_HOT_SWAP_HANDLE));
    }

    // AMC
    else if (strcmp(name, "amc_ga") == 0) {
        value = (gpio_read_pin(PIN_PORT(GPIO_GA2), PIN_NUMBER(GPIO_GA2)) & 0x1) << 0x2 |
                (gpio_read_pin(PIN_PORT(GPIO_GA1), PIN_NUMBER(GPIO_GA1)) & 0x1) << 0x1 |
                (gpio_read_pin(PIN_PORT(GPIO_GA0), PIN_NUMBER(GPIO_GA0)) & 0x1);
    }
    else if (strcmp(name, "amc_en") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_MMC_ENABLE), PIN_NUMBER(GPIO_MMC_ENABLE));
    }

    // RTM
    else if (strcmp(name, "rtm_ps") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_RTM_PS), PIN_NUMBER(GPIO_RTM_PS));
    }

    // FMC
    else if (strcmp(name, "fmc_prsnt") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_FMC1_PRSNT_M2C), PIN_NUMBER(GPIO_FMC1_PRSNT_M2C));
    }
    else if (strcmp(name, "fmc_pg_m2c") == 0) {
        value = gpio_read_pin(PIN_PORT(GPIO_FMC1_PG_M2C), PIN_NUMBER(GPIO_FMC1_PG_M2C));
    }

    else {
        snprintf((char *) pcWriteBuffer, sizeof(pcWriteBuffer), "GPIO name <%s> not recognized.", name);
        return pdFALSE;
    }

    // Write gpio value to the output buffer
    itoa(value, (char *) pcWriteBuffer, 10);

    return pdFALSE;
}

static BaseType_t GpioWriteCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    char *name;
    uint8_t value;
    BaseType_t param_1_length, param_2_length;

    // Get GPIO value
    value = atoi(FreeRTOS_CLIGetParameter((const char *) pcCommandString, 2, &param_2_length));

    // Get GPIO name
    name = strtok((char *) FreeRTOS_CLIGetParameter((const char *) pcCommandString, 1, &param_1_length), " ");

    // Convert GPIO name to lower-case
    for (int i = 0; name[i]; i++) {
        name[i] = tolower((int) name[i]);
    }

    // I2C MUX
    if (strcmp(name, "i2c_mux_addr") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR2), PIN_NUMBER(GPIO_I2C_MUX_ADDR2), (value >> 0x1) & 0x1);
        gpio_set_pin_state(PIN_PORT(GPIO_I2C_MUX_ADDR1), PIN_NUMBER(GPIO_I2C_MUX_ADDR1), value & 0x1);
    }
    else if (strcmp(name, "sw_reset_n") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_SW_RESETn), PIN_NUMBER(GPIO_SW_RESETn), value & 0x1);
    }

    // LEDs
    else if (strcmp(name, "led_blue") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_LEDBLUE), PIN_NUMBER(GPIO_LEDBLUE), value & 0x1);
    }
    else if (strcmp(name, "led_green") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_LEDGREEN), PIN_NUMBER(GPIO_LEDGREEN), value & 0x1);
    }
    else if (strcmp(name, "led_red") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_LEDRED), PIN_NUMBER(GPIO_LEDRED), value & 0x1);
    }

    // JTAG
    else if (strcmp(name, "rtm_jtag_override") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_RTM_JTAG_Override), PIN_NUMBER(GPIO_RTM_JTAG_Override), value & 0x1);
    }
    else if (strcmp(name, "fmc_jtag_override") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_FMC_JTAG_Override), PIN_NUMBER(GPIO_FMC_JTAG_Override), value & 0x1);
    }

    // FPGA
    else if (strcmp(name, "fpga_program_b") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_FPGA_PROGRAM_B), PIN_NUMBER(GPIO_FPGA_PROGRAM_B), value & 0x1);
    }
    else if (strcmp(name, "fpga_reset_n") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_FPGA_RESETn), PIN_NUMBER(GPIO_FPGA_RESETn), value & 0x1);
    }
    else if (strcmp(name, "boot_mode") == 0) {
        gpio_set_pin_state(PIN_PORT(BOOT_MODE3), PIN_NUMBER(BOOT_MODE3), (value >> 0x3) & 0x1);
        gpio_set_pin_state(PIN_PORT(BOOT_MODE2), PIN_NUMBER(BOOT_MODE2), (value >> 0x2) & 0x1);
        gpio_set_pin_state(PIN_PORT(BOOT_MODE1), PIN_NUMBER(BOOT_MODE1), (value >> 0x1) & 0x1);
        gpio_set_pin_state(PIN_PORT(BOOT_MODE0), PIN_NUMBER(BOOT_MODE0), value & 0x1);
    }
    else if (strcmp(name, "fpga_status") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_FPGA_STATUS), PIN_NUMBER(GPIO_FPGA_STATUS), value & 0x1);
    }

    // FLASH
    else if (strcmp(name, "en_flash_update") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_EN_FLASH_UPDATE), PIN_NUMBER(GPIO_EN_FLASH_UPDATE), value & 0x1);
    }

    // FMC
    else if (strcmp(name, "fmc_en_p12v0") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_EN_FMC1_P12V), PIN_NUMBER(GPIO_EN_FMC1_P12V), value & 0x1);
    }
    else if (strcmp(name, "fmc_en_p3v3") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_EN_P3V3_FMC), PIN_NUMBER(GPIO_EN_P3V3_FMC), value & 0x1);
    }
    else if (strcmp(name, "fmc_pg_c2m") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_FMC1_PG_C2M), PIN_NUMBER(GPIO_FMC1_PG_C2M), value & 0x1);
    }
    else if (strcmp(name, "fmc_dir") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_FMC1_DIR), PIN_NUMBER(GPIO_FMC1_DIR), value & 0x1);
    }

    // RTM
    else if (strcmp(name, "rtm_en_p12v0") == 0) {
        gpio_set_pin_state(PIN_PORT(GPIO_EN_RTM_PWR), PIN_NUMBER(GPIO_EN_RTM_PWR), value & 0x1);
    }

    else {
        snprintf((char *) pcWriteBuffer, sizeof(pcWriteBuffer), "GPIO name <%s> not recognized.", name);
        return pdFALSE;
    }

    strcpy(pcWriteBuffer, "OK");

    return pdFALSE;
}

static BaseType_t ReadMagicValueCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    strcpy(pcWriteBuffer, "4D41474943");
    return pdFALSE;
}

static BaseType_t ReadVersionCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    strcpy(pcWriteBuffer, g_GIT_TAG);
    return pdFALSE;
}

static BaseType_t ResetCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    NVIC_SystemReset();
    return pdFALSE;
}

static const CLI_Command_Definition_t EnablePowerSupplyCommandDefinition = {
        "enable_power",
        "\r\nenable_power <state>:\r\n Enables DCDC converters\r\n",
        EnablePowerSupplyCommand,
        1
};

static const CLI_Command_Definition_t GpioReadCommandDefinition = {
        "gpio_read",
        "\r\ngpio_read <gpio>:\r\n Reads <gpio> value\r\n",
        GpioReadCommand,
        1
};

static const CLI_Command_Definition_t GpioWriteCommandDefinition = {
        "gpio_write",
        "\r\ngpio_write <gpio> <value>:\r\n Writes <value> to the selected <gpio>\r\n",
        GpioWriteCommand,
        2
};

static const CLI_Command_Definition_t ReadMagicValueCommandDefinition = {
        "magic",
        "\r\nmagic:\r\n Reads magic value\r\n",
        ReadMagicValueCommand,
        0
};

static const CLI_Command_Definition_t ReadVersionCommandDefinition = {
        "version",
        "\r\nversion:\r\n Returns firmware version (GIT SHA)\r\n",
        ReadVersionCommand,
        0
};

static const CLI_Command_Definition_t ResetCommandDefinition = {
        "reset",
        "\r\nreset:\r\n Resets MMC\r\n",
        ResetCommand,
        0
};

/**
 * @brief Registers all the defined CLI commands.
 */
void RegisterCLICommands(void)
{
    // Power Supply
    // FreeRTOS_CLIRegisterCommand(&EnablePowerSupplyCommandDefinition);

    // GPIO
    FreeRTOS_CLIRegisterCommand(&GpioReadCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&GpioWriteCommandDefinition);

    FreeRTOS_CLIRegisterCommand(&ReadMagicValueCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&ReadVersionCommandDefinition);
    FreeRTOS_CLIRegisterCommand(&ResetCommandDefinition);

}
