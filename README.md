The goal of this file is to succesfully connect and communicate with a DS 18B20  Dallas Semiconductors temperature sensor.

The temperature sensors use serial communication (UART via Universal Serial Bus [USB]).

Materials for this program:
* Dallas Semiconductors DS 18B20 Temperature sensors (100)
* Nordic Semiconductors NRF52840-DK development board

Outline Goal:

* Connect to Temperature sensor and receive room temperature data
* Receive data at set intervals
* Ramp up to many temperature sensors connected in ... ? (P/S)
* Transmit the data into a 1D array of all 100 data points to be collected by another program\
