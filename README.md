# Temperature Sensor Program:
**Authors:** Lucas Paul, Valerie Hanes, Ny Ary Razafindramamba  
**Date:** Spring 2025


**Purpose:**
The goal of this file was to receive temperature data from sensors and send the data to be read by another program.

The temperature sensors use 1-Wire communication and the microcontroller communicates with the other program via a string sent by UART (UART via USB).

## Outline Goal:

* Connect microcontroller to temperature sensor and ask for temperature data from all (100) sensors
* Attain data at set intervals (2s)
* Print readable data in 1D 100 element string format for Python script


## Components:
* Dallas Semiconductors DS 18B20 temperature sensors (100)
* Nordic Semiconductors NRF52840-DK development board
* 4.7k resistor (YVR)
**Note:** Pull up resistor is required for 1-Wire communication. More resistance may be required for a larger system of sensors

## Configuration

### Devicetree overlay:
* 1-Wire serial communication
* DS18B20: 12-bit resolution
* UART baud rate set to 19200 Bits/s (Bd)

### Kernel configuration via 'prj.conf':
* 1-wire network layer and drivers
* 1-wire multidrop addressing
* UART console (for transport using serial communication)
* Logging
* Printf with folating point support

## Hardware Setup
### External  
**NOTE:** All pins connected to DS18B20 are relative to flat side forwards

* 5V power to right pin of DS 18B20
* GND to left pin of DS 18B20
* P1.01 to middle pin as receive (Rx)
* P1.02 to middle pin as transmit (Tx)
* 4.7 k resistor (YVR) in series with Rx and Tx to VDD (3.3V)
* DS18B20 sensors connected in parallel to the previous sensor
