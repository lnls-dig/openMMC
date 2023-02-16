/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2021  Augusto Fraga Giachero <augusto.fraga@cnpem.br>
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

#ifndef I2C_MUX_H_
#define I2C_MUX_H_

#include "i2c.h"
#include "error.h"
#include <stdint.h>

struct i2c_mux_dev {
	struct i2c_dev mux_dev;
	struct i2c_dev* parent_dev;
	enum i2c_bus_id bus_id;
	uint8_t addr;
	uint8_t channel;
};

/**
 * @brief Initializes a single I2C multiplexer channel
 *
 * I2C multiplexers can be nested provided that they have different slave
 * addresses.
 *
 * @param[out] mux_dev    A pointer to an i2c_mux_dev struct. This struct can
 *                        safely be casted to i2c_dev before calling
 *                        i2c_register_bus().
 * @param[in]  parent_dev Parent I2C bus.
 * @param[in]  addr       I2C slave address of the I2C mux.
 * @param[in]  channel    Which channel is associated with this bus. Should be
 *                        between 0 and 7.
 *
 * @return MMC_OK if success, an error code otherwise.
 */
mmc_err i2c_mux_init(struct i2c_mux_dev* mux_dev, struct i2c_dev* parent_dev, uint8_t addr, uint8_t channel);

#endif
