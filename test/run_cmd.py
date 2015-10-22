import subprocess
import argparse
from time import sleep

parser = argparse.ArgumentParser()
parser.add_argument('-t', '--target', help='I2C slave address of AFC Board')
parser.add_argument('-i', '--ip', help='MCH IP address', default='10.0.18.192')
parser.add_argument('-n', '--netfn', help='Net Function', required=True)
parser.add_argument('-c', '--cmd', help='Command', required=True)
parser.add_argument('-d', '--data', nargs='+', help='Data')
args = parser.parse_args()

ipmi_cmd = ['ipmitool']
ipmi_cmd.extend(['-I', 'lan'])
ipmi_cmd.extend(['-H', args.ip])
ipmi_cmd.extend(['-P', ''])
ipmi_cmd.extend(['-T', '0x82'])
ipmi_cmd.extend(['-m', '0x20'])
ipmi_cmd.extend(['-t', args.target])
ipmi_cmd.extend(['raw', args.netfn, args.cmd])
if (args.data):
    ipmi_cmd.extend(args.data)

while 1:
    p = subprocess.Popen(ipmi_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    model = ' 0a 02 05 50 02 1f 5a 31 00 01 01\n'
    if (out == model) :
        print 'Get_Device_ID has run successfully!'
    else:
        print 'Error!!'
    sleep(0.2)
