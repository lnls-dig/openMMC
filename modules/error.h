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

#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>

/**
 * @file   error.h
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  Error handling functions and codes
 *
 * @ingroup error
 */

typedef enum {
    MMC_OK,
    MMC_OOM_ERR,
    MMC_IO_ERR,
    MMC_INVALID_ARG_ERR,
    MMC_TIMEOUT_ERR,
    MMC_RESOURCE_ERR,
    MMC_UNKNOWN_ERR,
} mmc_err;

/**
 * @brief Get the corresponding string for an error code
 *
 * @param[in] e  Error code
 *
 * @return a pointer to a null-terminated string
 */
const char* get_error_str(mmc_err e);

/**
 * @brief Print the file name, line number and error description
 *
 * @param[in] e  Error code
 */
#define PRINT_ERR_LINE(e) printf("%s line %d: %s\n", __FILE__, __LINE__, get_error_str(e))

#endif
