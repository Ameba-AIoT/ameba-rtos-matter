from socket import gethostname
import os
import array as arr
import subprocess
import argparse
import os

# python3 python_custom_ecdsa_D_gcc.py <path/to/input_file> <path/to/output_file>

# Parse command line arguments
parser = argparse.ArgumentParser()
parser.add_argument("input")
parser.add_argument("output")
args = parser.parse_args()

input_file = args.input
output_file = args.output
script_dir = os.path.dirname(__file__)
abs_input_file_path = os.path.join(script_dir, input_file)
abs_output_file_path = os.path.join(script_dir, output_file)

version = 0xffffffff
version_byte = version.to_bytes(4,'little')

headernum = 0x00000001
headernum_byte = headernum.to_bytes(4,'little')

signature = 0x3141544f
signature_byte = signature.to_bytes(4,'little')

headerlen = 0x00000018
headerlen_byte = headerlen.to_bytes(4,'little')

checksum = 0;
with open(abs_input_file_path, "rb") as f:
    byte = f.read(1)
    num = int.from_bytes(byte, 'big')
    checksum += num
    while byte != b"":
        byte = f.read(1)
        num = int.from_bytes(byte, 'big')
        checksum += num
checksum_byte = checksum.to_bytes(4,'little')

imagelen = os.path.getsize(abs_input_file_path)
imagelen_bytes = imagelen.to_bytes(4, 'little')

offset = 0x00000020
offset_bytes = offset.to_bytes(4, 'little')

rvsd = 0x0800b000
rvsd_bytes = rvsd.to_bytes(4, 'little')

img2_bin = open(abs_input_file_path, 'br').read()

f = open(abs_output_file_path, 'wb')
f.write(version_byte)
f.write(headernum_byte)
f.write(signature_byte)
f.write(headerlen_byte)
f.write(checksum_byte)
f.write(imagelen_bytes)
f.write(offset_bytes)
f.write(rvsd_bytes)
f.write(img2_bin)
f.close()

#Debug info in case you want to check the actual signature binaries generated separately
'''
sf = open("ota1.sig", 'wb')
sf.write(ota1_sig)
sf.close()

sf = open("ota2.sig", 'wb')
sf.write(ota2_sig)
sf.close()
'''
