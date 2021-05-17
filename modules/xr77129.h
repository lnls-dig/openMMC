/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2021  Krzysztof Macias <krzysztof.macias@creotech.pl>
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

#ifndef MODULES_XR77129_H_
#define MODULES_XR77129_H_

#define XR77129_UPDATE_RATE     200

#define XR77129_HW_ID           4868
#define XR7724_HW_ID            4354
#define XR7724_HW_ID_1          4611

/* Value indicating that all supplies are in regulation ( PWR_GET_STATUS register ) */
#define XR77129_POWER_OK                0x1f00

/* SYSTEM COMMANDS */
#define XR77129_GET_HOST_STS            0x02
#define XR77129_SET_HOST_INT_MASK       0x03
#define XR77129_CLEAR_HOST_INT          0x04
#define XR77129_GET_FAULT_STS           0x05
#define XR77129_CLEAR_FAULT_STS         0x06
#define XR77129_PWR_GET_STATUS          0x09
#define XR77129_CHIP_READY              0x0E
#define XR77129_PWR_RESTART             0x0F
/* POWER COMMANDS */
#define XR77129_PWR_READ_VOLTAGE_CH1    0x10
#define XR77129_PWR_READ_VOLTAGE_CH2    0x11
#define XR77129_PWR_READ_VOLTAGE_CH3    0x12
#define XR77129_PWR_READ_VOLTAGE_CH4    0x13
#define XR77129_PWR_READ_VOLTAGE_VIN    0x14
#define XR77129_PWR_READ_TEMP_VTJ       0x15
#define XR77129_PWR_READ_CURRENT_CH1    0x16
#define XR77129_PWR_READ_CURRENT_CH2    0x17
#define XR77129_PWR_READ_CURRENT_CH3    0x18
#define XR77129_PWR_READ_CURRENT_CH4    0x19
/* IO COMMANDS */
#define XR77129_GPIO_READ_GPIO          0x30
#define XR77129_GPIO_SET_GPIO           0x31
#define XR77129_GPIO_POL_GPIO           0x32

/* FLASH COMMANDS */
#define XR77129_FLASH_PROGRAM_ADDRESS           0x40
#define XR77129_FLASH_PROGRAM_DATA              0x41
#define XR77129_FLASH_PROGRAM_DATA_INC_ADDRESS  0x42
#define XR77129_FLASH_INIT                      0x4D
#define XR77129_FLASH_PAGE_CLEAR                0x4E
#define XR77129_FLASH_PAGE_ERASE                0x4F

/* GPIO */
#define XR77129_GPIO0           1 << 0
#define XR77129_GPIO1           1 << 1
#define XR77129_PSIO0           1 << 2
#define XR77129_PSIO1           1 << 3
#define XR77129_PSIO2           1 << 4

enum XR77129_GET_HOST_STS_REG
{
    XR77129_GPIO_EVENT                  = 1 << 0,
    XR77129_SUPPLY_FAULT_EVENT          = 1 << 1,
    XR77129_TEMP_OVER_EVENT             = 1 << 2,
    XR77129_TEMP_UNDER_EVENT            = 1 << 3,
    XR77129_TEMP_WARNING_EVENT          = 1 << 4,
    XR77129_UVLO_FAULT_ACTIVE_EVENT     = 1 << 5,
    XR77129_UVLO_FAULT_INACTIVE_EVENT   = 1 << 6,
    XR77129_UVLO_WARNING_EVENT          = 1 << 7,
    XR77129_FLASH_CLEAR_DONE_EVENT      = 1 << 8,
    XR77129_FLASH_ERASE_DONE_EVENT      = 1 << 9,
    XR77129_V5EXT_RISE_EVENT            = 1 << 11,
    XR77129_V5EXT_FALL_EVENT            = 1 << 12,
    XR77129_LDO5OVC_EVENT               = 1 << 13
};

enum XR77129_STATUS_REGISTERS {
	HOST_STS_REG,
	FAULT_STS_REG,
	PWR_STATUS_REG,
	PWR_CHIP_READY,
	GPIO_STATE,
	XR77129_STATUS_REGISTERS_COUNT
};

typedef struct {
    uint16_t status_regs[XR77129_STATUS_REGISTERS_COUNT];
    uint8_t status_regs_addr[XR77129_STATUS_REGISTERS_COUNT];
    uint8_t chipid;
    uint8_t gpio_default; // 0 - GPIO0; 1 - GPIO1; 2 - PSIO0; 3 - PSIO1; 4 - PSIO2
    uint8_t gpio_mask;
} xr77129_data_t;

typedef struct {
	uint16_t address;
	uint8_t data;
} runtime_data_t;

uint8_t xr77129_flash_verify(xr77129_data_t * data, const uint8_t * flash_data, uint32_t size);
uint8_t xr77129_flash_read(xr77129_data_t * data, uint16_t address, uint32_t size);
uint8_t xr77129_flash_program(xr77129_data_t * data, const uint8_t * flash_data, uint32_t size);
uint8_t xr77129_flash_erase(xr77129_data_t * data);

uint8_t xr77129_check_flash(xr77129_data_t * data, const uint8_t * xr77129_config, uint32_t size);
uint8_t xr77129_flash_load(xr77129_data_t * data, const uint8_t * xr77129_config, uint32_t size);

uint8_t xr77129_reset(xr77129_data_t * data);
uint8_t xr77129_set_ready(xr77129_data_t * data, uint16_t status);
uint8_t xr77129_read_status(xr77129_data_t * data);

uint8_t xr77129_read_value(xr77129_data_t * data, uint8_t reg_address, uint16_t *read);
uint8_t xr77129_write_value(xr77129_data_t * data, uint8_t reg_address, uint16_t value);
uint8_t xr77129_runtime_load(xr77129_data_t * exar, runtime_data_t * runtime_data, uint32_t len);

#endif /* MODULES_XR77129_H_ */
