/*
  GrowattInverter.cpp - Library for the ESP8266/ESP32 Arduino platform
  To read data from Growatt SPH and SPA inverters

  Based heavily on the growatt-esp8266 project at https://github.com/jkairys/growatt-esp8266

  Modified by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/
#include "GrowattInverter.h"

float GrowattInverter::glueFloat(uint16_t w1, uint16_t w0) {
  unsigned long t;
  t = w1 << 16;
  t += w0;

  float f;
  f = t;
  f = f / 10;
  return f;
}

uint8_t GrowattInverter::read() {
    uint8_t result1 = this->node->readInputRegisters(0, REGISTERS_TO_READ);

    if (result1 == this->node->ku8MBSuccess) {

        this->status = this->node->getResponseBuffer(0);

        // base mono phase SPH inverter with 2 PV inputs
        this->Vpv1 = this->glueFloat(0, this->node->getResponseBuffer(3));
        this->Ipv1 = this->glueFloat(0, this->node->getResponseBuffer(4)) / 10.0;
        this->Ppv1 = this->glueFloat(this->node->getResponseBuffer(5), this->node->getResponseBuffer(6));

        this->Vpv2 = this->glueFloat(0, this->node->getResponseBuffer(7));
        this->Ipv2 = this->glueFloat(0, this->node->getResponseBuffer(8)) / 10.0;
        this->Ppv2 = this->glueFloat(this->node->getResponseBuffer(9), this->node->getResponseBuffer(10));

        this->Pac = this->glueFloat(this->node->getResponseBuffer(35), this->node->getResponseBuffer(36));
        this->Fac = this->glueFloat(0, this->node->getResponseBuffer(37))/10;

        this->Vac1 = this->glueFloat(0, this->node->getResponseBuffer(38));
        this->Iac1 = this->glueFloat(0, this->node->getResponseBuffer(39));
        this->Pac1 = this->glueFloat(this->node->getResponseBuffer(40), this->node->getResponseBuffer(41));

    }

    // Stuff for three phase SPH inverters and energy produced
    // start reading at 42 and read up to 20 registers
    uint8_t result2 = this->node->readInputRegisters(42, 20);
    if (result2 == this->node->ku8MBSuccess) {
#ifdef TL_INVERTER
        this->Vac2 = this->glueFloat(0, this->node->getResponseBuffer(0)); //42
        this->Iac2 = this->glueFloat(0, this->node->getResponseBuffer(1)); //43
        this->Pac2 = this->glueFloat(this->node->getResponseBuffer(2), this->node->getResponseBuffer(3)); //44, 45

        this->Vac3 = this->glueFloat(0, this->node->getResponseBuffer(4)); //46
        this->Iac3 = this->glueFloat(0, this->node->getResponseBuffer(5)); //47
        this->Pac3 = this->glueFloat(this->node->getResponseBuffer(6), this->node->getResponseBuffer(7)); //48, 49
#endif
        this->Etoday = this->glueFloat(this->node->getResponseBuffer(11), this->node->getResponseBuffer(12)); //53, 54
        this->Etotal = this->glueFloat(this->node->getResponseBuffer(13), this->node->getResponseBuffer(14)); //55, 56
        this->Ttotal = this->glueFloat(this->node->getResponseBuffer(15), this->node->getResponseBuffer(16)); //57, 58
    }

    // Temperatures, Power and SPH inverter Work mode
    // start reading at register 93 and read up to 30 registers
    uint8_t result3 = this->node->readInputRegisters(93, 30);
    if (result3 == this->node->ku8MBSuccess) {

        this->temp1 = this->glueFloat(0, this->node->getResponseBuffer(0)); //93
        this->temp2 = this->glueFloat(0, this->node->getResponseBuffer(1)); //94
        this->temp3 = this->glueFloat(0, this->node->getResponseBuffer(2)); //95

        this->Priority = this->node->getResponseBuffer(25); //118
        this->BatteryType = this->node->getResponseBuffer(26); //119
    }

    // Battery status
    // start reading at register 1009 and read up to 6 registers
    uint8_t result4 = this->node->readInputRegisters(1009, 6);
    if (result4 == this->node->ku8MBSuccess) {

        this->Pdischarge = this->glueFloat(this->node->getResponseBuffer(0), this->node->getResponseBuffer(1)); //1009, 1010
        this->Pcharge = this->glueFloat(this->node->getResponseBuffer(2), this->node->getResponseBuffer(3)); //1011, 1012
        this->Vbat = this->glueFloat(0, this->node->getResponseBuffer(4)); //1013
        this->SOC = this->node->getResponseBuffer(5); // 1014
    }

    // EPS
    // EPS starts at register 1067 and is 15 registers long (see page 44)
    uint8_t result5 = this->node->readInputRegisters(1067, 15);
    if (result5 == this->node->ku8MBSuccess) {

        this->EpsFac = this->glueFloat(0, this->node->getResponseBuffer(0)) / 10.0; //1067

        this->EpsVac1 = this->glueFloat(0, this->node->getResponseBuffer(1)); //1068
        this->EpsIac1 = this->glueFloat(0, this->node->getResponseBuffer(2)); //1069
        this->EpsPac1 = this->glueFloat(this->node->getResponseBuffer(3), this->node->getResponseBuffer(4)); //1070, 1071

#ifdef TL_INVERTER
        this->EpsVac2 = this->glueFloat(0, this->node->getResponseBuffer(5)); //1072
        this->EpsIac2 = this->glueFloat(0, this->node->getResponseBuffer(6)); //1073
        this->EpsPac2 = this->glueFloat(this->node->getResponseBuffer(7), this->node->getResponseBuffer(8)); //1074, 1075

        this->EpsVac3 = this->glueFloat(0, this->node->getResponseBuffer(9)); //1076
        this->EpsIac3 = this->glueFloat(0, this->node->getResponseBuffer(10)); //1077
        this->EpsPac3 = this->glueFloat(this->node->getResponseBuffer(11), this->node->getResponseBuffer(12)); //1078, 1079
#endif

        this->EpsLoadPercent = this->node->getResponseBuffer(13); //1080
        this->EpsPF = this->glueFloat(0, this->node->getResponseBuffer(14)) / 100.0; //1081
    }

    if (result1 != this->node->ku8MBSuccess || result2 != this->node->ku8MBSuccess
    || result3 != this->node->ku8MBSuccess || result4 != this->node->ku8MBSuccess
    || result5 != this->node->ku8MBSuccess) {
        this->valid = false;
    } else {
        this->valid = true;
    }

    return 0;
}


GrowattInverter::GrowattInverter(Stream &serial, uint8_t slaveAddress) {

    this->node = new ModbusMaster();
    this->node->begin(slaveAddress, serial);

    this->valid = false;
    this->status = 0;
    this->Ppv1 = -1.0;
    this->Vpv1 = -1.0;
    this->Ipv1 = -1.0;

    this->Ppv2 = -1.0;
    this->Vpv2 = -1.0;
    this->Ipv2 = -1.0;

    this->Pac1 = -1.0;
    this->Vac1 = -1.0;
    this->Iac1 = -1.0;

    this->Pac2 = -1.0;
    this->Vac2 = -1.0;
    this->Iac2 = -1.0;

    this->Pac3 = -1.0;
    this->Vac3 = -1.0;
    this->Iac3 = -1.0;

    this->Fac = -1.0;
    this->Pac = -1.0;

    this->Etoday = -1.0;
    this->Etotal = -1.0;
    this->Ttotal = -1.0;

    this->temp1 = -1.0;
    this->temp2 = -1.0;
    this->temp3 = -1.0;

    this->Priority = 0;
    this->BatteryType = 0;

    this->Pdischarge = -1.0;
    this->Pcharge = -1.0;
    this->Vbat = -1.0;
    this->SOC = 0;

    this->EpsFac = 0;

    this->EpsPac1 = 0.0;
    this->EpsVac1 = 0.0;
    this->EpsIac1 = 0.0;

    this->EpsPac2 = 0.0;
    this->EpsVac2 = 0.0;
    this->EpsIac2 = 0.0;

    this->EpsPac3 = 0.0;
    this->EpsVac3 = 0.0;
    this->EpsIac3 = 0.0;

    this->EpsLoadPercent = 0.0;
    this->EpsPF = 0.0;
}
