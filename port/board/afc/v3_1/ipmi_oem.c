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

#include "ipmi.h"
#include "ipmi_oem.h"

#include "adn4604.h"

IPMI_HANDLER(ipmi_afc_clock_crossbar_set, NETFN_CUSTOM_AFC, IPMI_AFC_CMD_CLOCK_CROSSBAR_SET, ipmi_msg *req, ipmi_msg* rsp)
{
    int len = rsp->data_len = 0;

    uint8_t map = req->data[0];
    uint8_t output = req->data[1];
    uint8_t input = req->data[2];
    uint8_t enable = req->data[3];

    adn_connect_map_t con = adn4604_out_status();

    if (output % 2) {
	*(&con+(output/2)) = (input << 4) & 0xF0;
    } else {
	*(&con+(output/2)) = input & 0x0F;
    }

    adn4604_xpt_config( map , con );

    if (enable) {
	adn4604_tx_enable( output );
    }

    adn4604_update();

    rsp->completion_code = IPMI_CC_OK;
}
