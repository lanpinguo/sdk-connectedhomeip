# Matter Telink Lighting Example Application

The Telink Lighting Example demonstrates how to remotely control a white
dimmable light bulb. It uses buttons to test changing the lighting and device
states and LEDs to show the state of these changes. You can use this example as
a reference for creating your own application.

![Telink B91 EVK](http://wiki.telink-semi.cn/wiki/assets/Hardware/B91_Generic_Starter_Kit_Hardware_Guide/connection_chart.png)

## Build and flash

1. Pull docker image from repository:

    ```bash
    $ docker pull connectedhomeip/chip-build-telink:latest
    ```

1. Run docker container:

    ```bash
    $ docker run -it --rm -v ${CHIP_BASE}:/root/chip -v /dev/bus/usb:/dev/bus/usb --device-cgroup-rule "c 189:* rmw" connectedhomeip/chip-build-telink:latest
    ```

    here `${CHIP_BASE}` is directory which contains CHIP repo files **!!!Pay
    attention that OUTPUT_DIR should contains ABSOLUTE path to output dir**

1. Activate the build environment:

    ```bash
    $ source ./scripts/activate.sh
    ```

1. In the example dir run:

    ```bash
    $ west build
    ```

1. Flash binary:

    ```
    $ west flash --erase
    ```

## Usage

### UART

To get output from device, connect UART to following pins:

| Name | Pin                           |
| :--: | :---------------------------- |
|  RX  | PB3 (pin 17 of J34 connector) |
|  TX  | PB2 (pin 16 of J34 connector) |
| GND  | GND                           |

### Buttons

The following buttons are available on **tlsr9518adk80d** board:

| Name     | Function               | Description                                                                                            |
| :------- | :--------------------- | :----------------------------------------------------------------------------------------------------- |
| Button 1 | Factory reset          | Perform factory reset to forget currently commissioned Thread network and back to uncommissioned state |
| Button 2 | Lighting control       | Manually triggers the lighting state                                                                   |
| Button 3 | Thread start           | Commission thread with static credentials and enables the Thread on device                             |
| Button 4 | Open commission window | The button is opening commissioning window to perform commissioning over BLE                           |

### LEDs

#### Indicate current state of Thread network

**Red** LED indicates current state of Thread network. It is able to be in
following states:

| State                       | Description                                                                  |
| :-------------------------- | :--------------------------------------------------------------------------- |
| Blinks with short pulses    | Device is not commissioned to Thread, Thread is disabled                     |
| Blinks with frequent pulses | Device is commissioned, Thread enabled. Device trying to JOIN thread network |
| Blinks with wide pulses     | Device commissioned and joined to thread network as CHILD                    |

#### Indicate identify of device

**Green** LED used to identify the device. The LED starts blinking when the
Identify command of the Identify cluster is received. The command's argument can
be used to specify the the effect. It is able to be in following effects:

| Effect                          | Description                                                          |
| :------------------------------ | :------------------------------------------------------------------- |
| Blinks (200 ms on/200 ms off)   | Blink (EMBER_ZCL_IDENTIFY_EFFECT_IDENTIFIER_BLINK)                   |
| Breathe (during 1000 ms)        | Breathe (EMBER_ZCL_IDENTIFY_EFFECT_IDENTIFIER_BREATHE)               |
| Blinks (50 ms on/950 ms off)    | Okay (EMBER_ZCL_IDENTIFY_EFFECT_IDENTIFIER_OKAY)                     |
| Blinks (1000 ms on/1000 ms off) | Channel Change (EMBER_ZCL_IDENTIFY_EFFECT_IDENTIFIER_CHANNEL_CHANGE) |
| Blinks (950 ms on/50 ms off)    | Finish (EMBER_ZCL_IDENTIFY_EFFECT_IDENTIFIER_FINISH_EFFECT)          |
| LED off                         | Stop (EMBER_ZCL_IDENTIFY_EFFECT_IDENTIFIER_STOP_EFFECT)              |

#### Indicate current state of lightbulb

By default, only **Blue** LED is used to show current state of lightbulb (only
for lightning-app).

To enable RGB functionality in Your application set this config:

In Matter examples/lighting-app/telink/include/**AppConfig.h**, set the define
`USE_RGB_PWM`:

```bash
    define USE_RGB_PWM 1
```

To get current state of lightbulb in RGB mode, connect 3-color LED module to
following pins:

| Name  |         Pin         |
| :---: | :-----------------: |
|  Red  | PE2 (pin 8 of J34)  |
| Green | PE0 (pin 5 of J34)  |
| Blue  | PB4 (pin 20 of J34) |
|  GND  | GND (pin 24 of J50) |

### CHIP tool commands

1. Build
   [chip-tool cli](https://github.com/project-chip/connectedhomeip/blob/master/examples/chip-tool/README.md)

2. Pair with device

    ```
    ${CHIP_TOOL_DIR}/chip-tool pairing ble-thread ${NODE_ID} hex:${DATASET} ${PIN_CODE} ${DISCRIMINATOR}
    ```

    Example:

    ```
    ./chip-tool pairing ble-thread 1234 hex:0e080000000000010000000300000f35060004001fffe0020811111111222222220708fd61f77bd3df233e051000112233445566778899aabbccddeeff030e4f70656e54687265616444656d6f010212340410445f2b5ca6f2a93a55ce570a70efeecb0c0402a0fff8 20202021 3840
    ```

3. Switch on the light:

    ```
    ${CHIP_TOOL_DIR}/chip-tool onoff on 1
    ```

    here:

    - **onoff** is name of cluster
    - **on** command to the cluster
    - **1** ID of endpoint

4. Switch off the light:

    ```
    ${CHIP_TOOL_DIR}/chip-tool onoff off 1
    ```

    here:

    - **onoff** is name of cluster
    - **off** command to the cluster
    - **1** ID of endpoint

5. Read the light state:

    ```
    ${CHIP_TOOL_DIR}/chip-tool onoff read on-off 1
    ```

    here:

    - **onoff** is name of cluster
    - **read** command to the cluster
    - **on-off** attribute to read
    - **1** ID of endpoint

6. Change brightness of light:

    ```
    ${CHIP_TOOL_DIR}/chip-tool levelcontrol move-to-level 32 0 0 0 1
    ```

    here:

    - **levelcontrol** is name of cluster
    - **move-to-level** command to the cluster
    - **32** brightness value
    - **0** transition time
    - **0** option mask
    - **0** option override
    - **1** ID of endpoint

7. Read brightness level:
    ```
    ./chip-tool levelcontrol read current-level 1
    ```
    here:
    - **levelcontrol** is name of cluster
    - **read** command to the cluster
    - **current-level** attribute to read
    - **1** ID of endpoint

### OTA with Linux OTA Provider

OTA feature enabled by default only for ota-requestor-app example. To enable OTA
feature for another Telink example:

-   set CONFIG_CHIP_OTA_REQUESTOR=y in corresponding "prj.conf" configuration
    file.

After build application with enabled OTA feature, use next binary files:

-   zephyr.bin - main binary to flash PCB (Use 2MB PCB).
-   zephyr-ota.bin - binary for OTA Provider

All binaries has the same SW version. To test OTA “zephyr-ota.bin” should have
higher SW version than base SW. Set CONFIG_CHIP_DEVICE_SOFTWARE_VERSION=2 in
corresponding “prj.conf” conﬁguration file.

Usage of OTA:

-   Build the [Linux OTA Provider](../../ota-provider-app/linux)

    ```
    ./scripts/examples/gn_build_example.sh examples/ota-provider-app/linux out/ota-provider-app chip_config_network_layer_ble=false
    ```

-   Run the Linux OTA Provider with OTA image.

    ```
    ./chip-ota-provider-app -f zephyr-ota.bin
    ```

-   Provision the Linux OTA Provider using chip-tool

    ```
    ./chip-tool pairing onnetwork ${OTA_PROVIDER_NODE_ID} 20202021
    ```

    here:

    -   \${OTA_PROVIDER_NODE_ID} is the node id of Linux OTA Provider

-   Configure the ACL of the ota-provider-app to allow access

    ```
    ./chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [112233], "targets": null}, {"fabricIndex": 1, "privilege": 3, "authMode": 2, "subjects": null, "targets": null}]' ${OTA_PROVIDER_NODE_ID} 0
    ```

    here:

    -   \${OTA_PROVIDER_NODE_ID} is the node id of Linux OTA Provider

-   Use the chip-tool to announce the ota-provider-app to start the OTA process

    ```
    ./chip-tool otasoftwareupdaterequestor announce-otaprovider ${OTA_PROVIDER_NODE_ID} 0 0 0 ${DEVICE_NODE_ID} 0
    ```

    here:

    -   \${OTA_PROVIDER_NODE_ID} is the node id of Linux OTA Provider
    -   \${DEVICE_NODE_ID} is the node id of paired device

Once the transfer is complete, OTA requestor sends ApplyUpdateRequest command to
OTA provider for applying the image. Device will restart on successful
application of OTA image.

### Building with Pigweed RPCs

The RPCs in `lighting-common/lighting_service/lighting_service.proto` can be
used to control various functionalities of the lighting app from a USB-connected
host computer. To build the example with the RPC server, run the following
command with _build-target_ replaced with the build target name of the Telink
Semiconductor's kit you own:

    ```
    $ west build -b tlsr9518adk80d -- -DOVERLAY_CONFIG=rpc.overlay
    ```
