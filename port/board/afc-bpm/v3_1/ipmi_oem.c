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

#include "ipmi_oem.h"

/* ADN4604 IPMI Control commands */
#ifdef MODULE_ADN4604

#include "adn4604.h"

/* This command may take a while to execute and hold the IPMI transaction */
IPMI_HANDLER(ipmi_oem_adn4604_cfg_output, NETFN_CUSTOM_OEM, IPMI_OEM_CMD_ADN4604_SET_OUTPUT_CFG, ipmi_msg *req, ipmi_msg* rsp)
{
    int len = rsp->data_len = 0;

    /* @todo  Read port status before setting the new configuration */
    extern adn_connect_map_t con;
    uint8_t map;
    uint8_t output = req->data[1];
    uint8_t input = req->data[2];
    uint8_t enable = req->data[3];

    if (output % 2) {
        *((uint8_t *)&con+(output/2)) &= 0x0F;
        *((uint8_t *)&con+(output/2)) |= (input << 4) & 0xF0;
    } else {
        *((uint8_t *)&con+(output/2)) &= 0xF0;
        *((uint8_t *)&con+(output/2)) |= input & 0x0F;
    }

    map = ( req->data[0] == 0 ) ? ADN_XPT_MAP0_CON_REG : ADN_XPT_MAP1_CON_REG;

    adn4604_xpt_config( map , con );

    if ( enable ) {
        adn4604_tx_control( output, TX_ENABLED );
    } else {
        adn4604_tx_control( output, TX_DISABLED );
    }

    adn4604_update();

    rsp->data_len = len;
    rsp->completion_code = IPMI_CC_OK;
}

IPMI_HANDLER(ipmi_oem_adn4604_reset, NETFN_CUSTOM_OEM, IPMI_OEM_CMD_ADN4604_RESET, ipmi_msg *req, ipmi_msg* rsp)
{
    adn4604_reset();

    rsp->data_len = 0;
    rsp->completion_code = IPMI_CC_OK;
}

#endif
