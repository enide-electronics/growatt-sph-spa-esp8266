/*
  GrowattPriorityTask.cpp - Library header for the ESP8266/ESP32 Arduino platform
  Changes the inverter priority to Load First, Battery First or Grid First
  
  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/
#include "GrowattPriorityTask.h"
#include "Glog.h"
#include "ModbusUtils.h"

#define LOG_MSG "GrowattPriorityTask: set priority to "

GrowattPriorityTask::GrowattPriorityTask(ModbusMaster * node, const String &mqttValue) {
    this->node = node;
    this->mqttValue = mqttValue;
    this->mqttValue.trim();
}

GrowattPriorityTask::~GrowattPriorityTask() {
}

String GrowattPriorityTask::subtopic() {
    return F(TOPIC_SETTINGS_PRIORITY);
}

// so this doesn't work... register 1044 seems to be read-only like the documentation states
// uint8_t result = this->node->writeSingleRegister(1044, priority);
/*
 * https://community.home-assistant.io/t/esphome-modbus-growatt-shinewifi-s/369171/214
 * https://community.home-assistant.io/t/sniffing-rs232-comms-between-sph6000-and-a-shine-s/553421/4
 * Growatt does a big write.
 */
 
#define GRID_REG_START 1080
#define BAT_REG_START 1100
#define TIME_REG_LEN 3
bool GrowattPriorityTask::run() {
    String &priority = this->mqttValue;
    GLOG::print(String(F(LOG_MSG)) + priority);
    setSuccessful(false);

    if (priority == F(TOPIC_VALUE_PRIORITY_LOAD)) {
        if (!checkAndSetEnableBit(BAT_REG_START, 0)) {
            GLOG::println(String(F(LOG_MSG)) + priority + F(" failed, cannot read/write 1100...1102"));
            return false;
        }
        if (!checkAndSetEnableBit(GRID_REG_START, 0)) {
            GLOG::println(String(F(LOG_MSG)) + priority + F(" failed, cannot read/write 1080...1082"));
            return false;
        }
        setSuccessful(true);
    } else if (priority == F(TOPIC_VALUE_PRIORITY_BAT)) {
        if (!checkAndSetEnableBit(GRID_REG_START, 0)) {
            GLOG::println(String(F(LOG_MSG)) + priority + F(" failed, cannot read/write 1080...1082"));
            return false;
        }
        if (!checkAndSetEnableBit(BAT_REG_START, 1)) {
            GLOG::println(String(F(LOG_MSG)) + priority + F(" failed, cannot read/write 1100...1102"));
            return false;
        }
        setSuccessful(true);
    } else if (priority == F(TOPIC_VALUE_PRIORITY_GRID)) {
        if (!checkAndSetEnableBit(BAT_REG_START, 0)) {
            GLOG::println(String(F(LOG_MSG)) + priority + F(" failed, cannot read/write 1100...1102"));
            return false;
        }
        if (!checkAndSetEnableBit(GRID_REG_START, 1)) {
            GLOG::println(String(F(LOG_MSG)) + priority + F(" failed, cannot read/write 1080...1082"));
            return false;
        }
        setSuccessful(true);
    } else {
        GLOG::println(String(F(LOG_MSG)) + priority + F(" failed, invalid value"));
        setSuccessful(false);
    }
    
    return isSuccessful();
}

bool GrowattPriorityTask::checkAndSetEnableBit(uint16_t baseAddr, uint8_t bitValue) {
    uint8_t result = this->node->readHoldingRegisters(baseAddr, TIME_REG_LEN);
    
    if (result == this->node->ku8MBSuccess) {
        for (int j = 0; j < TIME_REG_LEN; j++) {
            node->setTransmitBuffer(j, node->getResponseBuffer(j));
        }
        
        // dump to logs
        ModbusUtils::dumpRegisters(this->node, TIME_REG_LEN);
        
        // ensure 0 or 1
        bitValue &= 0x01;
        
        if (this->node->getResponseBuffer(2) != bitValue) {
            // don't upset the inverter
            delay(1000);
            
            // set bit
            this->node->setTransmitBuffer(2, bitValue);
                
            // write back to inverter
            result = this->node->writeMultipleRegisters(baseAddr, TIME_REG_LEN);
        }
    }
    
    // waits even if read fails
    delay(1000);
    return result == this->node->ku8MBSuccess;
}

