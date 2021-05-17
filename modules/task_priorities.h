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

#ifndef TASK_PRIORITIES_H_
#define TASK_PRIORITIES_H_

#ifndef tskIDLE_PRIORITY
#define tskIDLE_PRIORITY                (0)
#endif

#define tskLED_PRIORITY                 (tskIDLE_PRIORITY+1)
#define tskFPGA_COMM_PRIORITY           (tskIDLE_PRIORITY+1)
#define tskWATCHDOG_PRIORITY            (tskIDLE_PRIORITY+1)

#define tskPAYLOAD_PRIORITY             (tskIDLE_PRIORITY+2)
#define tskRTM_MANAGE_PRIORITY          (tskIDLE_PRIORITY+2)
#define tskXR77129_PRIORITY             (tskIDLE_PRIORITY+2)

#define tskSENSOR_PRIORITY              (tskIDLE_PRIORITY+3)
#define tskHOTSWAP_PRIORITY             (tskIDLE_PRIORITY+3)
#define tskLM75SENSOR_PRIORITY          (tskIDLE_PRIORITY+3)
#define tskMAX6642SENSOR_PRIORITY       (tskIDLE_PRIORITY+3)
#define tskINA220SENSOR_PRIORITY        (tskIDLE_PRIORITY+3)
#define tskINA3221SENSOR_PRIORITY       (tskIDLE_PRIORITY+3)
#define tskADT7420SENSOR_PRIORITY       (tskIDLE_PRIORITY+3)

#define tskIPMI_HANDLERS_PRIORITY       (tskIDLE_PRIORITY+4)
#define tskIPMI_PRIORITY                (tskIDLE_PRIORITY+4)

#define tskIPMB_RX_PRIORITY             (tskIDLE_PRIORITY+5)
#define tskIPMB_TX_PRIORITY             (tskIDLE_PRIORITY+5)

#endif
