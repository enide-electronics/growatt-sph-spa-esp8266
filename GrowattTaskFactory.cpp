/*
  GrowattTaskFactory.cpp - Library header for the ESP8266/ESP32 Arduino platform
  Builds tasks based on the incoming topic and value (MQTT)
  
  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/
#include "GrowattTaskFactory.h"
#include "GrowattPriorityTask.h"
#include "GrowattReadHoldingTask.h"

Task* GrowattTaskFactory::create(ModbusMaster *node, const String &topic, const String &value) {
    Task *task = NULL;
    
    if (topic == String(F(TOPIC_SETTINGS_PRIORITY))) {
        if (value == String(F(TOPIC_VALUE_PRIORITY_LOAD))) {
            task = new GrowattPriorityTask(node, 0);
        } else if (value == String(F(TOPIC_VALUE_PRIORITY_BAT))) {
            task = new GrowattPriorityTask(node, 1);
        } else if (value == String(F(TOPIC_VALUE_PRIORITY_GRID))) {
            task = new GrowattPriorityTask(node, 2);
        }
    } else if (topic == String(F(TOPIC_SETTINGS_READ_HOLDING_TASK))) {
        String payload = value;
        String fields[2];
        uint8_t fieldsIndex = 0;
        
        while (payload.length() > 0) {
            int index = payload.indexOf(' ');
            if (index == -1) {
                // no space, keep whole/remaining string
                fields[fieldsIndex++] = payload;
                break;
            } else {
                fields[fieldsIndex++] = payload.substring(0, index);
                payload = payload.substring(index + 1);
            }
        }
        
        if (fieldsIndex == 2) {
            uint16_t addr = fields[0].toInt();
            uint8_t length = fields[1].toInt();
            task = new GrowattReadHoldingTask(node, addr, length);
        }
    }
    
    return task;
}

std::list<String> GrowattTaskFactory::registeredSubtopics() {
    std::list<String> topics;
    
    topics.push_back(F(TOPIC_SETTINGS_PRIORITY));
    topics.push_back(F(TOPIC_SETTINGS_READ_HOLDING_TASK));
    
    return topics;
}
