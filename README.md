# PHMonitor Firmware (nRF52810)

This repository contains a minimal data‑logging firmware for the Nordic nRF52810
SoC.  The firmware periodically powers an external analog front end, samples an
analog sensor, stores the readings in external FRAM and exposes the log over
Bluetooth Low Energy using a custom service.

## Features

- **Ultra low power** operation.  The analog circuit and high frequency clocks
  are only enabled while taking a measurement.
- **External FRAM logging.**  Samples are written sequentially to FRAM with a
  pointer that wraps when the memory end is reached.
- **BLE Data‑Log Service.**  A simple service allows a connected peer to read the
  number of stored samples, dump the log and erase it.

## File Overview

| File          | Purpose                                                    |
|---------------|------------------------------------------------------------|
| `main.c`      | Application entry point. Configures modules, starts timers |
| `afe_ctrl.c`  | Controls GPIO rails for the analog front end and performs SAADC sampling |
| `mem_fram.c`  | I²C helper for communicating with the external FRAM        |
| `data_log.c`  | Maintains the log pointer and sample count in FRAM         |
| `ble_dls.c`   | Implements the custom BLE Data‑Log Service                 |
| `dls_ctrl.c`  | High level BLE command handling for dump/erase operations  |

The `include/` directory contains the corresponding headers.  `sdk_config.h`
provides the minimal SoftDevice and driver configuration used by this example.

## How It Works

1. **Startup:** all peripherals are initialized and a periodic timer is started
   (`PERIOD_MS`).
2. **Sampling:** every period the analog front end is powered on, a short delay
   allows it to settle and a sample is taken.  The SAADC value is converted to
   millivolts and stored in FRAM.
3. **Notification:** if a BLE connection is active, the sample count is
   notified to the peer.  After `ON_MS` the hardware is powered down again.
4. **Data retrieval:** when the host writes the *dump* command to the control
   characteristic, samples are sent back in chunks over BLE.  An *erase* command
   clears the stored data.

This application is meant as a starting point for very low power data‑logging
projects using the nRF52810.  Integration with your own sensor hardware may
require adapting the GPIO assignments and ADC scaling.

