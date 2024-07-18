#!/usr/bin/env python3
#
# AFCv3.1 clock switch config decoder / encoder
# Copyright (C) 2023 CNPEM LNLS
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import argparse
import json
import sys

parser = argparse.ArgumentParser(description="Decode AFCv3.1 clock configuration")
parser.add_argument("--json-out", type=str, help="Save clock configuration in a json file", required=False)
parser.add_argument("--print-reg-array", help="Print register configuration data", required=False, action="store_true")
parser.add_argument("--print-config", help="Pretty print clock configuration", required=False, action="store_true")
parser_data_in_grp = parser.add_mutually_exclusive_group(required=True)
parser_data_in_grp.add_argument("--bytes", metavar="byte", type=str, nargs=16, help="Byte array", required=False)
parser_data_in_grp.add_argument("--json-in", type=str, help="Decode clock configuration json file", required=False)

args = parser.parse_args()

afcv3_clk_in_names = [
    "FMC2_CLK3_BIDIR",
    "FMC2_CLK1_M2",
    "FMC2_CLK0_M2C",
    "FMC2_CLK2_BIDIR",
    "TCLKB",
    "TCLKA",
    "TCLKC",
    "TCLKD",
    "FCLKA",
    "FMC1_CLK3_BIDIR",
    "FMC1_CLK1_M2C",
    "FMC1_CLK0_M2C",
    "FMC1_CLK2_BIDIR",
    "WR_PLL_CLK1",
    "CLK20_VCXO",
    "SI57X_CLK"
]

afcv3_clk_out_names = [
    "TCLKD",
    "TCLKC",
    "TCLKA",
    "TCLKB",
    "FPGA_CLK1",
    "FP2_CLK2",
    "LINK01_CLK",
    "FP2_CLK1",
    "PCIE_CLK1",
    "LINK23_CLK",
    "FIN1_CLK3",
    "FIN1_CLK2",
    "RTM_SYNC_CLK",
    "OP15C",
    "FIN2_CLK2",
    "FIN2_CLK3"
]

afcv3_clk_in_names_dict = {afcv3_clk_in_names[i] : i for i in range(0, len(afcv3_clk_in_names))}

def adn4604_decode(data):
    adn4604_dict = {"afc_ver": "3.1"}
    in_cfg = {}
    out_cfg = {}
    for ch_cfg_raw, ch_in_name, ch_out_name in zip(data, afcv3_clk_in_names, afcv3_clk_out_names):
        ch_in_cfg = {
            "POL": "POS" if (ch_cfg_raw & 0x20) != 0 else "NEG",
        }
        ch_out_cfg = {
            "SRC": afcv3_clk_in_names[ch_cfg_raw & 0xF],
            "EN": True if (ch_cfg_raw & 0x80) != 0 else False,
        }
        in_cfg[ch_in_name] = ch_in_cfg
        out_cfg[ch_out_name] = ch_out_cfg
    adn4604_dict["in_cfg"] = in_cfg
    adn4604_dict["out_cfg"] = out_cfg
    return adn4604_dict

def adn4604_encode(adn4604_cfg):
    data = bytearray()
    for pin, pout in zip(afcv3_clk_in_names, afcv3_clk_out_names):
        b = afcv3_clk_in_names_dict[adn4604_cfg["out_cfg"][pout]["SRC"]]
        b = b | 0x20 if adn4604_cfg["in_cfg"][pin]["POL"] == "POS" else b
        b = b | 0x80 if adn4604_cfg["out_cfg"][pout]["EN"] == True else b
        data.append(b)
    return data

def print_adn4604(adn4604_cfg):
    for ch_out_name, ch_out_cfg in adn4604_cfg["out_cfg"].items():
        if ch_out_cfg["EN"] == True:
            ch_out_src = ch_out_cfg["SRC"]
            print("{:<19} -> {}".format(ch_out_src, ch_out_name))
            print("  └─POL: {}".format(adn4604_cfg["in_cfg"][ch_out_src]["POL"]))

if args.bytes != None:
    adn4604_raw_data = bytearray([int(i, 16) for i in args.bytes])
    clk_cfg = adn4604_decode(adn4604_raw_data)
elif args.json_in != None:
    with open(args.json_in, "r") as f:
        clk_cfg = json.load(f)
    if clk_cfg.get("afc_ver") != "3.1":
        print("JSON input not compatible with AFCv3.1!", file=sys.stderr)
        exit(1)

if args.json_out != None:
    with open(args.json_out, "w") as f:
        json.dump(clk_cfg, f, indent=True)

if args.print_config:
    print_adn4604(clk_cfg)

if args.print_reg_array:
    reg_str = ""
    reg_data = adn4604_encode(clk_cfg)
    for b in reg_data:
        reg_str = reg_str + "0x{:02X} ".format(b)
    print(reg_str)
