The goal of this file is to succesfully connect and communicate with a DS 18B20  Dallas Semiconductors temperature sensor.

The temperature sensors use serial communication (UART via Universal Serial Bus [USB]).

Materials for this program:
* Dallas Semiconductors DS 18B20 Temperature sensors (100)
* Nordic Semiconductors NRF52840-DK development board

Outline Goal:

* Connect to Temperature sensor and receive room temperature data
* Receive data at set intervals
* Ramp up to many temperature sensors connected in ... ? (P/S)
* Transmit the data into a 1D array of all 100 data points to be collected by another program


Hardware Setup for testing one DS 18B20:
* NOTE: All pins connected to DS 18B20 are relative to flat side towards user

* 5V power to right pin of DS 18B20
* GND to left pin of DS 18B20
* P1.01 to middle pin as receive (Rx)
* P1.02 to middle pin as transmit (Tx)
* 4.7 kO resistor (YVO_Go) in series with Rx and Tx to VDD (3.3V)