# Realtek Matter Solution Integration

Integrate the `ameba-rtos-matter` repository using the following steps:

## Getting `ameba-rtos-matter`

### Method 1: Clone the Repository

Navigate to `sdk/component/common/application/matter`:

1. Navigate to the directory `sdk/component/common/application/matter`.
2. Clone the repository `ameba-rtos-matter` from GitHub:

```bash
    cd sdk
    git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/common/application/matter
```

### Method 2: Add Repository as a Submodule

1. Navigate to the directory `sdk`.
2. Add `ameba-rtos-matter` as a submodule:

```bash
    cd sdk
    git submodule add https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/common/application/matter
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
