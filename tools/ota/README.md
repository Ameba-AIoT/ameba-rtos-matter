# Matter OTA Guide

Follow this guide to carry out Matter standard Over the Air Software Update

## Prerequisites

Build Linux ota-provider-app

    cd connectedhomeip
    source scripts/activate.sh
    ./scripts/examples/gn_build_example.sh examples/ota-provider-app/linux/ ota-provider

## Generating the OTA Image

When building the OTA firmware, ensure that `CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION` and `CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING` is higher than that of the current image.
You can check the current image's software version using `chip-tool` commands below.

    ./chip-tool basicinformation read software-version 1 0
    ./chip-tool basicinformation read software-version-string 1 0

<details>
  <summary>For AmebaDplus OTA Image</summary>

Ensure that the OTA address is set correctly according to the device's flash size in the following files:

    component/soc/usrcfg/amebadplus/ameba_flashcfg.c

The file can be found in the base SDK.

### Example

In `component/soc/usrcfg/amebadplus/ameba_flashcfg.c`
```c
const FlashLayoutInfo_TypeDef Flash_Layout[] = {
	/* Region_Type, [StartAddr, EndAddr] */
	{IMG_BOOT,      0x08000000, 0x08013FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K)
	//Users should modify below according to their own memory
	{IMG_APP_OTA1,  0x08014000, 0x081FFFFF}, //Certificate(4K) + Manifest(4K) + KM4 Application OTA1 + RDP IMG OTA1

	{IMG_BOOT_OTA2, 0x08200000, 0x08213FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K) OTA
	{IMG_APP_OTA2,  0x08214000, 0x083DCFFF}, //Certificate(4K) + Manifest(4K) + KM4 Application OTA2 + RDP IMG OTA2

	{FTL,           0x083DD000, 0x083DFFFF}, //FTL for BT(>=12K), The start offset of flash pages which is allocated to FTL physical map.
	{VFS1,          0x083E0000, 0x083FFFFF}, //VFS region 1 (128K)
	{VFS2,          0xFFFFFFFF, 0xFFFFFFFF}, //VFS region 2
	{USER,          0xFFFFFFFF, 0xFFFFFFFF}, //reserve for user

	/* End */
	{0xFF,          0xFFFFFFFF, 0xFFFFFFFF},
};
```

Once you've set the addresses correctly for `IMG_APP_OTA1` and `IMG_APP_OTA2`, ensure that you rebuild both the firmware correctly.

### Generating Matter OTA Image

Use the ota_image_tool.py to generate the Matter OTA image, embedding the Matter OTA header:

    python3 ota_image_tool.py create -v <VENDORID> -p <PRODUCTID> -vn <VERSION> -vs <VERSIONSTRING> -da <DIGESTALGO> <path to km0_km4_app.bin> <output ota image>

Example:

    python3 ota_image_tool.py create -v 0xFFF1 -p 0x8001 -vn 2 -vs 2.0 -da sha256 ../../../../../amebadplus_gcc_project/km0_km4_app.bin ota_image.bin

</details>

<details>
  <summary>For AmebaLite OTA Image</summary>

Ensure that the OTA address is set correctly according to the device's flash size in the following files:

    component/soc/usrcfg/amebalite/ameba_flashcfg.c

The file can be found in the base SDK.

### Example

In `component/soc/usrcfg/amebadplus/ameba_flashcfg.c`
```c
FlashLayoutInfo_TypeDef Flash_Layout[] = {
	/* Region_Type, [StartAddr, EndAddr] */
	{IMG_BOOT,      0x08000000, 0x08013FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K)
	//Users should modify below according to their own memory
	{IMG_APP_OTA1,  0x08014000, 0x081FFFFF}, //Certificate(4K) + Manifest(4K) + KR4 & KM4 Application OTA1 + RDP IMG OTA1

	{IMG_BOOT_OTA2, 0x08200000, 0x08213FFF}, //Boot Manifest(4K) + KM4 Bootloader(76K) OTA
	{IMG_APP_OTA2,  0x08214000, 0x083DCFFF}, //Certificate(4K) + Manifest(4K) + KR4 & KM4 Application OTA2 + RDP IMG OTA2
	{FTL,           0x083DD000, 0x083DFFFF}, //FTL for BT(>=12K), The start offset of flash pages which is allocated to FTL physical map.
	{VFS1,          0x083E0000, 0x083FFFFF}, //VFS region 1 (128K)
	{IMG_DSP,       0x08400000, 0x086FFFFF}, //Manifest(4K) + DSP IMG, only one DSP region in layout
	{VFS2,          0xFFFFFFFF, 0xFFFFFFFF}, //VFS region 2
	{USER,          0xFFFFFFFF, 0xFFFFFFFF}, //reserve for user

	/* End */
	{0xFF,          0xFFFFFFFF, 0xFFFFFFFF},
};
```

Once you've set the addresses correctly for `IMG_APP_OTA1` and `IMG_APP_OTA2`, ensure that you rebuild both the firmware correctly.

### Generating Matter OTA Image

Use the ota_image_tool.py to generate the Matter OTA image, embedding the Matter OTA header:

    python3 ota_image_tool.py create -v <VENDORID> -p <PRODUCTID> -vn <VERSION> -vs <VERSIONSTRING> -da <DIGESTALGO> <path to kr4_km4_app.bin> <output ota image>

Example:

    python3 ota_image_tool.py create -v 0xFFF1 -p 0x8001 -vn 2 -vs 2.0 -da sha256 ../../../../../amebalite_gcc_project/kr4_km4_app.bin ota_image.bin

</details>

<details>
  <summary>For AmebaSmart OTA Image</summary>

Ensure that the OTA address is set correctly according to the device's flash size in the following files:

    component/soc/usrcfg/amebasmart/ameba_flashcfg.c

The file can be found in the base SDK.

### Example

In `component/soc/usrcfg/amebasmart/ameba_flashcfg.c`
```c
FlashLayoutInfo_TypeDef Flash_Layout[] = {
	/* Region_Type, [StartAddr, EndAddr] */
	{IMG_BOOT,      0x08000000, 0x0801FFFF}, //Boot Manifest(4K) + KM4 Bootloader(124K)
	//Users should modify below according to their own memory
	{IMG_APP_OTA1,  0x08020000, 0x082FFFFF}, //Certificate(4K) + Manifest(4K) + KM0 & KM4 & CA32 Application OTA1 + RDP IMG OTA1
	// + AP IMG OTA1
	{IMG_BOOT_OTA2, 0x08300000, 0x0833FFFF}, //Boot Manifest(4K) + KM4 Bootloader(252K) OTA
	{IMG_APP_OTA2,  0x08340000, 0x0861FFFF}, //Certificate(4K) + Manifest(4K) + KM0 & KM4 & CA32 Application OTA2 + RDP IMG OTA2
	// + AP IMG OTA2
	{FTL,           0x08620000, 0x08622FFF}, //FTL for BT(>=12K), The start offset of flash pages which is allocated to FTL physical map.
	{VFS1,          0x08623000, 0x08642FFF}, //VFS region 1 (128K)
	{VFS2,          0xFFFFFFFF, 0xFFFFFFFF}, //VFS region 2
	{USER,          0xFFFFFFFF, 0xFFFFFFFF}, //reserve for user
	/* End */
	{0xFF,          0xFFFFFFFF, 0xFFFFFFFF},
};
```

Once you've set the addresses correctly for `IMG_APP_OTA1` and `IMG_APP_OTA2`, ensure that you rebuild both the firmware correctly.

### Generating Matter OTA Image

Use the ota_image_tool.py to generate the Matter OTA image, embedding the Matter OTA header:

    python3 ota_image_tool.py create -v <VENDORID> -p <PRODUCTID> -vn <VERSION> -vs <VERSIONSTRING> -da <DIGESTALGO> <path to km0_km4_ca32_app.bin> <output ota image>

Example:

    python3 ota_image_tool.py create -v 0xFFF1 -p 0x8001 -vn 2 -vs 2.0 -da sha256 ../../../../../amebasmart_gcc_project/km0_km4_ca32_app.bin ota_image.bin

</details>

Ensure that the `VERSION` and `VERSIONSTRING` matches your `CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION` and `CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING` respectively.

To check your OTA image

    python3 ota_image_tool.py show <ota image>

Once you have ensured that the OTA image is correct, copy the output OTA image to the directory of the ota-provider built from the previous step.

    cp ota_image.bin <path to the ota-provider directory>

## Executing the OTA

#### Terminal 1 (Linux ota-provider-app)

    cd connectedhomeip/ota-provider
    ./chip-ota-provider-app -f ota_image.bin

#### Terminal 2 (chip-tool): 

Pair the device on NodeID=1, pair the ota-provider-app on NodeID=2.

    ./chip-tool ble-wifi 1 <SSID> <PASSWORD> 20202021 3840
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
