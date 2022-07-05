# RAILtest ZG23 #

## Description ##

As described in [UG522: Mandatory Crystal Adjustments for EFR32ZG14/EFR32ZG23 Based Products User's Guide](https://www.silabs.com/documents/public/user-guides/ug522-mandatory-xtal-adjustments-for-zwave-products.pdf), the Silicon Labs EFR32ZG23 requires per-unit calibration of the HFXO (High Frequency Crytal Oscillator) CTUNE (crystal tuning) value. This calibration minimizes per-unit frequency variation so that the required accuracy for Z-Wave can be obtained with a standard crystal. Per [UG523: Bring-Up/Test HW Development User's Guide](https://www.silabs.com/documents/public/user-guides/ug523-bring-up_test-hw-development.pdf), the Silicon Labs [RAILtest utility[](https://www.silabs.com/documents/public/user-guides/ug409-railtest-users-guide.pdf) is to be used for testing and obtaining the CTUNE value. However, RAILtest as supplied by Silicon Labs is only capable of adjusting the volatile register value for CTUNE and is not capable of writing the non-volatile manufacturing token (MFG_CTUNE) which is required in order to change the value of CTUNE after power-up and/or reset. This modification implements a custom command in RAILtest that allows setting the ZG23 MFG_CTUNE token value so that the calibrated value is used on every power up / reset.

MFG_CTUNE
In the EFR32ZG23, the MFG_CTUNE token is stored in the USERDATA flash space (0x0FE00000) at offset 0x100. Refer to the memory map in table 6.1 in the [EFR32ZG23 Reference Manual](https://www.silabs.com/documents/public/reference-manuals/efr32xg23-rm.pdf). MFG_CTUNE is 16-bits, but per section xxxx in the [EFR32ZG23 Reference Manual](https://www.silabs.com/documents/public/reference-manuals/efr32xg23-rm.pdf), the HFXO CTUNE value for the EFR32ZG23 is an 8-bit value stored as little endian (i.e. the first byte in the two-byte MFG_CTUNE value).

Writing To Flash
For the EFR32ZG23 (just like other EFR32 devices), a flash write is only valid if the location being written to is in the blank/erased state (i.e. 0xff). However, EFR32ZG23 flash can only be erased by page, not by word/byte. Therefore, overwriting a previous value in flash requires first erasing the page. For the MFG_CTUNE value, this means the entire USERDATA flash page has to be erased in order for MFG_CTUNE to be written with a new value.

## Gecko SDK version ##

Tested with GSDK v4.1

## Hardware Required ##

- A WSTK board.

- A Bluetooth capable radio board, e.g: BRD4161A.

## Setup ##

1. Create a new RAILtest project targeting the device and/or hardware.

2. Replace the contents of "app.c" in the project with the app.c from this repo. This will implement the custom command required to write the MFG_CTUNE value.

3. (Optional) If you wish to make your RAILtest firmware compatible with the bootloader (so you can also flash a bootloader and upgrade to your application/SerialAPI image via bootloader upgrade), add the "Platform->Bootloader->Bootloader Application Interface" component to your RAILtest project. This makes the resulting executable compatible with a bootloader.

4. Build and flash to the target.

## Usage ##

This implements two commands, "getCtuneToken" and "setCtuneToken" (not case sensitive). "getCtuneToken" reads the existing MFG_CTUNE token value. Note that if the value is blank and not previously programmed, it will read "0xffff".
"setCtuneToken" writes the MFG_CTUNE value provided in the first argument, and the second argument determines whether the USERDATA page is erased (1) or not erased (0) prior to writing.

From the RAILtest help system:

```
> help getctunetoken
getCtuneToken       get Ctune Token (non-volatile) value
> help setctunetoken
setCtuneToken       set HFXO CTUNE non-volatile token with optional USERDATA page erase
                    [uint16] CTUNE token value
                    [uint8] 1 to erase USERDATA page first, 0 not to erase page
```

and an example successful MFG_CTUNE token write with USERDATA page erase:

```
> setctunetoken 0xa5 1
{{(setctunetoken)}{ctune msc write status:0}}
> getctunetoken
{{(getctunetoken)}{ctuneTokenVal:0x00a5}
```

**NOTE: Using the page erase risks losing any other data stored in the USERDATA page. This should only be used if other data is not already being stored elsewhere in the USERDATA page.**

## Reference Links ##
* [UG522: Mandatory Crystal Adjustments for EFR32ZG14/EFR32ZG23 Based Products User's Guide](https://www.silabs.com/documents/public/user-guides/ug522-mandatory-xtal-adjustments-for-zwave-products.pdf)
* [UG523: Bring-Up/Test HW Development User's Guide](https://www.silabs.com/documents/public/user-guides/ug523-bring-up_test-hw-development.pdf)
* [UG409: RAILtest User's Guide](https://www.silabs.com/documents/public/user-guides/ug409-railtest-users-guide.pdf)
* [EFR32ZG23 Reference Manual](https://www.silabs.com/documents/public/reference-manuals/efr32xg23-rm.pdf)
