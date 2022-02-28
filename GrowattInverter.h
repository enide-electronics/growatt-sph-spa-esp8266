/*
  GrowattInverter.h - Library header for the ESP8266/ESP32 Arduino platform
  To read data from Growatt SPH and SPA inverters
  
  Based heavily on the growatt-esp8266 project at https://github.com/jkairys/growatt-esp8266
  
  Modified by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/

#ifndef _GrowattInverter_h
#define _GrowattInverter_h

#include <Arduino.h>
#include <stdint.h>
#include <functional>
#include <ModbusMaster.h>
#define REGISTERS_TO_READ 45


class GrowattInverter
{
  private:
    float glueFloat(uint16_t w1, uint16_t w2);
    ModbusMaster * node;

  public:
    GrowattInverter(Stream &serial, uint8_t slaveAddress);
    uint8_t read();

    float Ppv1;
    float Vpv1;
    float Ipv1;

    float Ppv2;
    float Vpv2;
    float Ipv2;

    float Pac1; // VA
    float Vac1;
    float Iac1;

    float Pac2; // VA
    float Vac2;
    float Iac2;

    float Pac3; // VA
    float Vac3;
    float Iac3;
    
    float Fac; // Hz
    float Pac; // W

    float Etoday;
    float Etotal;
    float Ttotal;
	
    float temp1;
    float temp2;
    float temp3;
	
    uint8_t Priority;
    uint8_t BatteryType;
	
    float Pdischarge;
    float Pcharge;
    float Vbat;
    uint16_t SOC;
	
	// EPS output
	float EpsFac; // Hz
	
	float EpsPac1; // VA
	float EpsVac1;
	float EpsIac1;
	
	float EpsPac2; // VA
	float EpsVac2;
	float EpsIac2;
	
	float EpsPac3; // VA
	float EpsVac3;
	float EpsIac3;
	
	uint16_t EpsLoadPercent; // 0 .. 100
	float EpsPF; // -1.0 .. 1.0

    bool valid;

    uint8_t status;

};

#endif
