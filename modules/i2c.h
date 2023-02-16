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

#ifndef I2C_H_
#define I2C_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "error.h"
#include <stdint.h>
#include <stdbool.h>

#define I2C_M_READ           0x0001 /* Read data, from slave to master */
#define I2C_M_TEN            0x0002 /* Ten bit address */
#define I2C_M_NOSTOP         0x0040 /* Message should not end with a STOP */
#define I2C_M_NOSTART        0x0080 /* Message should not begin with a START */

/**
 * @brief I2C message struct (copied from Nuttx)
 */
struct i2c_msg {
    uint16_t addr;              /* Slave address (7- or 10-bit) */
    uint16_t flags;             /* See I2C_M_* definitions */
    uint8_t* buffer;            /* Buffer to be transferred */
    ssize_t length;             /* Length of the buffer in bytes */
    struct i2c_msg* next;       /* Next message to be sent or NULL */
};

struct i2c_dev {
    mmc_err (*master_transfer) (struct i2c_dev* dev, struct i2c_msg* msgs);
};

/**
 * @brief Register an I2C bus
 *
 * This should be called only during hardware initialization. Currently
 * there is no way of unregistering I2C buses.
 *
 * @param[in] bus_id Bus id enum
 * @param[in] dev    A pointer to an I2C dev struct. This should remain
 *                   valid for the entire program life time.
 *
 * @return MMC_OK if success, an error code otherwise
 */
mmc_err i2c_register_bus(enum i2c_bus_id bus_id, struct i2c_dev* dev);


mmc_err i2c_master_trans(enum i2c_bus_id bus_id, struct i2c_msg* msgs, int count);

/**
 * @brief I2C Chips information regarding the bus and slave address
 */
typedef struct i2c_chip_mapping {
    uint8_t bus_id;                 /**< User-defined bus ID
                                     * @note This bus has to be defined in a i2c_bus_mapping_t table
                                     */
    uint8_t i2c_address;            /**< Chip I2C slave address (7-bit) */
} i2c_chip_mapping_t;

/**
 * @brief I2C Bus description
 */
typedef struct i2c_bus_mapping {
    uint8_t i2c_interface;          /**< Physical I2C bus number */
    int8_t mux_bus;                 /**< Mux options */
    uint8_t enabled;                /**< Enabled flag */
} i2c_bus_mapping_t;

/**
 * @brief I2C Mux state
 */
typedef struct i2c_mux_state {
    uint8_t i2c_interface;         /**< Physical I2C bus number */
    int8_t state;                   /**< Mux state */
    SemaphoreHandle_t semaphore;    /**< Bus semaphore handle */
} i2c_mux_state_t;

/**
 * @brief Initialize peripheral I2C buses
 *
 * This function initializes all buses listed on the i2c_mux table, configuring the controller hardware and creating a binary semaphore for each.
 */
void i2c_init( void );

/**
 * @brief Change I2C bus mux state
 *
 * @param bus_id Target bus ID
 * @param i2c_mux Pointer to bus mux structure
 * @param new_state New bus mux state
 *
 * @retval true Bus was successfuly changed to the desired mux setting
 * @retval false Bus could not be changed to the desired mux setting
 */
bool i2c_set_mux_bus( uint8_t bus_id, i2c_mux_state_t *i2c_mux, int8_t new_state );

/**
 * @brief Read I2C bus mux state
 *
 * @param bus_id Target bus ID
 * @param i2c_mux Pointer to bus mux structure
 *
 * @return Bus current state
 */
uint8_t i2c_get_mux_bus( uint8_t bus_id, i2c_mux_state_t *i2c_mux );

/**
 * @brief Take control over an I2C bus given a bus id
 *
 * @param[in] bus_id Bus ID to take control
 * @param[out] i2c_interface Pointer to variable that will hold the I2C physical bus ID
 * @param[in] timeout Limit time to perform this operation
 *
 * @retval true Bus was successfuly gained
 * @retval false Could not gain bus
 */
bool i2c_take_by_busid( uint8_t bus_id, uint8_t *i2c_interface, uint32_t timeout );

/**
 * @brief Take control over an I2C bus given a chip id
 *
 * @param[in] chip_id Chip ID to communicate
 * @param[out] i2c_address Pointer to variable that will hold the chip slave address
 * @param[out] i2c_interface Pointer to variable that will hold the I2C physical bus ID
 * @param[in] timeout Limit time to perform this operation
 *
 * @retval true Bus was successfuly gained
 * @retval false Could not gain bus
 */
bool i2c_take_by_chipid( uint8_t chip_id, uint8_t *i2c_address, uint8_t * i2c_interface,  TickType_t timeout );

/**
 * @brief Release the previously gained I2C bus
 *
 * @param i2c_interface Physical I2C bus ID
 */
void i2c_give( uint8_t i2c_interface );

#endif
