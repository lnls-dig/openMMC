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

/**
 * @defgroup SENSORS Sensors
 * @ingroup PERIPH_IC
 */

/**
 * @file sensors.h
 * @author Henrique Silva <henrique.silva@lnls.br>, LNLS
 *
 * @brief Shortcut to include all sensors headers (not mandatory, user can include each sensor separately)
 *
 * @ingroup SENSORS
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include "port.h"

#if defined(MODULE_INA220_VOLTAGE) || defined(MODULE_INA220_CURRENT)
#include "ina220.h"
#endif

#if defined(MODULE_INA3221_VOLTAGE) || defined(MODULE_INA3221_CURRENT)
#include "ina3221.h"
#endif

#ifdef MODULE_HOTSWAP
#include "hotswap.h"
#endif

#ifdef MODULE_LM75
#include "lm75.h"
#endif

#ifdef MODULE_MAX6642
#include "max6642.h"
#endif

#endif
