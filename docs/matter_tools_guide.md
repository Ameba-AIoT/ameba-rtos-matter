# Matter Tools

Introduction to several tools that are frequently used for Matter Development.

## Chip Tool

Chip-Tool is a simulator of Matter Commissioner, designed to send messages to a Matter Commissionee. The purpose of the Matter Commissioner is to commission the Matter Commissionee into the Matter fabric, and control it for reading or writing commands.

To generate Chip-Tool, use the following command:

    cd connectedhomeip
    source scripts/activate.sh
    scripts/examples/gn_build_example.sh examples/chip-tool out/chip-tool

Afterwards you should be able to find chip-tool in `connectedhomeip/out/chip-tool` directory

## Chip Cert and spake2p

CHIP Certificate Tool (chip-cert) is used to generate, convert, validate, and resign Matter certificates. The spake2p tool is used for generating spake parameters for device manufacturing provisioning.

Build chip-cert tool:

    cd connectedhomeip
    source scripts/activate.sh
    cd src/tools/chip-cert
    gn gen out
    ninja -C out

Build spake2p tool:

    cd connectedhomeip
    source scripts/activate.sh
    cd src/tools/spake2p
    gn gen out
    ninja -C out

## Zap Tool

ZAP contains the Zigbee Cluster Library (ZCL) developed by Connectivity Standards Alliance (CSA). The Zap Tool generates a user interface to modify these libraries to support different types of devices (e.g., clusters and attributes).

To generate the Zap-Tool, use the following command:

    cd connectedhomeip
    source scripts/activate.sh
    scripts/tools/zap/run_zaptools.sh <PATH_TO_ZAP_FILE>

Where:
 - You can build a fresh zap file by running the zap tool without including <PATH_TO_ZAP_FILE>.
 - Alternatively, you can use any existing zap files and modify it for your product.
 - For instance, if you are supporting lighting application, the <PATH_TO_ZAP_FILE> you can use is `connectedhomeip/examples/lighting-
app/lighting-common/lighting-app.zap`.
