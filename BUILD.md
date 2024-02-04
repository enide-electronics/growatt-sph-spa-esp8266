# Building it

At the time I wrote this project I used the Arduino "IDE" version 1.8.16 and the following libraries
- esp8266 core versions 2.6.3 & 2.7.4 (both work fine and I prefer the latter)
- pubsubclient version 2.8.0
- modbusmaster version 2.0.1
- wifimanager version 2.0.5-beta
- arduinojson version 6.19.2

You'll need to add them to your Arduino "IDE" and then you should be able to compile the code.

:warning: If you plan on running the ESP8266 board connected to your computer to debug changes you made to the code, **make sure to not power the board from the inverter serial pin 9** otherwise you'll risk frying the ESP module, your computer or the inverter.
Power the board from the USB cable only.
Remember the Growatt SPH and SPA are non-isolated inverters.
