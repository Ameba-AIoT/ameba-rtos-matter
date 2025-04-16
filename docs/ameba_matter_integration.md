# Realtek Matter Solution Integration

Integrate the `ameba-rtos-matter` repository using the following steps:

## Getting `ameba-rtos-matter`

### Method 1: Clone the Repository

Navigate to `application/matter`:

1. Navigate to the directory `application/matter`.
2. Clone the repository `ameba-rtos-matter` from GitHub:

```bash
    cd sdk

    # for AmebaZ2, AmebaZ2plus, and AmebaD
    git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/common/application/matter
    cd component/common/application/matter
    # for AmebaDplus, AmebaLite, and AmebaSmart
    git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/application/matter
    cd component/application/matter

    git checkout release/v1.4
```

### Method 2: Add Repository as a Submodule

1. Navigate to the directory `sdk`.
2. Add `ameba-rtos-matter` as a submodule:

```bash
    cd sdk

    # for AmebaZ2, AmebaZ2plus, and AmebaD
    git submodule add https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/common/application/matter
    cd component/common/application/matter
    # for AmebaDplus, AmebaLite, and AmebaSmart
    git submodule add https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/application/matter
    cd component/application/matter

    git checkout release/v1.4
```

Ensure to checkout to the respective branch/tag.

## Creating the `third_party` Folder for Linking to `connectedhomeip`

1. Navigate to the directory `sdk`.
2. Create a directory named `third_party`.
3. Inside `third_party`, create a symbolic link named `connectedhomeip` pointing to `../connectedhomeip`.

```bash
    cd sdk
    mkdir third_party ; cd third_party
    ln -s ../../connectedhomeip connectedhomeip
```