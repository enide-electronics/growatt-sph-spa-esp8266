/*
  GrowattInverter.cpp - Library for the ESP8266/ESP32 Arduino platform
  To read data from Growatt SPH and SPA inverters

  Based heavily on the growatt-esp8266 project at https://github.com/jkairys/growatt-esp8266

  Modified by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/
#include "GrowattInverter.h"
#include "GLog.h"

static int stateSequence[] = {0, 1, 3, 0, 1, 4, 0, 1, 3, 0, 1, 4, 2};
static void incrementStateIdx(int *idx) {
    *idx = *idx + 1;
    
    if (*idx >= (sizeof(stateSequence)/sizeof(int))) {
        *idx = 0;
    }   
}

float GrowattInverter::glueFloat(uint16_t w1, uint16_t w0) {
  unsigned long t;
  t = w1 << 16;
  t += w0;

  float f;
  f = t;
  f = f / 10;
  return f;
}

void GrowattInverter::read() {
    
    GLOG::print(String(",state=") + stateSequence[currentStateIdx]);

    if (stateSequence[currentStateIdx] == 0) {
        uint8_t result1 = this->node->readInputRegisters(0, 12);
        if (result1 == this->node->ku8MBSuccess) {

            this->status = this->node->getResponseBuffer(0);

            // 2 PV inputs
            this->Vpv1 = this->glueFloat(0, this->node->getResponseBuffer(3));
            this->Ipv1 = this->glueFloat(0, this->node->getResponseBuffer(4)) / 10.0;
            this->Ppv1 = this->glueFloat(this->node->getResponseBuffer(5), this->node->getResponseBuffer(6));

            this->Vpv2 = this->glueFloat(0, this->node->getResponseBuffer(7));
            this->Ipv2 = this->glueFloat(0, this->node->getResponseBuffer(8)) / 10.0;
            this->Ppv2 = this->glueFloat(this->node->getResponseBuffer(9), this->node->getResponseBuffer(10));

            this->valid = true;
        } else {
            this->valid = false;
        }
        
    } else if (stateSequence[currentStateIdx] == 1) {
        // AC stuff, including the other phases for TL SPH inverters and energy produced
        // start reading at 35 and read up to 24 registers
        uint8_t result2 = this->node->readInputRegisters(35, 24);
        if (result2 == this->node->ku8MBSuccess) {
            this->Pac = this->glueFloat(this->node->getResponseBuffer(0), this->node->getResponseBuffer(1)); // 35, 36
            this->Fac = this->glueFloat(0, this->node->getResponseBuffer(2))/10; // 37

            this->Vac1 = this->glueFloat(0, this->node->getResponseBuffer(3)); // 38
            this->Iac1 = this->glueFloat(0, this->node->getResponseBuffer(4)); // 39
            this->Pac1 = this->glueFloat(this->node->getResponseBuffer(5), this->node->getResponseBuffer(6)); // 40, 41
    #ifdef TL_INVERTER
            this->Vac2 = this->glueFloat(0, this->node->getResponseBuffer(7)); //42
            this->Iac2 = this->glueFloat(0, this->node->getResponseBuffer(8)); //43
            this->Pac2 = this->glueFloat(this->node->getResponseBuffer(9), this->node->getResponseBuffer(10)); //44, 45

            this->Vac3 = this->glueFloat(0, this->node->getResponseBuffer(11)); //46
            this->Iac3 = this->glueFloat(0, this->node->getResponseBuffer(12)); //47
            this->Pac3 = this->glueFloat(this->node->getResponseBuffer(13), this->node->getResponseBuffer(14)); //48, 49
    #endif
            this->Etoday = this->glueFloat(this->node->getResponseBuffer(18), this->node->getResponseBuffer(19)); //53, 54
            this->Etotal = this->glueFloat(this->node->getResponseBuffer(20), this->node->getResponseBuffer(21)); //55, 56
            this->Ttotal = this->glueFloat(this->node->getResponseBuffer(22), this->node->getResponseBuffer(23)); //57, 58
            
            this->valid = true;
        } else {
            this->valid = false;
        }
        
    } else  if (stateSequence[currentStateIdx] == 2) {
        // Temperatures, Battery and Priority (LoadFirst, BatFirst, GridFirst)
        // start reading at register 93 and read up to 30 registers
        uint8_t result3 = this->node->readInputRegisters(93, 30);
        if (result3 == this->node->ku8MBSuccess) {

            this->temp1 = this->glueFloat(0, this->node->getResponseBuffer(0)); //93
            this->temp2 = this->glueFloat(0, this->node->getResponseBuffer(1)); //94
            this->temp3 = this->glueFloat(0, this->node->getResponseBuffer(2)); //95

            this->Priority = this->node->getResponseBuffer(25); //118
            this->BatteryType = this->node->getResponseBuffer(26); //119
            
            this->valid = true;
        } else {
            this->valid = false;
        }
        
    } else if (stateSequence[currentStateIdx] == 3) {
        // Battery status
        // start reading at register 1009 and read up to 6 registers
        uint8_t result4 = this->node->readInputRegisters(1009, 6);
        if (result4 == this->node->ku8MBSuccess) {

            this->Pdischarge = this->glueFloat(this->node->getResponseBuffer(0), this->node->getResponseBuffer(1)); //1009, 1010
            this->Pcharge = this->glueFloat(this->node->getResponseBuffer(2), this->node->getResponseBuffer(3)); //1011, 1012
            this->Vbat = this->glueFloat(0, this->node->getResponseBuffer(4)); //1013
            this->SOC = this->node->getResponseBuffer(5); // 1014
            
            this->valid = true;
        } else {
            this->valid = false;
        }
        
    } else {
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
            
            this->valid = true;
        } else {
            this->valid = false;
        }
        
    }
    
    lastUpdatedState = stateSequence[currentStateIdx];
    incrementStateIdx(&currentStateIdx);
}


GrowattInverter::GrowattInverter(Stream &serial, uint8_t slaveAddress) {

    this->node = new ModbusMaster();
    this->node->begin(slaveAddress, serial);
    this->currentStateIdx = 0;
    this->lastUpdatedState = 0;

    this->valid = false;
    this->status = 0;
    this->Ppv1 = 0.0;
    this->Vpv1 = 0.0;
    this->Ipv1 = 0.0;

    this->Ppv2 = 0.0;
    this->Vpv2 = 0.0;
    this->Ipv2 = 0.0;

    this->Pac1 = 0.0;
    this->Vac1 = 0.0;
    this->Iac1 = 0.0;

    this->Pac2 = 0.0;
    this->Vac2 = 0.0;
    this->Iac2 = 0.0;

    this->Pac3 = 0.0;
    this->Vac3 = 0.0;
    this->Iac3 = 0.0;

    this->Fac = 0.0;
    this->Pac = 0.0;

    this->Etoday = 0.0;
    this->Etotal = 0.0;
    this->Ttotal = 0.0;

    this->temp1 = 0.0;
    this->temp2 = 0.0;
    this->temp3 = 0.0;

    this->Priority = 0;
    this->BatteryType = 0;

    this->Pdischarge = 0.0;
    this->Pcharge = 0.0;
    this->Vbat = 0.0;
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

GrowattInverter::~GrowattInverter() {
    delete this->node;
}

bool GrowattInverter::isDataValid() {
    return this->valid;
}

InverterData GrowattInverter::getData(bool fullSet) {
    InverterData data;
    
    if (lastUpdatedState == 0 || fullSet) {
        data.set("status", this->status);
      
        data.set("Ppv1", this->Ppv1);    
        data.set("Vpv1", this->Vpv1);
        data.set("Ipv1", this->Ipv1);
      
        data.set("Ppv2", this->Ppv2);   
        data.set("Vpv2", this->Vpv2);
        data.set("Ipv2", this->Ipv2);
    }

    if (lastUpdatedState == 1 || fullSet) {
        data.set("Vac1", this->Vac1);
        data.set("Iac1", this->Iac1);
        data.set("Pac1", this->Pac1);
        
        data.set("Pac", this->Pac);
        data.set("Fac", this->Fac);
#ifdef TL_INVERTER
        data.set("Vac2", this->Vac2);
        data.set("Iac2", this->Iac2);
        data.set("Pac2", this->Pac2);
        
        data.set("Vac3", this->Vac3);
        data.set("Iac3", this->Iac3);
        data.set("Pac3", this->Pac3);
#endif

        data.set("Etoday", this->Etoday);
        data.set("Etotal", this->Etotal);
        data.set("Ttotal", this->Ttotal);
    } 

    if (lastUpdatedState == 2 || fullSet) {
        data.set("Temp1", this->temp1);
        data.set("Temp2", this->temp2);
        data.set("Temp3", this->temp3);
      
        switch (this->Priority) {
            case 0:
                data.set("Priority", "Load First");
            break;
            case 1:
                data.set("Priority", "Bat First");
            break;
            case 2:
                data.set("Priority", "Grid First");
            break;
            default:
                data.set("Priority", (String("Unknown ") + this->Priority).c_str());
        }

        // Battery
        switch (this->BatteryType) {
            case 0:
                data.set("Battery", "LeadAcid");
            break;
            
            case 1:
                data.set("Battery", "Lithium");
            break;
            default:
                data.set("Battery", (String("Unknown type ") + this->BatteryType).c_str());
        }
    }

    if (lastUpdatedState == 3 || fullSet) {
        data.set("Pdischarge", this->Pdischarge);
        data.set("Pcharge", this->Pcharge);
        data.set("Vbat", this->Vbat);
        data.set("SOC", this->SOC);
    }

    if (lastUpdatedState == 4 || fullSet) {
        // EPS
        data.set("EpsFac", this->EpsFac);

        data.set("EpsPac1", this->EpsPac1);
        data.set("EpsVac1", this->EpsVac1);
        data.set("EpsIac1", this->EpsIac1);
#ifdef TL_INVERTER
        data.set("EpsPac2", this->EpsPac2);
        data.set("EpsVac2", this->EpsVac2);
        data.set("EpsIac2", this->EpsIac2);
        
        data.set("EpsPac3", this->EpsPac3);
        data.set("EpsVac3", this->EpsVac3);
        data.set("EpsIac3", this->EpsIac3);
#endif

        data.set("EpsLoadPercent", this->EpsLoadPercent);
        data.set("EpsPF", this->EpsPF);
    }
    
    return data;
}
