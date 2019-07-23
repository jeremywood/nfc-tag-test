# NFC Tag Test

## Overview

Basic project to get NFC Tag Emulation working for the nRF52832 running the Apache MyNewt RTOS. It targets the nRF52 DK (Nordic PCA10040) BSP. Based on the Apache MyNewt Blinky example.

## Issues

This does not currently work, `nfc_t4t_lib` does not seem to start NFC functionality as expected. Not sure whether the issue is in supporting functions `nfc_platform_setup`, etc, or the overall configuration.

## Building

1. Download and install Apache Newt.
2. Download the Apache Mynewt Core package (executed from the blinky directory).

```no-highlight
    $ newt install
```

3. Build and load the bootloader and application

```no-highlight
    $ newt build nrf52_boot
    $ newt load nrf52_boot
    $ newt build nrf52_tag_test
    $ newt create-image nrf52_tag_test 1.0.0
    $ newt load nrf52_tag_test
```
