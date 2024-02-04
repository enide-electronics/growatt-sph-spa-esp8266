# MQTT Topics
Please note that the "growatt" prefix in all topics shown below is the default one. It is configurable via the web interface if you want to change it. 

## Energy Data
Energy data is polled periodically from the inverter Input Registers, every N seconds defined via the web interface.

| Topic                       | Units | Format | Description                                                           |
|-----------------------------|-------|--------|-----------------------------------------------------------------------|
| `growatt/status`            | -     | int    | Inverter numeric status                                               |
| `growatt/Priority`          | -     | text   | Inverter working priority (Load, Bat, Grid)                           |
| `growatt/DerateMode`        | -     | int    | Derating due to overtemp, overvoltage, unstable frequency, etc.       |
| `growatt/Derate`            | -     | text   | Derating textual cause                                                |
|-----------------------------|-------|--------|-----------------------------------------------------------------------|
| `growatt/Vpv1`              | V     | float  | Voltage of solar string 1                                             |
| `growatt/Ppv1`              | W     | float  | Power of solar string 1                                               |
| `growatt/Ipv1`              | A     | float  | Current of solar string 1                                             |
| `growatt/Vpv2`              | V     | float  | Voltage of solar string 2                                             |
| `growatt/Ppv2`              | W     | float  | Power of solar string 2                                               |
| `growatt/Ipv2`              | A     | float  | Current of solar string 2                                             |
|-----------------------------|-------|--------|-----------------------------------------------------------------------|
| `growatt/Vac1`              | V     | float  | Grid Phase 1 voltage                                                  | 
| `growatt/Iac1`              | A     | float  | Grid Phase 1 current                                                  |
| `growatt/Pac1`              | VA    | float  | Grid Phase 1 apparent power                                           |
| `growatt/Vac2`              | V     | float  | Grid Phase 2 voltage (disabled by default, TL inverters only)         | 
| `growatt/Iac2`              | A     | float  | Grid Phase 2 current (disabled by default, TL inverters only)         |
| `growatt/Pac2`              | VA    | float  | Grid Phase 2 apparent power (disabled by default, TL inverters only)  |
| `growatt/Vac3`              | V     | float  | Grid Phase 3 voltage (disabled by default, TL inverters only)         | 
| `growatt/Iac3`              | A     | float  | Grid Phase 3 current (disabled by default, TL inverters only)         |
| `growatt/Pac3`              | VA    | float  | Grid Phase 3 apparent power (disabled by default, TL inverters only)  |
| `growatt/Pac`               | W     | float  | Grid Tie inverter output ACTIVE power                                 |
| `growatt/Fac`               | Hz    | float  | Grid Tie inverter output/grid frequency                               |
|-----------------------------|-------|--------|-----------------------------------------------------------------------|
| `growatt/Etoday`            | kWh   | float  | Total energy produced today                                           |
| `growatt/Etotal`            | kWh   | float  | Total energy produced                                                 |
| `growatt/Ttotal`            | Hours | float  | Total time inverter running                                           |
|-----------------------------|-------|--------|-----------------------------------------------------------------------|
| `growatt/Temp1`             | ºC    | float  | Temperature of inverter                                               |
| `growatt/Temp2`             | ºC    | float  | Temperature inside IPM of inverter                                    |
| `growatt/Temp3`             | ºC    | float  | Temperature of boost module                                           |
|-----------------------------|-------|--------|-----------------------------------------------------------------------|
| `growatt/Battery`           | -     | text   | Type of battery (Lithium, LeadAcid or Unknown)                        |
| `growatt/Pdischarge`        | W     | float  | Battery discharge power                                               |
| `growatt/Pcharge`           | W     | float  | Battery charge power                                                  |
| `growatt/Vbat`              | V     | float  | Battery voltage                                                       |
| `growatt/SOC`               | %     | int    | Battery charge percentage (state of charge)                           |
|-----------------------------|-------|--------|-----------------------------------------------------------------------|
| `growatt/EpsFac`            | Hz    | float  | EPS output frequency                                                  |
| `growatt/EpsVac1`           | V     | float  | EPS Phase 1 voltage                                                   | 
| `growatt/EpsIac1`           | A     | float  | EPSPhase 1 current                                                    |
| `growatt/EpsPac1`           | W     | float  | EPS Phase 1 total power                                               |
| `growatt/EpsVac2`           | V     | float  | EPS Phase 2 voltage (disabled by default, for TL inverters only)      | 
| `growatt/EpsIac2`           | A     | float  | EPSPhase 2 current (disabled by default, for TL inverters only)       |
| `growatt/EpsPac2`           | W     | float  | EPS Phase 2 total power (disabled by default, for TL inverters only)  |
| `growatt/EpsVac3`           | V     | float  | EPS Phase 3 voltage (disabled by default, for TL inverters only)      | 
| `growatt/EpsIac3`           | A     | float  | EPSPhase 3 current (disabled by default, for TL inverters only)       |
| `growatt/EpsPac3`           | W     | float  | EPS Phase 3 total power (disabled by default, for TL inverters only)  |
| `growatt/EpsLoadPercent`    | %     | float  | EPS Load in percentage 100% at 3kW                                    |
| `growatt/EpsPF`             | -     | float  | EPS power factor                                                      |



## Settings
Inverter settings are kept in Holding Registers.
These can be modified by publishing messages with the correct values to the following MQTT topics:

| Topic                                 | Value                                            | Parameter                                | Observations                                                        | 
|---------------------------------------|--------------------------------------------------|------------------------------------------|---------------------------------------------------------------------|
| `growatt/settings/led`                | `0` OR `1` OR `2`                                | Default ESP8266 LED behaviour            | LED always OFF (0), always ON (1) or blinking when polling data (2) |
| `growatt/settings/priority`           | `load` OR `bat` OR `grid`                        | Priority setting from the inverter menu  | `grid` is being tested and may require more work to work correctly  |
|---------------------------------------|--------------------------------------------------|------------------------------------------|---------------------------------------------------------------------|
| `growatt/settings/priority/bat/ac`    | `0` OR `1` OR `off` OR `on` OR `false` OR `true` | AC Charger                               | Enables or disables the AC Charger in Battery First                 |
| `growatt/settings/priority/bat/pr`    | `1` ... `100`                                    | Charge Power Rating                      | Battery First charging power rating                                 |
| `growatt/settings/priority/bat/ssoc`  | `13` ... `100`                                   | Stop State Of Charge                     | Battery First SSOC                                                  |
| `growatt/settings/priority/bat/t1`    | `00:00 23:59`                                    | Battery First Time                       | Battery First Time Interval 1 that can be set from the panel        |
|---------------------------------------|--------------------------------------------------|------------------------------------------|---------------------------------------------------------------------|
| `growatt/settings/priority/grid/pr`   | `1` ... `100`                                    | Discharge Power Rating                   | Grid First discharge power rating                                   |
| `growatt/settings/priority/grid/ssoc` | `13` ... `100`                                   | Stop State Of Charge                     | Grid First SSOC                                                     |
| `growatt/settings/priority/grid/t1`   | `00:00 23:59`                                    | Grid First Time                          | Grid First Time Interval 1 that can be set from the panel           |


## Settings currently not enabled in the code

:warning:  These topics **are supported** by the code but are disabled in the released binaries for safety.

:warning:  You can recompile the code to enable them if you want.

:warning:  Remember, if you set any of these values, they will not be visible on the inverter screen nor they can be modified through the panel.

:warning:  You will only be able to change them or disable them via their MQTT topics.

| Topic                               | Value         | Parameter            | Observations                  | 
|-------------------------------------|---------------|----------------------|-------------------------------|
| `growatt/settings/priority/bat/t2`  | `00:00 23:59` | Battery First Time 2 | Battery First Time Interval 2 |
| `growatt/settings/priority/bat/t3`  | `00:00 23:59` | Battery First Time 3 | Battery First Time Interval 3 |
|-------------------------------------|---------------|----------------------|-------------------------------|
| `growatt/settings/priority/grid/t2` | `00:00 23:59` | Grid First Time 2    | Grid First Time Interval 2    |
| `growatt/settings/priority/grid/t3` | `00:00 23:59` | Grid First Time 3    | Grid First Time Interval 3    |


## Modbus and inverter registers
See [REGISTERS.md](REGISTERS.md) for more details.