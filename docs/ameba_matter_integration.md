# Realtek Matter Solution Integration

Integrate the `ameba-rtos-matter` repository using the following steps:

## SDKs Integration with the help of setup script

<details>
  <summary>  Ameba RTOS v1.2 SDK </summary>

1. Check out ameba-rtos-matter repository on the same directory level of ameba-rtos and connectedhomeip

2. Execute matter_setup.sh to setup the SDK

```bash
    git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git -b ameba-rtos/release/v1.5

    cd ameba-rtos-matter

    chmod u+x matter_setup.sh

    ./matter_setup.sh ../ameba-rtos
```

</details>

<details>
  <summary>  Ameba RTOS v1.1 SDK / Ameba RTOS v1.0 SDK </summary>

1. Navigate to the directory `ameba-rtos`.
2. Execute matter_setup.sh to setup the SDK

```bash
    cd ameba-rtos

    chmod u+x matter_setup.sh

    ./matter_setup.sh ameba-rtos v1.5
```

</details>

## SDKs Integration without the help of setup script

In the following sections, there is no guide for Ameba RTOS v1.2 SDK. Please refer to the [previous section](#sdks-integration-with-the-help-of-setup-script).

### Getting `ameba-rtos-matter`

<details>
  <summary>  Ameba RTOS v1.1 SDK / Ameba RTOS v1.0 SDK </summary>

#### Method 1: Clone the Repository

Navigate to `application/matter`:

1. Navigate to the directory `application/matter`.
2. Clone the repository `ameba-rtos-matter` from GitHub:

```bash
    cd ameba-rtos

    git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/application/matter

    cd component/application/matter

    git checkout ameba-rtos/release/v1.5
```

#### Method 2: Add Repository as a Submodule

1. Navigate to the directory `ameba-rtos`.
2. Add `ameba-rtos-matter` as a submodule:

```bash
    cd ameba-rtos

    git submodule add https://github.com/Ameba-AIoT/ameba-rtos-matter.git component/application/matter

    cd component/application/matter

    git checkout ameba-rtos/release/v1.5
```

Ensure to checkout to the respective branch/tag.

</details>

### Creating the `third_party` Folder for Linking to `connectedhomeip`

<details>
  <summary>  Ameba RTOS v1.1 SDK / Ameba RTOS v1.0 SDK </summary>

1. Navigate to the directory `ameba-rtos`.
2. Create a directory named `third_party`.
3. Inside `third_party`, create a symbolic link named `connectedhomeip` pointing to `../connectedhomeip`.

```bash
    cd ameba-rtos
    mkdir third_party ; cd third_party
    ln -s ../../connectedhomeip connectedhomeip
```

</details>
