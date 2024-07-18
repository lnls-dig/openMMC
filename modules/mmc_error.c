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
 * @file   mmc_error.c
 * @author Augusto Fraga Giachero <augusto.fraga@cnpem.br>
 *
 * @brief  Error handling functions
 *
 * @ingroup error
 */
#include <stddef.h>
#include "mmc_error.h"

static const char* mmc_error_strings[] = {
    [MMC_OK] = "No error",
    [MMC_OOM_ERR] = "Out of memory",
    [MMC_IO_ERR] = "Input output error",
    [MMC_INVALID_ARG_ERR] = "Invalid arguments",
    [MMC_RESOURCE_ERR] = "Resource unavailable",
    [MMC_TIMEOUT_ERR] = "Timeout",
    [MMC_UNKNOWN_ERR] = "Unknown error",
};

const char* get_error_str(mmc_err e)
{
    const size_t eindex = (size_t) e;
    const char* err_str;

    if (eindex < (sizeof(mmc_error_strings) / sizeof(char*))) {
        err_str = mmc_error_strings[eindex];
    } else {
        err_str = mmc_error_strings[MMC_UNKNOWN_ERR];
    }

    return err_str;
}
