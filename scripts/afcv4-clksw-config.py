#!/usr/bin/env python3
#
# AFCv4 clock switch config decoder / encoder
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

parser = argparse.ArgumentParser(description="Decode AFCv4 clock configuration")
parser.add_argument("--json-out", type=str, help="Save clock configuration in a json file", required=False)
parser.add_argument("--print-reg-array", help="Print register configuration data", required=False, action="store_true")
parser.add_argument("--print-config", help="Pretty print clock configuration", required=False, action="store_true")
parser_data_in_grp = parser.add_mutually_exclusive_group(required=True)
parser_data_in_grp.add_argument("--bytes", metavar="byte", type=str, nargs=16, help="Byte array", required=False)
parser_data_in_grp.add_argument("--json-in", type=str, help="Decode clock configuration json file", required=False)

args = parser.parse_args()

afcv4_clk_names = [
    "AMC_CLK_RTM_CLK",
    "FMC2_CLK1_M2C_FMC2_CLK3_BIDIR",
    "FMC1_CLK1_M2C_FMC1_CLK3_BIDIR",
    "FMC1_CLK0_M2C",
    "FMC1_CLK2_BIDIR",
    "SI57X_PRI",
    "FMC2_CLK0_M2C",
    "FMC2_CLK2_BIDIR",
    "TCLKD_FLEX_FPGA_CLK3",
    "TCLKC_FLEX_FPGA_CLK2",
    "TCLKA",
    "TCLKB",
    "FLEX_GTP113_CLK0",
    "FLEX_GTP113_GTP216_CLK1",
    "FLEX_GTP116_CLK1_RTM_GTP03",
    "FLEX_GTP213_CLK1_RTM_GTP47"
]

afcv4_clk_names_dict = {afcv4_clk_names[i] : i for i in range(0, len(afcv4_clk_names))}

def idt_decode(data):
    idt_dict = {}
    for ch_cfg_raw, ch_name in zip(data, afcv4_clk_names):
        ch_cfg = {
            "SRC": afcv4_clk_names[ch_cfg_raw & 0xF],
            "POL": "POS" if (ch_cfg_raw & 0x20) != 0 else "NEG",
            "TERM": "ON" if (ch_cfg_raw & 0x40) != 0 else "OFF",
            "DIR": "OUT" if (ch_cfg_raw & 0x80) != 0 else "IN",
        }
        idt_dict[ch_name] = ch_cfg
    return {"afc_ver": "4.0", "cfg": idt_dict}

def idt_encode(idt_cfg):
    data = bytearray()
    for port in afcv4_clk_names:
        b = afcv4_clk_names_dict[idt_cfg["cfg"][port]["SRC"]]
        b = b | 0x20 if idt_cfg["cfg"][port]["POL"] == "POS" else b
        b = b | 0x40 if idt_cfg["cfg"][port]["TERM"] == "ON" else b
        b = b | 0x80 if idt_cfg["cfg"][port]["DIR"] == "OUT" else b
        data.append(b)
    return data

def print_idt(idt_cfg):
    for ch_name, ch_cfg in idt_cfg["cfg"].items():
        if ch_cfg["DIR"] == "OUT":
            ch_src = ch_cfg["SRC"]
            print("{:<29} -> {}".format(ch_src, ch_name))
            print("  ├─POL: {:<24} ├─POL: {}".format(idt_cfg["cfg"][ch_src]["POL"], ch_cfg["POL"]))
            print("  └─TERM: {:<23} └─TERM: {}".format(idt_cfg["cfg"][ch_src]["TERM"], ch_cfg["TERM"]))

if args.bytes != None:
    idt_raw_data = bytearray([int(i, 16) for i in args.bytes])
    clk_cfg = idt_decode(idt_raw_data)
elif args.json_in != None:
    with open(args.json_in, "r") as f:
        clk_cfg = json.load(f)
    if clk_cfg.get("afc_ver") != "4.0":
        print("JSON input not compatible with AFCv4!", file=sys.stderr)
        exit(1)

if args.json_out != None:
    with open(args.json_out, "w") as f:
        json.dump(clk_cfg, f, indent=True)

if args.print_config:
    print_idt(clk_cfg)

if args.print_reg_array:
    reg_str = ""
    reg_data = idt_encode(clk_cfg)
    for b in reg_data:
        reg_str = reg_str + "0x{:02X} ".format(b)
    print(reg_str)
