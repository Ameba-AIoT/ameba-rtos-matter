# Door Lock Example
This example is an implementation of the *Door Lock* device type. Peripherals consists of a button.
You can initialize any GPIO if necessary. Please check the different GPIO pin used for respective ICs below.
Note that these driver codes are meant to be just the skeleton, you should replace them and implement your own.

## ZAP
Since there is no example ZAP file for the door lock device type, we will use `door-lock-app.zap`.

## How it works
The door lock can be controlled in two ways, by the Matter controller, or by external means. 
In this example, we demonstrate both methods via Matter controller and external means.
If you wish to control by external means, you will need to use the `downlink` task shown in `matter_drivers.cpp`. Please feel free to add more based on your implementations. Meanwhile, controlling with Matter controller will trigger the `uplink` handler.

### Peripheral Initialization
The initializations are handled in `matter_drivers.cpp`.

### Matter Attribute Change Callback
Whenever the Matter controller changes the attribute of the Door Lock cluster, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Change the status/value before updating the attribute (TBD)
  2. MatterPostAttributeChangeCallback - Change the status/value after updating the attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

## How to build

### Configurations
`CONFIG_EXAMPLE_MATTER` and `CONFIG_EXAMPLE_MATTER_DOORLOCK` are automatically enabled in the Makefiles / CMake.

### Setup the Build Environment
  
    cd connectedhomeip
    source scripts/activate.sh

---

<details>
  <summary>Building with AmebaDplus</summary>

### AmebaDplus (RTL8721Dx)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| PWM        | PB_18 |
| Button     | PA_12 |

<details>
  <summary>Building with ameba-rtos_v1.1 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    python build.py -D MATTER_EXAMPLE=doorlock_port

#### Flash the Image
Refer to this [guide](../../docs/amebadplus_general_build.md#Flash-Image-using-Python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebadplus_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with ameba-rtos_v1.0 SDK</summary>

#### Build Matter Libraries

    cd ameba-rtos/amebadplus_gcc_project
    make -C project_km4/asdk doorlock_port

#### Build the Final Firmware

    cd ameba-rtos/amebadplus_gcc_project
    make all MATTER_EXAMPLE=doorlock

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/release/v1.0/README.md#flashing) to flash the image with Windows Image Tool.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebadplus_gcc_project
    make clean

</details>

</details>

---

<details>
  <summary>Building with AmebaLite</summary>

### AmebaLite (RTL8720EA / RTL8726EA)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| PWM        | PA_31 |
| Button     | PA_29 |

<details>
  <summary>Building with ameba-rtos_v1.1 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    python build.py -D MATTER_EXAMPLE=doorlock_port

#### Flash the Image
Refer to this [guide](../../docs/amebalite_general_build.md#Flash-Image-using-Python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebalite_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with ameba-rtos_v1.0 SDK</summary>

#### Build Matter Libraries

    cd ameba-rtos/amebalite_gcc_project
    make -C project_km4/asdk doorlock_port

#### Build the Final Firmware

    cd ameba-rtos/amebalite_gcc_project
    make all MATTER_EXAMPLE=doorlock

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/release/v1.0/README.md#flashing) to flash the image with Windows Image Tool.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebalite_gcc_project
    make clean

</details>

</details>

---

<details>
  <summary>Building with AmebaSmart</summary>

### AmebaSmart (RTL8730E)

#### GPIO Pin Configuration

| Peripheral | Pin   |
| ---------- | ----- |
| PWM        | PA_5  |
| Button     | PA_10 |

<details>
  <summary>Building with ameba-rtos_v1.1 SDK</summary>

#### Build Matter Libraries and the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    python build.py -D MATTER_EXAMPLE=doorlock_port

#### Flash the Image
Refer to this [guide](../../docs/amebasmart_general_build.md#Flash-Image-using-Python-script) to flash the image with python script.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project
    cd build/ && ninja clean_matter_libs clean && cd .. && rm -rf build/

</details>

<details>
  <summary>Building with ameba-rtos_v1.0 SDK</summary>

#### Build Matter Libraries

    cd ameba-rtos/amebasmart_gcc_project
    make -C project_ap/asdk doorlock_port

#### Build the Final Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make all MATTER_EXAMPLE=doorlock

#### Flash the Image
Refer to this [guide](https://github.com/Ameba-AIoT/ameba-rtos/blob/release/v1.0/README.md#flashing) to flash the image with Windows Image Tool.

#### Clean Matter Libraries and Firmware

    cd ameba-rtos/amebasmart_gcc_project
    make clean

</details>

</details>

---

## Running example using chip-tool

### Operations without PIN code

To disable pincode for remote operation run the following command:

    ./chip-tool doorlock write require-pinfor-remote-operation 0 <node-id> <endpoint-id>

Check if the config is disabled:

    ./chip-tool doorlock read require-pinfor-remote-operation <node-id> <endpoint-id>

To check lock state of a door, run the following command:

    ./chip-tool doorlock read lock-state <node-id> <endpoint-id>

To unlock the door without pincode, run the following command:

    ./chip-tool doorlock unlock-door <node-id> <endpoint-id> --timedInteractionTimeoutMs 1000

To lock the door without pincode, run the following command:

    ./chip-tool doorlock lock-door <node-id> <endpoint-id> --timedInteractionTimeoutMs 1000

To enable pincode for remote operation run the following command:

    ./chip-tool doorlock write require-pinfor-remote-operation 1 <node-id> <endpoint-id>

### Operations with PIN code

---

<details>
  <summary>Arguments explanation</summary>

- OperationType:
    - 0: Add
    - 1: Clear
    - 2: Modify
- UserIndex: Index of the user added, starts from 1
- UserName: Name fo the User
- UserUniqueID: Unique ID of the user
- UserStatus:
    - 0: Available
    - 1: OccupiedEnabled
    - 3: OccupiedDisabled
- UserType:
    - 0: UnrestrictedUser
    - 1: YearDayScheduleUser
    - 2: WeekDayScheduleUser
    - 3: ProgrammingUser
    - 4: NonAccessUser
    - 5: ForcedUser
    - 6: DisposableUser
    - 7: ExpiringUser
    - 8: ScheduleRestrictedUser
    - 9: RemoteOnlyUser
- Credential: json format: '{ "credentialType" : X , "credentialIndex" : X }'
    - CredentialType:
        - 0: ProgrammingPIN
        - 1: Pin
        - 2: Rfid
        - 3: Fingerprint
        - 4: FingerVein
        - 5: Face
        - 6: AliroCredentialIssuerKey
        - 7: AliroEvictableEndpointKey
        - 8: AliroNonEvictableEndpointKey
    - CredentialIndex: Index of Credential, starts from 1
- CredentialRule:
    - 0: Single
    - 1: Dual
    - 2: Tri
</details>

---

#### Adding user and PIN code

To add user to the door lock database, run the following command:

    ./chip-tool doorlock set-user <OperationType> <UserIndex> <UserName> <UserUniqueID> <UserStatus> <UserType> <CredentialRule> <node-id> <endpoint-id>
    # e.g.:
    ./chip-tool doorlock set-user 0 2 user2 6767 1 0 0 1 1  --timedInteractionTimeoutMs 1000

Note that UserIndex is set to 2. This is because in Ameba implementation, first user (UserIndex=1) is initialized.

To add credential to the to the user that is added in the previous step, run the following command:

    ./chip-tool doorlock set-credential <OperationType> <Credential> <CredentialData> <UserIndex> <UserStatus> <UserType> <node-id> <endpoint-id>
    # e.g.:
    ./chip-tool doorlock set-credential 0 '{ "credentialType" : 1 , "credentialIndex" : 2 }' 123123 2 null null 1 1 --timedInteractionTimeoutMs 1000

Note that credentialIndex is set to 2. This is because in Ameba implementation, first user is initialized with the Pin code credential (credentialType=1).

#### Locking and Unlocking door with PIN code

To unlock/lock door with PinCode, run the following code:

    ./chip-tool doorlock <unlock-door/lock-door> <node-id> <endpoint-id> --timedInteractionTimeoutMs 1000 --PINCode <PINCcode>
    # e.g.:
    ./chip-tool doorlock unlock-door 1 1 --timedInteractionTimeoutMs 1000 --PINCode 123123
    ./chip-tool doorlock lock-door 1 1 --timedInteractionTimeoutMs 1000 --PINCode 123123

#### Removing user and PIN code

To remove a credential from a user, run the following command:

    ./chip-tool doorlock clear-credential <Credential> <node-id> <endpoint-id>
    # e.g.:
    ./chip-tool doorlock clear-credential '{ "credentialType" : 1 , "credentialIndex" : 2 }' 1 1 --timedInteractionTimeoutMs 1000

To remove a user, run the following command:

    ./chip-tool doorlock clear-user <UserIndex> <node-id> <endpoint-id>
    # e.g.:
    ./chip-tool doorlock clear-user 2 1 1 --timedInteractionTimeoutMs 1000
