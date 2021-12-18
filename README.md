# growatt-esp8266-sph-spa
This project uses a ESP8266 (NodeMCU v3, Wemos D1, etc.) to read energy data from a **Growatt SPH** or **Growatt SPA** inverter.
It publishes this data to a MQTT server from where it can be consumed by almost any Home Automation solution.

Many ideas to implement this project came from other projects, most notably:
- [growatt-esp8266](https://github.com/jkairys/growatt-esp8266)
- [growatt-rs232-reader](https://github.com/lemval/growatt-rs232-reader)

## Configuration:
You need to modify the "settings.h" file and add your Wifi and MQTT settings.

## Building it:
At the time I wrote this project I used the Arduino "IDE" version 1.8.16 and the following libraries
- esp8266 core versions 2.6.3 & 2.7.4 (both work fine and I prefer the latter)
- pubsubclient version 2.8.0
- modbusmaster version 2.0.1

## Using it
You'll need ad adapter board with a MAX3232 or similar 3.3V RS232 converter to connect between the ESP8266 board and the inverter RS232 port.
As seen below, pins D5 and D6 are the soft serial port used to communicate with the inverter at 9600bps.
It's possible to power the board from the inverter if you add a voltage regulator to convert the 8V down to 3.3v, required by the ESP8266.

```
    FROM INVERTER
-----------------------   2 = TX
\ (5) (4) (3) (2) (1) /   3 = RX
 \  (9) (8) (7) (6)  /    5 = GND
  \_________________/     9 = V+ (about 8 volts)


  Inverter DB9           Our DB9       PCB         Wemos
    (female)              (male)       (J4)        (pins)
       2 ---(TX)----(RX)--- 2 --------- 3 ---(RX)--- D6
       3 ---(RX)----(TX)--- 3 --------- 4 ---(TX)--- D5
       5 ---(GND)---------- 5 --------- 1 ---------- GND
       9 ---(V+)----------- 9 --------- 2 -- VREG -- VCC 

credit: https://github.com/jkairys/growatt-esp8266
```

## The Growatt SPH and SPA inverters
These inverters use a different modbus input register map from the one found on growatt-esp8266 project.
Below are the registers currently being read by this project and what data they contain.

### Main input registers
```
0   Inverter status (0, 1, 3, 5 & 6)
    0 Standby
    1 Normal / Checking
    3 Error / Fault
    5 Normal with Solar power
    6 Normal without Solar power

3   Vpv1   (PV1 DC voltage)
4   Ipv1   (PV1 DC current)
5   Ppv1 H
6   Ppv1 L (PV1 power in watts)

7   Vpv2   (PV2 DC voltage)
8   Ipv2   (PV2 DC voltage)
9   Ppv2 H
10  Ppv2 L (PV2 power in watts)

35  Pac H
36  Pac L  (Power output in Watts)
37  Fac    (AC Grid frequency)

38  Vac1   (AC Grid phase 1 voltage)
39  Iac1   (AC Grid phase 1 current)
40  Pac1 H  
41  Pac1 L (AC Grid phase 1 power output in VA)

--------------------------- 8< ------------------------------
If you have a 3 phase inverter you can enable these as well

42  Vac2   (AC Grid phase 2 voltage)
43  Iac2   (AC Grid phase 2 current)
44  Pac2 H  
45  Pac2 L (AC Grid phase 2 power output in VA)

46  Vac3   (AC Grid phase 3 voltage)
47  Iac3   (AC Grid phase 3 current)
48  Pac3 H  
49  Pac3 L (AC Grid phase 3 power output in VA)
--------------------------- 8< ------------------------------

53  Etotal H
54  Etotal L  (total energy produced by inverter [in watts ?])
55  Etoday H
56  Etoday L  (energy produced by inverter today [in watts ?])
57  Ttotal H
58  Ttotal L  (total time inverter running [unknown units])
```

### Extra input registers of interest
```
93  Temp 1 inverter / ºC
94  Temp 2 inverter IPM / ºC 
95  Temp 3 inverter boost / ºC
96  Temp 4 (reserved and not read)

100 Inverter output PF now 0-20000  (always 0)
101 Real output power percent 0-100 (always 0)

118 Priority 0:LoadF, 1:BatF, 2:GridF
119 Battery type 0:LeadAcid, 1:Lithium 

1009 PdischargeH
1010 PdischargeL
1011 PchargeH
1012 PchargeL
1013 Vbat
1014 SOC
```

### Not used but seem interesting to add in the future
```
BMS related (Pylontech)
1086 BMS_SOC
1087 BMS_BatteryVolt
1088 BMS_BatteryCurr
1089 BMS_BatteryTemp
1090 BMS_MaxCurr (pylon)
```
