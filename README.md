# STM32F401 UART Bootloader

A custom serial bootloader for the **STM32F401RC** (ARM Cortex-M4), written
from scratch on bare-metal CMSIS with a layered MCAL/HAL driver stack. It lets
you flash a new application over UART without an SWD/JTAG programmer, using the
included Python host client to parse an ELF and stream it to the target.

## Overview

The bootloader lives at the bottom of flash. On reset it checks a marker word
in flash:

- **Marker present** → jump straight to the user application.
- **Marker absent** → stay in bootloader mode and wait for commands over UART1.

The host sends framed commands (write / erase / read / get-version / done). When
the host finishes, it sends `DONE`; the bootloader writes the marker, performs a
software system reset, and on the next boot jumps to the freshly programmed app.

## Repository layout

```
bootloader/
├── bootloader.py            # Python host client (ELF parser + serial protocol)
├── firmware.elf             # Prebuilt bootloader image
├── platformio.ini           # PlatformIO build config (board, framework, flags)
├── include/
│   ├── bootloader.h         # Bootloader API, command/state enums
│   ├── std_types.h, macros.h
│   ├── mcal/                # Register-level drivers (rcc, gpio, uart, spi,
│   │   │                    #   dma, nvic, flashdriver)
│   └── hal/                 # led, hserial (high-level serial wrapper)
├── src/
│   ├── main.c               # Reset entry: marker check → jump or init
│   ├── bootloader.c         # Protocol state machine & command handlers
│   ├── mcal/                # MCAL driver implementations
│   └── hal/                 # HAL driver implementations
└── lib/  test/  .vscode/
```

## Architecture

The firmware is organized in layers:

| Layer | Modules | Responsibility |
|-------|---------|----------------|
| Application | `main.c`, `bootloader.c` | Boot decision, protocol state machine |
| HAL | `hserial`, `led` | Interrupt-driven serial interface, board I/O |
| MCAL | `rcc`, `gpio`, `uart`, `spi`, `dma`, `nvic`, `flashdriver` | Direct register access |

UART reception is fully interrupt-driven: `rx_callback_function()` in
[src/bootloader.c](src/bootloader.c) implements a byte-by-byte state machine
(`IDLE → SOF → CMD → LENGTH → ADDRESS → DATA → execute`), re-arming the receive
buffer for the exact number of bytes expected at each step.

## Serial protocol

- **UART1**, default **115200** baud, 8N1.
- Every frame begins with a Start-Of-Frame byte and is acknowledged byte/stage.

| Symbol | Value | Meaning |
|--------|-------|---------|
| `SOF`  | `0x55` | Start of frame |
| `ACK`  | `0xAA` | Acknowledge |
| `NACK` | `0xFF` | Negative acknowledge |

| Command | Code | Description |
|---------|------|-------------|
| `READ_MEMORY`  | `0x00` | Read flash and stream bytes back |
| `WRITE_MEMORY` | `0x01` | Program flash at address |
| `ERASE_MEMORY` | `0x02` | Erase a flash sector |
| `GET_VERSION`  | `0x03` | Return bootloader version byte (`0x02`) |
| `DONE`         | `0x04` | Write marker, reset, jump to app |

A `WRITE_MEMORY` frame is: `SOF, CMD, LENGTH, ADDR[4] (big-endian), DATA[LENGTH]`,
with the target ACKing after each stage and a final ACK once the flash write
completes.

### Memory map

| Region | Address |
|--------|---------|
| Bootloader | `0x08000000` |
| Valid app marker | `0x0800C000` (sector 2), value `0x0000DEAD` |
| Application vector table | `0x08020000` |
| Writable address range | `0x08001000` – `0x0803FFFF` |

> Note: the host client and target use a partly different command numbering for
> `DONE` (host `0x04` vs. the header's `0x05`); the values that actually matter
> are the ones defined in [src/bootloader.c](src/bootloader.c) and mirrored in
> [bootloader.py](bootloader.py).

## Building the bootloader

Built with [PlatformIO](https://platformio.org/) targeting the CMSIS framework.

```bash
# Build
pio run

# Flash the bootloader itself via ST-Link
pio run --target upload
```

Board/toolchain settings are in [platformio.ini](platformio.ini): board
`genericSTM32F401RC`, framework `cmsis`, ST-Link upload/debug, debug build at
`-O0`.

## Flashing an application (host client)

The Python client parses the application ELF, extracts loadable (`PT_LOAD`)
segments, and programs them in 128-byte aligned chunks.

### Requirements

```bash
pip install pyserial
```

### Usage

```bash
python3 bootloader.py <app.elf> [port] [baudrate]

# Example
python3 bootloader.py app.elf /dev/ttyUSB0 115200
```

Defaults: port `/dev/ttyUSB0`, baudrate `115200`.

The client will:
1. Open the serial port and query the bootloader version.
2. Program each loadable section in 128-byte chunks (with progress logging).
3. Send `DONE`, after which the MCU resets and runs the new application.

> Ensure the target is in bootloader mode (no valid marker) before flashing —
> e.g. after a fresh bootloader flash or a marker erase.

## License

Educational project (ITI Embedded Systems track). Use at your own risk.
