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

/**
 * @file   sys_utils.c
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  System utilities functions
 *
 * @ingroup sysutils
 */

/**
 * @brief Schedule a MCU reset
 *
 * @param[in] period_ms  Reset the MCU after a period specified in milisseconds
 *
 * @return 0 if successful, non zero if there was an error
 */
int sys_schedule_reset(int period_ms);
