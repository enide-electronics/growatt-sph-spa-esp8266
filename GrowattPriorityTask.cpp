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

GrowattPriorityTask::GrowattPriorityTask(ModbusMaster * node, uint16 priority) {
    this->node = node;
    this->priority = priority;
}

GrowattPriorityTask::~GrowattPriorityTask() {
}

String GrowattPriorityTask::subtopic() {
    return F(TOPIC_SETTINGS_PRIORITY);
}

bool GrowattPriorityTask::run() {
    if (priority <=2) {
        // so this doesn't work... register 1044 seems to be read-only like the documentation states
        // uint8_t result = this->node->writeSingleRegister(1044, priority);
        /*
         * https://community.home-assistant.io/t/esphome-modbus-growatt-shinewifi-s/369171/214
         * https://community.home-assistant.io/t/sniffing-rs232-comms-between-sph6000-and-a-shine-s/553421/4
         * Growatt does a big write.
         */
        GLOG::println(String(F(LOG_MSG)) + String(priority, DEC));
#define PRI_REG_START 1090
#define PRI_REG_LEN 20
        uint8_t result = this->node->readHoldingRegisters(PRI_REG_START, PRI_REG_LEN);
        if (result == this->node->ku8MBSuccess) {
            for (int j = 0; j < PRI_REG_LEN; j++) {
                node->setTransmitBuffer(j, node->getResponseBuffer(j));
            }
            
            ModbusUtils::dumpRegisters(this->node, PRI_REG_LEN);

            /*
             * this was used when reading from 1080...1118 (LEN = 39) but it requires HH:MM to be set on counters to work
            // grid first EN 1
            node->setTransmitBuffer(2, priority == 2 ? 1 : 0);
            // bat first EN 1
            node->setTransmitBuffer(12, priority == 1 ? 1 : 0);
            // load first EN 1
            node->setTransmitBuffer(32, priority == 0 ? 1 : 0);
            */
            
            // only change the first time slot enable bit vv
            //      28:2b:01:00:00:00:00:00:00:00:00:173b:01:00:00:00:00:00:00:00
            //                                            ^^
            node->setTransmitBuffer(12, priority == 1 ? 1 : 0);

            // don't upset the inverter
            delay(1000);
            
            // write back (TODO enable)
            uint8_t result = this->node->writeMultipleRegisters(PRI_REG_START, PRI_REG_LEN);
            
            setSuccessful(result == this->node->ku8MBSuccess);
        } else {
            GLOG::println(String(F(LOG_MSG)) + String(priority, DEC) + F(" failed, cannot read 1080...1118"));
            setSuccessful(false);
        }        
    } else {
        GLOG::println(String(F(LOG_MSG)) + String(priority, DEC) + F(" failed, invalid value"));
        setSuccessful(false);
        
    }
    return isSuccessful();
}

