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

		this->valid = true;
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
	
    if (result1 != this->node->ku8MBSuccess || result2 != this->node->ku8MBSuccess || result3 != this->node->ku8MBSuccess || result4 != this->node->ku8MBSuccess) {
		this->valid = false;
	}
	
	return 0;
}


GrowattInverter::GrowattInverter(uint8_t pinRx, uint8_t pinTx, uint8_t slaveAddress) {
	this->ser = new SoftwareSerial(pinRx, pinTx);
	this->ser->begin(9600);
	
	this->node = new ModbusMaster();
	this->node->begin(slaveAddress, (Stream &)(*(this->ser)));

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
	this->SOC = -1.0;

}
