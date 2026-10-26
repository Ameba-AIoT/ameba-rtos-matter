#!/usr/bin/env python3

#
#    Copyright (c) 2026 Realtek Semiconductor Corporation. All rights reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

"""
Ameba OTA container (ota_all.bin) utility.

The Ameba SDK build produces two application artifacts:

  * the bare application image - `app.bin` for AmebaGreen2, `km0_km4_app.bin`
    for AmebaDplus, `kr4_km4_app.bin` for AmebaLite, `km0_km4_ca32_app.bin` for
    AmebaSmart. It holds certificate + manifest + sub-images and is what lands
    verbatim in an OTA slot.
  * the OTA container `ota_all.bin` - the same application image prefixed with
    the Ameba OTA file header that the on-device OTA client (`component/ota`)
    parses to learn where each image lives and what its checksum is.

This tool parses and validates that container header, so an `ota_all.bin` can be
checked before it is wrapped into a Matter OTA image with `ota_image_tool.py`.

Container layout, as written by
`tools/scripts/image_process/op_ota_prepend_header.py`:
::

    offset 0                FwVer      u32 LE   container format version (0xFFFFFFFF)
    offset 4                HdrNum     u32 LE   number of image sub-headers
    offset 8                sub-header #0                      \\ 24 B each,
    offset 8 + 24           sub-header #1                      / HdrNum entries
    offset 8 + 24 * HdrNum  image data, images concatenated in sub-header order

Each 24-byte sub-header is little-endian except `Signature`:
::

    Signature  4 B    "OTA1" / "OTA2" / ... ("OTA" followed by one digit)
    ImgHdrLen  u32    length of this sub-header (24)
    Checksum   u32    sum of all bytes of the image, modulo 2**32
    ImgLen     u32    length of the image
    Offset     u32    offset of the image from the start of the container
    ImgID      u32    0 = bootloader, 1 = application, 2 = AP image

Usage examples:
::

    ./ota_all_tool.py show ota_all.bin
    ./ota_all_tool.py verify ota_all.bin
    ./ota_all_tool.py extract --img-id 1 ota_all.bin app.bin
"""

import argparse
import struct
import sys

# FwVer, HdrNum
FILE_HEADER_FORMAT = '<II'
# Signature, ImgHdrLen, Checksum, ImgLen, Offset, ImgID
SUB_HEADER_FORMAT = '<4sIIIII'

FILE_HEADER_SIZE = struct.calcsize(FILE_HEADER_FORMAT)
SUB_HEADER_SIZE = struct.calcsize(SUB_HEADER_FORMAT)

OTA_SIGNATURE_PREFIX = b'OTA'

# Sanity bound for HdrNum. The SDK allows OTA_IMGID_MAX images, which is 2 for
# AmebaDplus/AmebaLite/AmebaGreen2 and 3 for AmebaSmart.
MAX_IMG_NUM = 3

IMG_ID_BOOT = 0
IMG_ID_APP = 1
IMG_ID_APIMG = 2

IMG_ID_NAMES = {
    IMG_ID_BOOT: 'bootloader',
    IMG_ID_APP: 'application',
    IMG_ID_APIMG: 'ap-image',
}


def warn(message: str):
    sys.stderr.write(f'warning: {message}\n')


def error(message: str):
    sys.stderr.write(f'error: {message}\n')
    sys.exit(1)


def img_id_name(img_id: int) -> str:
    return IMG_ID_NAMES.get(img_id, f'unknown({img_id})')


# The application image leads with the certificate pattern "81958711", which is
# how the bootloader recognises a valid slot. Used only to give a clearer error
# when a bare app image is passed instead of the container.
APP_IMAGE_PATTERN = b'81958711'


class ImageHeader:
    """One 24-byte Ameba OTA image sub-header."""

    __slots__ = ('signature', 'hdr_len', 'checksum', 'img_len', 'offset', 'img_id')

    def __init__(self, signature: bytes, hdr_len: int, checksum: int,
                 img_len: int, offset: int, img_id: int):
        self.signature = signature
        self.hdr_len = hdr_len
        self.checksum = checksum
        self.img_len = img_len
        self.offset = offset
        self.img_id = img_id


def parse_container(data: bytes):
    """Parse an Ameba OTA container.

    Returns (fw_ver, images) where images is a list of ImageHeader in
    sub-header order. Exits on a malformed file header.
    """
    if len(data) < FILE_HEADER_SIZE:
        error(f'file is {len(data)} B, too small to hold an Ameba OTA file header '
              f'({FILE_HEADER_SIZE} B)')

    fw_ver, hdr_num = struct.unpack_from(FILE_HEADER_FORMAT, data, 0)

    if hdr_num == 0:
        error('Ameba OTA file header declares 0 images; this is not an ota_all.bin')
    if hdr_num > MAX_IMG_NUM:
        if data[:len(APP_IMAGE_PATTERN)] == APP_IMAGE_PATTERN:
            error('this looks like a bare application image (certificate pattern '
                  'present at offset 0), not the Ameba OTA container; pass '
                  'ota_all.bin instead')
        error(f'Ameba OTA file header declares {hdr_num} images, '
              f'max supported is {MAX_IMG_NUM}; this is not an ota_all.bin')

    header_size = FILE_HEADER_SIZE + hdr_num * SUB_HEADER_SIZE
    if len(data) < header_size:
        error(f'file truncated: {header_size} B of header declared, '
              f'file holds {len(data)} B')

    images = []
    for i in range(hdr_num):
        fields = struct.unpack_from(SUB_HEADER_FORMAT, data,
                                    FILE_HEADER_SIZE + i * SUB_HEADER_SIZE)
        images.append(ImageHeader(*fields))

    return fw_ver, images


def image_checksum(data: bytes, img: ImageHeader) -> int:
    """Sum of the image bytes, modulo 2**32, as the SDK computes it."""
    return sum(data[img.offset:img.offset + img.img_len]) & 0xFFFFFFFF


def verify_container(data: bytes):
    """Validate an Ameba OTA container.

    Returns a list of human-readable problems; empty means the container is
    consistent.
    """
    fw_ver, images = parse_container(data)
    problems = []

    expected_offset = FILE_HEADER_SIZE + len(images) * SUB_HEADER_SIZE
    for index, img in enumerate(images):
        label = f'image[{index}] (ImgID={img.img_id} {img_id_name(img.img_id)})'

        if img.signature[:len(OTA_SIGNATURE_PREFIX)] != OTA_SIGNATURE_PREFIX:
            problems.append(f'{label}: Signature {img.signature!r} does not start '
                            f'with {OTA_SIGNATURE_PREFIX!r}')

        if img.hdr_len != SUB_HEADER_SIZE:
            problems.append(f'{label}: ImgHdrLen is {img.hdr_len}, '
                            f'expected {SUB_HEADER_SIZE}')

        if img.img_len == 0:
            problems.append(f'{label}: ImgLen is 0')

        if img.offset != expected_offset:
            problems.append(f'{label}: Offset is {img.offset}, '
                            f'expected {expected_offset} (images must be contiguous)')
        expected_offset = img.offset + img.img_len

        if img.offset + img.img_len > len(data):
            problems.append(f'{label}: Offset+ImgLen is {img.offset + img.img_len}, '
                            f'past the end of the {len(data)} B file')
            continue

        actual = image_checksum(data, img)
        if actual != img.checksum:
            problems.append(f'{label}: Checksum is 0x{img.checksum:08x}, '
                            f'computed 0x{actual:08x}')

    if expected_offset != len(data):
        # Trailing bytes are harmless for the device (it stops at ImgLen) but
        # usually mean the file was concatenated with something else.
        warn(f'{len(data) - expected_offset} trailing byte(s) after the last image')

    if not any(img.img_id == IMG_ID_APP for img in images):
        problems.append('no application image (ImgID=1) in the container; '
                        'a Matter OTA image must carry one')

    return problems


def find_image(images, img_id: int):
    for img in images:
        if img.img_id == img_id:
            return img
    return None


def cmd_show(args):
    with open(args.image_file, 'rb') as file:
        data = file.read()

    fw_ver, images = parse_container(data)

    print(f'File: {args.image_file}')
    print(f'Size: {len(data)}')
    print('Ameba OTA file header:')
    print(f'  FwVer:  {fw_ver} (0x{fw_ver:x})')
    print(f'  HdrNum: {len(images)}')
    print('Image sub-headers:')
    for index, img in enumerate(images):
        print(f'  [{index}] Signature: {img.signature.decode(errors="replace")}')
        print(f'      ImgHdrLen: {img.hdr_len}')
        print(f'      Checksum:  {img.checksum} (0x{img.checksum:x})')
        print(f'      ImgLen:    {img.img_len}')
        print(f'      Offset:    {img.offset}')
        print(f'      ImgID:     {img.img_id} ({img_id_name(img.img_id)})')


def cmd_verify(args):
    with open(args.image_file, 'rb') as file:
        data = file.read()

    problems = verify_container(data)

    if problems:
        for problem in problems:
            sys.stderr.write(f'error: {problem}\n')
        error(f'{args.image_file}: {len(problems)} problem(s) found, NOT a usable '
              f'Ameba OTA container')

    fw_ver, images = parse_container(data)
    print(f'{args.image_file}: OK (FwVer 0x{fw_ver:x}, {len(images)} image(s), '
          f'{len(data)} B)')
    for img in images:
        print(f'  ImgID={img.img_id} ({img_id_name(img.img_id)}) '
              f'offset={img.offset} len={img.img_len} '
              f'checksum=0x{img.checksum:08x} OK')
    return 0


def cmd_extract(args):
    with open(args.image_file, 'rb') as file:
        data = file.read()

    _fw_ver, images = parse_container(data)
    img = find_image(images, args.img_id)
    if img is None:
        available = ', '.join(
            f'{i.img_id} ({img_id_name(i.img_id)})' for i in images)
        error(f'no image with ImgID={args.img_id}; container holds: {available}')

    if img.offset + img.img_len > len(data):
        error(f'image[{args.img_id}] extends past the end of the file')

    payload = data[img.offset:img.offset + img.img_len]
    with open(args.output_file, 'wb') as file:
        file.write(payload)

    print(f'Wrote {args.output_file}: ImgID={args.img_id} '
          f'({img_id_name(img.img_id)}), {len(payload)} B')
    return 0


def main():
    parser = argparse.ArgumentParser(
        description='Ameba OTA container (ota_all.bin) utility')
    subcommands = parser.add_subparsers(
        dest='subcommand', title='valid subcommands', required=True)

    show_parser = subcommands.add_parser('show', help='Show the container header')
    show_parser.add_argument('image_file', help='Path to the ota_all.bin file')
    show_parser.set_defaults(func=cmd_show)

    verify_parser = subcommands.add_parser(
        'verify', help='Validate the container header and image checksums')
    verify_parser.add_argument('image_file', help='Path to the ota_all.bin file')
    verify_parser.set_defaults(func=cmd_verify)

    extract_parser = subcommands.add_parser(
        'extract', help='Extract one image from the container')
    extract_parser.add_argument('-id', '--img-id', type=int, default=IMG_ID_APP,
                                help='ImgID to extract (default: 1, application)')
    extract_parser.add_argument('image_file', help='Path to the ota_all.bin file')
    extract_parser.add_argument('output_file', help='Path to the output image')
    extract_parser.set_defaults(func=cmd_extract)

    args = parser.parse_args()
    sys.exit(args.func(args))


if __name__ == '__main__':
    main()
