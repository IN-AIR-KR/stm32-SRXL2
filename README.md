# stm32-SRXL2

This repository provides a C-based implementation of the Spektrum SRXL2 protocol for STM32 microcontrollers, leveraging the LL (Low-Layer) drivers.
It allows your STM32 projects to interface with Spektrum receivers and sensors using high-speed, single-wire UART communication.

## Hardware Setup

To use SRXL2 with STM32, you must connect the signal wire to a UART TX pin.
- **MCU**
  - Tested hardware : `STM32F407VETx`
- **UART Configuration:**
  - **Baudrate:** 115,200 bps
  - **Mode:** Half-duplex (Single-wire)
  - **Logic Level:** 3.3V

## Reference
- [[Github] SRXL2 Specification.pdf](https://github.com/SpektrumRC/SRXL2/tree/master/Docs)
