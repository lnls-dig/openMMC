/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2016  Henrique Silva <henrique.silva@lnls.br>
 *   Copyright (C) 2015  Piotr Miedzik  <P.Miedzik@gsi.de>
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

#ifndef __IPMI_OEM_H_
#define __IPMI_OEM_H_

/*
   Following definitions by Piotr Miedzik in JAMMCI
   https://github.com/qermit/JAMMCI/blob/master/src/ipmi/ipmi_oem.h
*/

/* Custom NetFn */
#define NETFN_CUSTOM_AFC                    0x30

/* AFC Custom Commands */
#define IPMI_AFC_CMD_I2C_TRANSFER	    0x00
#define IPMI_AFC_CMD_GPIO		    0x01
#define IPMI_AFC_CMD_CLOCK_CROSSBAR_GET	    0x02
#define IPMI_AFC_CMD_CLOCK_CROSSBAR_SET	    0x03
#define IPMI_AFC_CMD_SSP_TRANSFER	    0x04
#define IPMI_AFC_CMD_SSP_TRANSFER_RAW	    0x05



#endif
