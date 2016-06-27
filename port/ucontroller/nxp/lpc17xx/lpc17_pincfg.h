/*
 *   openMMC  --
 *
 *   Copyright (C) 2015  Henrique Silva  <henrique.silva@lnls.br>
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

/**
 * @file lpc17_pincfg.h
 * @brief Pin Config functions redirection for LPC17xx
 *
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 */

#ifndef LPC17_PINCFG_H_
#define LPC17_PINCFG_H_

#include "port.h"

/**
 * @brief       Sets I/O Control pin mux
 * @param       port    : GPIO port to mux
 * @param       pin     : GPIO pin to mux
 * @param       cfg     : Configuration bits to select pin mode/function
 * @see IOCON_17XX_40XX_MODE_FUNC
 */
#define pin_config(port, pin, cfg) Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, cfg)

#endif
