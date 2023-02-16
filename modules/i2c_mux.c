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

#include "FreeRTOS.h"
#include "semphr.h"
#include "i2c_mux.h"

static mmc_err i2c_mux_transfer(struct i2c_dev* dev, struct i2c_msg* msgs) {
	struct i2c_mux_dev* dev_priv = (struct i2c_mux_dev*) dev;
	uint8_t ch_sel = 1 << dev_priv->channel;
	struct i2c_msg mux_msg = {
		.addr = dev_priv->addr,
		.flags = 0,
		.buffer = &ch_sel,
		.length = 1,
		.next = msgs,
	};

	/*
	 * Always set the I2C mux channel before each master transfer.
	 * This could be optmized by mantaining a local copy of the mux
	 * state and only reconfiguring the mux if necessary, but you
	 * would need to do it in a thread safe way.
	 *
	 */

	return dev_priv->parent_dev->master_transfer(dev, mux_msg);
}

mmc_err i2c_mux_init(struct i2c_mux_dev* mux_dev, struct i2c_dev* parent_dev, uint8_t addr, uint8_t channel) {
	mux_dev->mux_dev.master_transfer = i2c_mux_transfer;
	mux_dev->parent_dev = parent_dev;
	mux_dev->bus_id = bus_id;
	mux_dev->addr = addr;

	if (channel > 7) {
		return MMC_INVALID_ARG_ERR;
	}

	dev->channel = channel;

	return MMC_OK;
}
