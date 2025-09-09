# Realtek Matter Solution Integration

Integrate the `ameba-rtos-matter` repository using the following steps:

## Getting `ameba-rtos-matter`

1. Navigate to the directory `ameba-rtos`.
2. Execute matter_setup.sh to setup the SDK

```bash
    cd ameba-rtos

    chmod u+x matter_setup.sh

    ./matter_setup.sh ameba-rtos v1.5
```

## Creating the `third_party` Folder for Linking to `connectedhomeip`

1. Navigate to the directory `sdk`.
2. Create a directory named `third_party`.
3. Inside `third_party`, create a symbolic link named `connectedhomeip` pointing to `../connectedhomeip`.

```bash
    cd sdk
    mkdir third_party ; cd third_party
    ln -s ../../connectedhomeip connectedhomeip
```