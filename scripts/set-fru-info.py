#!/usr/bin/env python3
#
# AFCv4 FRU fix (serial number and Zone3InterfaceCompatibility fields)
# Copyright (C) 2024 CNPEM LNLS
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

import frugy
from frugy.fru import Fru
import argparse

parser = argparse.ArgumentParser(description="Corrects AFCv4 FRU records")
parser.add_argument("--bin-in", type=str, help="Binary file from FRU read", required=True)
parser.add_argument("--bin-out", type=str, help="Binary file for FRU write", required=True)
parser.add_argument("--set-serial-number", metavar="serial_number", type=str, help="Serial Number", required=False)
parser.add_argument("--set-board-version", metavar="board_version", type=str, help="Board Version", default='4.0.2' ,required=False)
parser.add_argument("--save-yml", type=str, help="Save YML configuration file", required=False)

args= parser.parse_args()

fru = Fru()
fru.load_bin(args.bin_in)
yaml = fru.to_dict()
yaml['BoardInfo']['manufacturer']='Creotech'
yaml['BoardInfo']['product_name']='AFC:' + args.set_board_version
yaml['BoardInfo']['part_number']='AFC'
yaml['BoardInfo']['fru_file_id']='AFCFRU'
yaml['BoardInfo']['manufacturer']='CNPEM'
yaml['ProductInfo']['manufacturer']='CNPEM'
yaml['ProductInfo']['product_name']='AFC'
yaml['ProductInfo']['part_number']='AFC'
yaml['ProductInfo']['version']=args.set_board_version
yaml['ProductInfo']['asset_tag']='No tag'
yaml['ProductInfo']['fru_file_id']='AFCFRU'
yaml['MultirecordArea'][3]['identifier_type'] = 'CLASS_ID'
yaml['MultirecordArea'][3]['identifier_body'] = ['D1.3']
if(args.set_serial_number != None):
  yaml['BoardInfo']['serial_number']=args.set_serial_number
  yaml['ProductInfo']['serial_number']=args.set_serial_number

fru.update(yaml)

if(args.save_yml):
  fru.save_yaml(args.save_yml)

fru.save_bin(args.bin_out)
