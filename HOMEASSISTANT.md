# Home Assistant entity configurations
If you use [Home Assistant](https://www.home-assistant.io/), here's a preconfigured list of all the MQTT sensors, to help you get started.

However, all inverter data is sent to a MQTT server of your choosing.
In no way you are required to run [Home Assistant](https://www.home-assistant.io/) to access it.

## Live data sensors (input registers)
```
mqtt:
  sensor:
    - name: "Growatt Status"
      state_topic: "growatt/status"
      unit_of_measurement: ""
    - name: "Growatt Derate Mode"
      state_topic: "growatt/DerateMode"
      unit_of_measurement: ""
    - name: "Growatt Derate"
      state_topic: "growatt/Derate"
    - name: "Growatt Ppv1"
      state_topic: "growatt/Ppv1"
      unit_of_measurement: "W"
      state_class: measurement
      device_class: power
    - name: "Growatt Vpv1"
      state_topic: "growatt/Vpv1"
      unit_of_measurement: "V"
      state_class: measurement
      device_class: voltage
    - name: "Growatt Ipv1"
      state_topic: "growatt/Ipv1"
      unit_of_measurement: "A"
      state_class: measurement
      device_class: current
    - name: "Growatt Ppv2"
      state_topic: "growatt/Ppv2"
      unit_of_measurement: "W"
      state_class: measurement
      device_class: power
    - name: "Growatt Vpv2"
      state_topic: "growatt/Vpv2"
      unit_of_measurement: "V"
      state_class: measurement
      device_class: voltage
    - name: "Growatt Ipv2"
      state_topic: "growatt/Ipv2"
      unit_of_measurement: "A"
      state_class: measurement
      device_class: current
    - name: "Growatt Vac1"
      state_topic: "growatt/Vac1"
      unit_of_measurement: "V"
      state_class: measurement
      device_class: voltage
    - name: "Growatt Iac1"
      state_topic: "growatt/Iac1"
      unit_of_measurement: "A"
      state_class: measurement
      device_class: current
    - name: "Growatt Pac1"
      state_topic: "growatt/Pac1"
      unit_of_measurement: "VA"
      state_class: measurement
    - name: "Growatt Po"
      state_topic: "growatt/Pac"
      unit_of_measurement: "W"
      state_class: measurement
      device_class: power
    - name: "Growatt Fac"
      state_topic: "growatt/Fac"
      unit_of_measurement: "Hz"
      state_class: measurement
      device_class: frequency
    - name: "Growatt Daily Generated Energy"
      state_topic: "growatt/Etoday"
      unit_of_measurement: "kWh"
      state_class: total_increasing
      device_class: energy
    - name: "Growatt Total Generated Energy"
      state_topic: "growatt/Etotal"
      unit_of_measurement: "kWh"
      state_class: total_increasing
      device_class: energy
    - name: "Growatt Inverter Temp"
      state_topic: "growatt/Temp1"
      unit_of_measurement: "C"
    - name: "Growatt IPM Inverter Temp"
      state_topic: "growatt/Temp2"
      unit_of_measurement: "C"
    - name: "Growatt Boost Temp"
      state_topic: "growatt/Temp3"
      unit_of_measurement: "C"
    - name: "Growatt Priority"
      state_topic: "growatt/Priority"
      unit_of_measurement: ""
      device_class: "enum"
    # -------------------------------------
    - name: "Growatt Battery Type"
      state_topic: "growatt/Battery"
      unit_of_measurement: ""
      device_class: "enum"
    - name: "Growatt Battery Discharge Power"
      state_topic: "growatt/Pdischarge"
      unit_of_measurement: "W"
      state_class: measurement
      device_class: power
    - name: "Growatt Battery Charge Power"
      state_topic: "growatt/Pcharge"
      unit_of_measurement: "W"
      state_class: measurement
      device_class: power
    - name: "Growatt Battery Voltage"
      state_topic: "growatt/Vbat"
      unit_of_measurement: "V"
      state_class: measurement
      device_class: voltage
    - name: "Growatt Battery SOC"
      state_topic: "growatt/SOC"
      unit_of_measurement: "%"
      state_class: measurement
      device_class: battery
    # -------------------------------------
    - name: "Growatt EPS Fac"
      state_topic: "growatt/EpsFac"
      unit_of_measurement: "Hz"
      state_class: measurement
      device_class: frequency
    - name: "Growatt EPS Pac1"
      state_topic: "growatt/EpsPac1"
      unit_of_measurement: "W"
      state_class: measurement
      device_class: power
    - name: "Growatt EPS Vac1"
      state_topic: "growatt/EpsVac1"
      unit_of_measurement: "V"
      state_class: measurement
      device_class: voltage
    - name: "Growatt EPS Iac1"
      state_topic: "growatt/EpsIac1"
      unit_of_measurement: "A"
      state_class: measurement
      device_class: current
    - name: "Growatt EPS Load Percent"
      state_topic: "growatt/EpsLoadPercent"
      unit_of_measurement: "%"
    - name: "Growatt EPS PF"
      state_topic: "growatt/EpsPF"
```

## Settings (holding registers)
```
TBD
```

