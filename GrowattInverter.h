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
#include <SoftwareSerial.h>
#define SLAVE_ADDRESS 1
#define REGISTERS_TO_READ 45


class GrowattInverter
{
  private:
    float glueFloat(uint16_t w1, uint16_t w2);
    SoftwareSerial * ser;
    ModbusMaster * node;

  public:
    GrowattInverter(uint8_t pinRx, uint8_t pinTx, uint8_t slaveAddress);
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

    bool valid;

    uint8_t status;

};

#endif
