# Matter OTA Guide

Follow this guide to carry out Matter standard Over the Air Software Update

## Contents

- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Quick start](#quick start)
- [Configure device identity and version](#configure-device-identity-and-version)
- [Build the OTA firmware](#build-the-ota-firmware)
- [Signed and unsigned OTA images](#signed-and-unsigned-ota-images)
- [Validate and package the Matter OTA image](#validate-and-package-the-matter-ota-image)
- [Flash layout requirements](#flash-layout-requirements)
- [Run the OTA update](#run-the-ota-update)
- [Verify the update](#verify-the-update)
- [Troubleshooting](#troubleshooting)
- [Ameba OTA container reference](#ameba-ota-container-reference)

## Overview

The update uses these components:

1. **Ameba firmware build** produces an Ameba OTA container (`ota_all.bin`).
2. **Container validation** checks the Ameba header and image checksum.
3. **Matter OTA packaging** wraps the Ameba container in a Matter OTA image with the device's Vendor ID, Product ID, software version, version string, and digest.
4. **OTA Provider** serves the Matter OTA image.
5. **OTA Requestor** downloads the image, validates it, writes the application image to the OTA slot, and applies the update.

Use the Ameba OTA container as the input to Matter packaging. Do not package the bare application image (`app.bin` or a related application binary).

## Quick start

This is the normal end-to-end flow. Complete the signing/key-provisioning step as well if `CONFIG_OTA_FW_SIGN_VERIFY` is enabled.

1. Configure the device Vendor ID, Product ID, and a software version newer than the currently installed version.
2. Apply the Matter configuration and build the firmware.
3. Confirm the build produced `ota_all.bin`.
4. Validate the Ameba OTA container.
5. Generate the Matter OTA image with `matter_ota_image`.
6. Inspect the generated image and copy it to the Matter OTA Provider's directory.
7. Start the Matter OTA Provider and pair/configure the provider and target device as described below.
8. Announce the provider to the OTA Requestor.
9. Confirm that the device reboots and reports the expected software version.

The build and packaging steps below provide the exact commands and additional detail.

## Prerequisites

From the `connectedhomeip` repository build Linux `ota-provider-app`

    cd connectedhomeip
    source scripts/activate.sh
    ./scripts/examples/gn_build_example.sh examples/ota-provider-app/linux/ ota-provider

## Configure device identity and version

The Matter OTA image's Vendor ID, Product ID, and software version must match the device's Basic Information cluster. The new software version must be greater than the currently running Matter software version.

Read the current version:

```sh
./chip-tool basicinformation read software-version 1 0
./chip-tool basicinformation read software-version-string 1 0
```

Read the device identity if needed:

```sh
./chip-tool basicinformation read vendor-id 1 0
./chip-tool basicinformation read product-id 1 0
```

The values below are configured through menuconfig and applied by running `matter_apply_conf`, which writes them into `current_proj.conf`.

| Menuconfig option | Default | Meaning |
|---|---:|---|
| `MATTER_DEVICE_VENDOR_ID` | `0xFFF1` | Matter Vendor ID |
| `MATTER_DEVICE_PRODUCT_ID` | `0x8001` | Matter Product ID |
| `MATTER_DEVICE_SOFTWARE_VERSION` | `2` | Numeric software version |
| `MATTER_DEVICE_SOFTWARE_VERSION_STR` | `"2.0"` | Software version string |

`0xFFF1` and `0x8001` are Matter test Vendor ID and Product ID values. Replace them with the appropriate values for your product when required.

### Matter software version vs. Ameba image version

The Matter software version is used for Matter OTA eligibility. The Ameba bootloader also has its own image version (`img_ver_major` / `img_ver_minor`) for rollback protection. In the documented ameba-rtos v1.2 layout, those values are in the `image2` entry in:

```text
component/soc/amebaxx/project/manifest.json5
```

These are separate mechanisms. Consult the SDK documentation for the bootloader image-version behavior.

## Build the OTA image

`matter_build_proj` generates the image automatically as the last step of the
build, at `build_<SOC>/matter_ota_image.bin` next to `ota_all.bin`.

To generate it by hand, from the `ameba-rtos-matter` directory after
`matter_apply_conf` and `matter_build_proj`:

    matter_ota_image [output image]

The script reads the device identity back out of the menuconfig, validates the
Ameba OTA container, and runs the steps below. It writes to
`./matter_ota_all.bin` unless you pass a path:

    $ matter_ota_image
    Matter OTA image
      container:   /path/to/ameba-rtos-matter/build_RTL8721F/ota_all.bin
      vendor ID:   0xFFF1
      product ID:  0x8001
      version:     2 (2.0)
      output:      /path/to/ameba-rtos-matter/matter_ota_all.bin
    /path/to/build_RTL8721F/ota_all.bin: OK (FwVer 0xffffffff, 1 image(s), 1713248 B)
    Wrote matter_ota_all.bin

The rest of this section documents what the script does, in case you need to run
the steps yourself or check its result.

#### 1. Start from the Ameba OTA container

The Matter OTA image must be built from the **Ameba OTA container**, i.e.
`ota_all.bin`, not from the bare application image. The build produces both:

| Artifact | Contents | Use for Matter OTA |
|---|---|---|
| `ota_all.bin` | 32-byte Ameba OTA header + application image | **yes** |
| `app.bin` / `km0_km4_app.bin` / `kr4_km4_app.bin` / `km0_km4_ca32_app.bin` | application image only | no |

The 32-byte Ameba OTA header tells the OTA Requestor how long the application
image is and what its checksum is:

    offset 0   FwVer      u32 LE   container format version
    offset 4   HdrNum     u32 LE   number of image sub-headers
    offset 8   sub-header #0 (24 bytes):
                 +0  Signature  4 B   "OTA1"
                 +4  ImgHdrLen  u32   length of this sub-header (24)
                 +8  Checksum   u32   sum of all image bytes, modulo 2^32
                 +12 ImgLen     u32   length of the application image
                 +16 Offset     u32   offset of the image in the container
                 +20 ImgID      u32   0 = bootloader, 1 = application
    offset 32  application image

On the device, `matter_ota_store_header()` collects these 32 bytes, validates
them, and `matter_ota_flash_burst_write()` writes only the application image to
the OTA slot. The image is checked against the declared length and checksum
before the new slot is committed, so a truncated or corrupted download is
rejected instead of being booted.

#### 2. Validate the container with `ota_all_tool.py`

`ota_all_tool.py` reads and validates `ota_all.bin` before you wrap it into a
Matter OTA image, so a bad or truncated build is caught on the host rather than
on the device. `matter_ota_image` runs `verify` for you.

    python3 ota_all_tool.py show <ota_all.bin>
    python3 ota_all_tool.py verify <ota_all.bin>
    python3 ota_all_tool.py extract [-id <IMGID>] <ota_all.bin> <output file>

`show` prints the header:

    $ python3 ota_all_tool.py show build_RTL8721F/ota_all.bin
    File: build_RTL8721F/ota_all.bin
    Size: 1713248
    Ameba OTA file header:
      FwVer:  4294967295 (0xffffffff)
      HdrNum: 1
    Image sub-headers:
      [0] Signature: OTA1
          ImgHdrLen: 24
          Checksum:  181411802 (0xad01fda)
          ImgLen:    1713216
          Offset:    32
          ImgID:     1 (application)

`verify` checks that the image signature is "OTA...", that `ImgHdrLen` is 24,
that `ImgID` is 1 (application), that the images are contiguous, and that each
`Checksum` matches the image bytes. It exits non-zero and prints one `error:`
line per problem:

    $ python3 ota_all_tool.py verify build_RTL8721F/ota_all.bin
    build_RTL8721F/ota_all.bin: OK (FwVer 0xffffffff, 1 image(s), 1713248 B)
      ImgID=1 (application) offset=32 len=1713216 checksum=0x0ad01fda OK

A container whose checksum does not match is rejected, as is a bare application
image passed by mistake:

    $ python3 ota_all_tool.py verify ota_all.bin
    error: image[0] (ImgID=1 application): Checksum is 0x0ad01fda, computed 0x0ad01edb
    error: ota_all.bin: 1 problem(s) found, NOT a usable Ameba OTA container

    $ python3 ota_all_tool.py verify app.bin
    error: this looks like a bare application image (certificate pattern present at offset 0), not the Ameba OTA container; pass ota_all.bin instead

`extract` pulls one image out; `-id` defaults to `1`, the application image that
lands in the OTA slot, so extracting it reproduces the SDK's `app.bin`:

    python3 ota_all_tool.py extract -id 1 build_RTL8721F/ota_all.bin app.bin

#### 3. Create the Matter OTA image

`matter_ota_image` takes care of this. To do it by hand, pass the container
(`ota_all.bin`, or `ota_all_signed.bin` for a signed build) — never the bare
application image:

    python3 ota_image_tool.py create -v <VENDORID> -p <PRODUCTID> -vn <VERSION> -vs <VERSIONSTRING> -da <DIGESTALGO> <path to ota_all.bin> <output ota image>

For example, on Green2:

    python3 ota_image_tool.py create -v 0xFFF1 -p 0x8001 -vn 2 -vs "2.0" -da sha256 ../../../../../build_RTL8721F/ota_all.bin matter_ota_all.bin

Substitute the `build_<SOC>` directory for your chip: `build_RTL8721Dx` for
AmebaDplus, `build_RTL8720E` for AmebaLite, `build_RTL8730E` for AmebaSmart,
`build_RTL8721F` for AmebaGreen2.

`VERSION` and `VERSIONSTRING` must match your
`CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION` and
`CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING`. `matter_ota_image` reads
these from the menuconfig, so they cannot drift.

#### 4. Check the result and serve it

    python3 ota_image_tool.py show matter_ota_all.bin

Once the image looks right, copy it to the directory the ota-provider serves from:

    cp matter_ota_all.bin <path to the ota-provider directory>

## Flash layout

OTA needs two application slots, `IMG_APP_OTA1` and `IMG_APP_OTA2`, in the flash
layout. The SDK ships a working default for every SoC, so in the common case
there is nothing to configure.

The layout is defined in the base SDK:

    component/soc/usrcfg/<soc>/ameba_flashcfg.c        # ameba-rtos v1.2 / v1.1
    component/soc/<soc>/usrcfg/ameba_flashcfg.c        # ameba-rtos v1.0

Consult the Ameba ApplicationNote for your chip at
[realmcu.com](https://www.realmcu.com/) for the flash layout chapter, and
`ameba_flashcfg.c` itself for the addresses in use. This guide does not restate
the layout: it changes between SDK versions and SoCs, and the SDK is the
authoritative source.

The only OTA-specific points worth carrying over:

- Both application slots must be large enough for your image. The layout does
  not enforce this, so an image that fits OTA1 but not OTA2 fails at update time
  rather than at build time.
- With `CONFIG_OTA_FW_SIGN_VERIFY` enabled, an `OTA_PUBKEY` region is also
  required.

After changing the layout, rebuild the firmware — the addresses are read at
runtime via `flash_get_layout_info()`.

## (WIP) Signing the OTA image (`CONFIG_OTA_FW_SIGN_VERIFY`)

Enabling `CONFIG_OTA_FW_SIGN_VERIFY` makes the device verify an ECDSA P-256
signature over the image before it commits the new slot. An unsigned or tampered
image is written but never made bootable, so the device stays on its current
firmware.

The build then produces two extra files next to `ota_all.bin`:

| File | Purpose |
|---|---|
| `ota_all_signed.bin` | 72-byte `RSIG` block + `ota_all.bin`; use this as the OTA payload |
| `ota_pubkey_area.bin` | 4 KB public key area, programmed once at manufacturing |

The public key lives in a dedicated `OTA_PUBKEY` flash region (`0x083BF000` on
Green2), not in the image.

### Provision the public key area

`ota_pubkey_area.bin` is written by the build. Flash it to the `OTA_PUBKEY`
region once per device, at manufacturing time - it is not part of an OTA update
and cannot be bootstrapped over OTA. Skipping this makes every signed update
fail with `No valid OTA public key in the OTA_PUBKEY region`.

## Executing the OTA

#### Terminal 1 (Linux ota-provider-app)

    cd connectedhomeip/ota-provider
    ./chip-ota-provider-app -f matter_ota_all.bin

#### Terminal 2 (chip-tool): 

Pair the device on NodeID=1, pair the ota-provider-app on NodeID=2.

    ./chip-tool pairing ble-wifi 1 <SSID> <PASSWORD> 20202021 3840
    ./chip-tool pairing onnetwork 2 20202021

Set the ota-provider to be the default-otaprovider of the device.

    ./chip-tool otasoftwareupdaterequestor write default-otaproviders '[{"fabricIndex": 1, "providerNodeID": 2, "endpoint": 0}]' 1 0

Configure the ACL of the ota-provider-app to allow access for device.

    ./chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 3, "authMode": 2, "subjects": null, "targets": [{"cluster": 41, "endpoint": null, "deviceType": null}]}]' 2 0

Announce the ota-provider-app to the device to start the OTA process.

    ./chip-tool otasoftwareupdaterequestor announce-otaprovider 2 0 0 0 1 0

## Expected Outcome

- The ota-provider-app will transfer the ota image by blocks of 1024 bytes.
- After completion, device will send an `ApplyUpdateRequest` to the ota-provider-app, who will send back an `ApplyUpdateResponse`.
- Upon receiving the `ApplyUpdateResponse`, the device will countdown 10 seconds before rebooting.
- If the OTA is successful, the device will reboot into the new image and will send a `NotifyUpdateApplied` to the ota-provider-app. 

## Common Mistakes

- VendorID and ProductID of the new OTA image does not match the ones in the Basic Information cluster, causing OTA to fail. Check your VendorID and ProductID using below `chip-tool` commands.

```
    ./chip-tool basicinformation read vendor-id 1 0
    ./chip-tool basicinformation read product-id 1 0
```

- Version of the new OTA image is not higher than the current image's version. OTA will only be allowed to execute if new image is newer than the current image.
