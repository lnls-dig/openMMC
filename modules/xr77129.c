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

/**
 * @file xr77129.c
 *
 *
 * @brief XR77129 / XRP7724 interface implementation
 *
 * @ingroup XR7129 / XRP7724
 */

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "stopwatch.h"

/* Project Includes */
#include "port.h"
#include "sdr.h"
#include "task_priorities.h"
#include "i2c.h"
#include "i2c_mapping.h"
#include "payload.h"
#include "xr77129.h"
#include "fpga_spi.h"
#include "fru.h"
#include "uart_debug.h"

static void xr77129_runtime_read(I2C_ID_T i2c_interf, uint8_t i2c_addr, uint16_t address, uint8_t * data)
{
    uint8_t lb = (address & 0xFF);
    uint8_t hb = (address & 0xFF00) >> 8;

    uint8_t payload[2] = {
        hb,
        lb
    };

    I2C_XFER_T xfer = {0};
    xfer.slaveAddr = i2c_addr;
    xfer.txBuff = payload;
    xfer.txSz = 2;
    xfer.rxBuff = data;
    xfer.rxSz = 1;
    while (Chip_I2C_MasterTransfer(i2c_interf, &xfer) == I2C_STATUS_ARBLOST) {}
//  return 1 - xfer.rxSz;
}

static void xr77129_runtime_write(I2C_ID_T i2c_interf, uint8_t i2c_addr, uint16_t address, uint8_t data)
{
    uint8_t lb = (address & 0xFF);
    uint8_t hb = (address & 0xFF00) >> 8;

    uint8_t payload[3] = {
        hb,
        lb,
        data
    };

    xI2CMasterWrite(i2c_interf, i2c_addr, payload, 3);
}

static uint8_t xr77129_flash_page_erase(xr77129_data_t * data, uint8_t page)
{
    if (page > 6)
        return 255;

    uint8_t rx[2] = { 0 };
    uint8_t tx[3] = { 0 };
    uint8_t i2c_interf, i2c_addr;

    if (i2c_take_by_chipid(data->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        // ENABLE FLASH
        uint16_t command = 0x0005;
        tx[0] = XR77129_FLASH_INIT;
        tx[2] = command & 0xFF;
        tx[1] = (command & 0xFF00) >> 8;
        xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

        vTaskDelay(500);

        // set FLASH address at 0x00
        command = (uint16_t) page;
        tx[0] = XR77129_FLASH_PAGE_ERASE;
        tx[2] = command & 0xFF;
        tx[1] = (command & 0xFF00) >> 8;
        xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

        vTaskDelay(1000);

        uint8_t page_erase_busy = 0x1;
        uint8_t page_erase_status = 0xFF;

        uint8_t page_erase_timeout = 0x20;
        uint32_t data_address = XR77129_FLASH_PAGE_ERASE;

        while (page_erase_busy) {
            xI2CMasterWriteRead(i2c_interf, i2c_addr, data_address, rx, 2);

            page_erase_busy = rx[1];
            page_erase_status = rx[0];

            vTaskDelay(500);

            if (!page_erase_timeout--) {
                printf("Page [%d] erase timeout\n", page);
                i2c_give(i2c_interf);
                return 0xFF;
            }
        }

        if (page_erase_status == 0xFF) {
            printf("Page [%d] erase error\n", page);
            i2c_give(i2c_interf);
            return 0xFF;
        }

        i2c_give(i2c_interf);
    }
    return 0;
}

static uint8_t xr77129_flash_page_clear(xr77129_data_t * data, uint8_t page)
{
    if (page > 6)
        return 255;

    uint8_t rx[2] = { 0 };
    uint8_t tx[3] = { 0 };
    uint8_t i2c_interf, i2c_addr;

    if (i2c_take_by_chipid(data->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        // ENABLE FLASH
        uint16_t command = 0x0001;
        tx[0] = XR77129_FLASH_INIT;
        tx[2] = command & 0xFF;
        tx[1] = (command & 0xFF00) >> 8;
        xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

        vTaskDelay(500);

        // set FLASH address at 0x00
        command = (uint16_t) page;
        tx[0] = XR77129_FLASH_PAGE_CLEAR;
        tx[2] = command & 0xFF;
        tx[1] = (command & 0xFF00) >> 8;
        xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

        vTaskDelay(1000);

        uint8_t page_clear_busy = 0x1;
        uint8_t page_clear_status = 0xFF;

        uint8_t page_clear_timeout = 0x20;
        uint32_t data_address = XR77129_FLASH_PAGE_CLEAR;

        while (page_clear_busy) {
            xI2CMasterWriteRead(i2c_interf, i2c_addr, data_address, rx, 2);

            page_clear_busy = rx[1];
            page_clear_status = rx[0];

            vTaskDelay(200);

            if (!page_clear_timeout--) {
                printf("Page [%d] clear timeout\n", page);
                i2c_give(i2c_interf);
                return 0xFF;
            }
        }

        if (page_clear_status == 0xFF) {
            printf("Page [%d] clear error\n", page);
            i2c_give(i2c_interf);
            return 0xFF;
        }

        i2c_give(i2c_interf);
    }
    return 0;
}

uint8_t xr77129_flash_verify(xr77129_data_t * data, const uint8_t * flash_data, uint32_t size)
{
    uint8_t rx[2] = { 0 };
    uint8_t tx[3] = { 0 };
    uint8_t i2c_interf, i2c_addr;

    printf("Checking %d EXAR configuration...\r\n", data->chipid);

    if (i2c_take_by_chipid(data->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        // ENABLE FLASH
        uint16_t command = 0x0001;
        tx[0] = XR77129_FLASH_INIT;
        tx[2] = command & 0xFF;
        tx[1] = (command & 0xFF00) >> 8;
        xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

        // 10msec delay
        vTaskDelay(200);

        // set FLASH address at 0x00
        command = 0x0000;
        tx[0] = XR77129_FLASH_PROGRAM_ADDRESS;
        tx[2] = command & 0xFF;
        tx[1] = (command & 0xFF00) >> 8;
        xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

        // 10msec delay
        vTaskDelay(200);

        uint8_t data_address = XR77129_FLASH_PROGRAM_DATA;
        for (uint32_t i = 0x00; i < size; i += 2) {
            tx[0] = XR77129_FLASH_PROGRAM_ADDRESS;
            tx[2] = i & 0xFF;
            tx[1] = (i & 0xFF00) >> 8;
            xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

            vTaskDelay(20);
            xI2CMasterWriteRead(i2c_interf, i2c_addr, data_address, rx, 2);

            if (rx[0] != flash_data[i] || rx[1] != flash_data[i + 1]) {
                printf("\nDiff for %d | [%d] %d != %d | [%d] %d != %d\r\n", data->chipid, (int) i, rx[0], flash_data[i],
                        (int) i + 1, rx[1], flash_data[i + 1]);
                i2c_give(i2c_interf);
                return false;
            }
        }

        i2c_give(i2c_interf);
    }

    return true;
}

uint8_t xr77129_flash_read(xr77129_data_t * data, uint16_t address, uint32_t size)
{
    uint8_t rx[2] = { 0 };
    uint8_t tx[3] = { 0 };
    uint8_t reg;
    uint8_t i2c_interf, i2c_addr;

    printf("Read %d at %d\n", (int) size, address);

    // ENABLE FLASH
    uint16_t command = 0x0001;
    reg = 0x4D;
    xr77129_write_value(data, reg, command);

    // 10msec delay
    vTaskDelay(100);

    // set FLASH address at 0x00
    command = address;
    xr77129_write_value(data, XR77129_FLASH_PROGRAM_ADDRESS, command);

    vTaskDelay(200);

    // Readout by using DATA INC each read
    uint8_t data_address = XR77129_FLASH_PROGRAM_DATA; //XR77129_FLASH_PROGRAM_DATA_INC_ADDRESS;

    if (i2c_take_by_chipid(data->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        for (uint32_t i = 0x00; i < size; i += 2)
                {
            vTaskDelay(100);

            tx[0] = XR77129_FLASH_PROGRAM_ADDRESS;
            tx[2] = (address + i) & 0xFF;
            tx[1] = ((address + i) & 0xFF00) >> 8;
            xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

            vTaskDelay(10);

            xI2CMasterWriteRead(i2c_interf, i2c_addr, data_address, rx, 2);
            printf("[%d] %02X %02X\n", (int) i, rx[0], rx[1]);
        }
        i2c_give(i2c_interf);
    }
    return true;
}

uint8_t xr77129_flash_program(xr77129_data_t * data, const uint8_t * flash_data, uint32_t size)
{
    /* program data */
    uint32_t i;
    uint8_t i2c_interf, i2c_addr;
    uint8_t tx[3] = { 0 };

    printf("Programming EXAR flash...\r\n");

    if (i2c_take_by_chipid(data->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        // ENABLE FLASH
        uint16_t command = 0x0001;
        tx[0] = XR77129_FLASH_INIT;
        tx[2] = command & 0xFF;
        tx[1] = (command & 0xFF00) >> 8;
        xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

        vTaskDelay(500);

        for (i = 0; i < size; i += 2) {

            tx[0] = XR77129_FLASH_PROGRAM_ADDRESS;
            tx[2] = i & 0xFF;
            tx[1] = (i & 0xFF00) >> 8;
            xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

            vTaskDelay(50);

            tx[0] = XR77129_FLASH_PROGRAM_DATA;
            tx[1] = flash_data[i];
            tx[2] = flash_data[i + 1];
            xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);

            vTaskDelay(200);
        }

        i2c_give(i2c_interf);
    }

    printf("Done\r\n");
    return 0;
}

uint8_t xr77129_flash_erase(xr77129_data_t * data)
{
    uint8_t page;
    uint8_t status = 0x0;
    printf("Clearing EXAR flash pages...\r\n");

    for (page = 0; page <= 6; page++) {
        status |= xr77129_flash_page_clear(data, page);
    }

    for (page = 0; page <= 6; page++) {
        status |= xr77129_flash_page_erase(data, page);
    }

    printf("Done\r\n");

    return status;
}

uint8_t xr77129_check_flash(xr77129_data_t * data, const uint8_t * xr77129_config, uint32_t size)
{
    // EXAR PROGRAMMING ROUTINE
    // If new configuration is detected, image is
    // flashed & verified
    if (!xr77129_flash_verify(data, xr77129_config, size)) {
        xr77129_flash_erase(data);
        xr77129_flash_program(data, xr77129_config, size);
        if (xr77129_flash_verify(data, xr77129_config, size)) {
            printf("EXAR flash programming successful\n");
            xr77129_reset(data);
        }
    } else {
        printf("EXAR %d flash verified\n", data->chipid);
    }

    return true;
}

uint8_t xr77129_flash_load(xr77129_data_t * data, const uint8_t * xr77129_config, uint32_t size)
{
    xr77129_flash_erase(data);
    xr77129_flash_program(data, xr77129_config, size);
    xr77129_flash_verify(data, xr77129_config, size);

    return 0;
}

uint8_t xr77129_reset(xr77129_data_t * data)
{
    uint16_t value = 0x0F00;
    return xr77129_write_value(data, XR77129_PWR_RESTART, value);
}

uint8_t xr77129_set_ready(xr77129_data_t * data, uint16_t status)
{
    xr77129_write_value(data, XR77129_CHIP_READY, status);

    uint8_t rx_len = 0;
    uint8_t reg = 0x0E;
    uint16_t value;

    vTaskDelay(100);

    rx_len = xr77129_read_value(data, reg, &value);
    printf("EXAR STATUS: %d\n", value);
    return rx_len;
}

uint8_t xr77129_read_status(xr77129_data_t * data)
{
    uint8_t reg;
    uint8_t rx_len = 0;

    for (reg = HOST_STS_REG; reg < XR77129_STATUS_REGISTERS_COUNT; reg++) {
        rx_len = xr77129_read_value(data, data->status_regs_addr[reg], &(data->status_regs[reg]));
    }
    return rx_len;
}

uint8_t xr77129_read_value(xr77129_data_t * data, uint8_t reg_address, uint16_t *read)
{
    uint8_t i2c_interf, i2c_addr;
    uint8_t val[2] = { 0 };
    uint8_t rx_len = 0;

    if (read == NULL) {
        return 0;
    }

    if (i2c_take_by_chipid(data->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        rx_len = xI2CMasterWriteRead(i2c_interf, i2c_addr, reg_address, val, sizeof(val) / sizeof(val[0]));
        i2c_give(i2c_interf);

        *read = (val[0] << 8) | (val[1]);
        return rx_len;
    }

    return 0;
}

uint8_t xr77129_write_value(xr77129_data_t * data, uint8_t reg_address, uint16_t value)
{
    uint8_t i2c_interf, i2c_addr;

    uint8_t tx[3] = {
            reg_address,
            (value & 0xFF00) >> 8,
            value & 0xFF
    };

    if (i2c_take_by_chipid(data->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        xI2CMasterWrite(i2c_interf, i2c_addr, tx, 3);
        i2c_give(i2c_interf);

        return 0;
    }

    return -1;
}

uint8_t xr77129_runtime_load(xr77129_data_t * exar, runtime_data_t * runtime_data,  uint32_t len)
{
    uint8_t i2c_interf, i2c_addr;
    uint16_t revid = { 0 };
    uint8_t rx_len = 0;

    // Restart
    xr77129_write_value(exar, XR77129_PWR_RESTART, 0x0F00);
    StopWatch_DelayMs(400);

    // Verify communication and device ID
    rx_len = xr77129_read_value(exar, 0x01, &revid);

    if (!rx_len) {
        printf("Failed to communicate with EXAR.\r\n");
        return 0;
    }

    if ((revid != XR7724_HW_ID) && (revid != XR7724_HW_ID_1)) {
        printf("Received wrong EXAR device ID.\r\n");
        return 0;
    }

    // Verify runtime loading
    uint8_t dummy = 0xFF;
    if (i2c_take_by_chipid(exar->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        xr77129_runtime_write(i2c_interf, i2c_addr, 0x8000, 0x22);
        StopWatch_DelayMs(200);
        xr77129_runtime_read(i2c_interf, i2c_addr, 0x8000, &dummy);
        i2c_give(i2c_interf);
    }
    else {
        printf("EXAR runtime dummy write failed. - 1\r\n");
        return 0;
    }

    if (dummy != 0x22) {
        printf("EXAR saved runtime variable check failed. - 1\r\n");
        return 0;
    }

    // Verify chip ready
    uint16_t val = 0xFF;
    xr77129_read_value(exar, XR77129_CHIP_READY, &val);

    if (val != 0) {
        printf("EXAR is not in configuration mode.\r\n");
        return 0;
    }

    if (i2c_take_by_chipid(exar->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {

        // Verify GPIO pin state
        uint8_t rx[2] = { 0xFF, 0XFF };
        xI2CMasterWriteRead(i2c_interf, i2c_addr, XR77129_GPIO_READ_GPIO, rx, 2);
        if ((rx[1] & exar->gpio_mask) != (exar->gpio_default & exar->gpio_mask)) {
            printf("EXAR GPIO verification failed. (%d != %d)\r\n", rx[1] & exar->gpio_mask, exar->gpio_default & exar->gpio_mask);
            return 0;
        }

        // Load configuration
		for (uint32_t i = 0; i < len; i++) {
            xr77129_runtime_write(i2c_interf, i2c_addr, runtime_data[i].address, runtime_data[i].data);
            StopWatch_DelayMs(3);

            // Verify
            uint8_t dummy = 0xFF;
            xr77129_runtime_read(i2c_interf, i2c_addr, runtime_data[i].address, &dummy);
            if (dummy != runtime_data[i].data) {
                printf("EXAR runtime configuration write failed. (%d - address: %d data: %d != %d)\r\n", (int) i, runtime_data[i].address, runtime_data[i].data, dummy);
                return 0;
            }
        }
        i2c_give(i2c_interf);
    }
    else {
        printf("EXAR runtime write failed.\r\n");
        return 0;
    }

    dummy = 0xFF;
    if (i2c_take_by_chipid(exar->chipid, &i2c_addr, &i2c_interf, portMAX_DELAY) == pdTRUE) {
        xr77129_runtime_read(i2c_interf, i2c_addr, 0x8000, &dummy);
        i2c_give(i2c_interf);
    }
    else {
        printf("EXAR runtime dummy write failed. - 2\r\n");
        return 0;
    }

    if (dummy != 0x22) {
        printf("EXAR saved runtime variable check failed. - 2\r\n");
        return 0;
    }

    // Set power chip ready
    StopWatch_DelayMs(100);
    xr77129_write_value(exar, XR77129_CHIP_READY, 0x01);
    StopWatch_DelayMs(100);

    return 1;
}
