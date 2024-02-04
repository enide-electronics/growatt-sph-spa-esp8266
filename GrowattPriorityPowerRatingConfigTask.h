/*
  GrowattPriorityPowerRatingConfigTask.h - Library header for the ESP8266/ESP32 Arduino platform
  This task set the Power Rating value for Grid First or Battery First
  It answers on: 
    "settings/priority/bat/pr"
    "settings/priority/grid/pr"
    
  The payload should be in integer format:
  1..100
  
  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/
#ifndef GROWATT_PRIORITY_POWER_RATING_CONFIG_TASK_H
#define GROWATT_PRIORITY_POWER_RATING_CONFIG_TASK_H

#include "GrowattPriorityConfigSetOneRegisterTask.h"

class GrowattPriorityPowerRatingConfigTask : public GrowattPriorityConfigSetOneRegisterTask {
    protected:
        virtual uint16_t getAddress() const;
        virtual bool isValid(uint16_t value) const;
        
    public:
        GrowattPriorityPowerRatingConfigTask(ModbusMaster * node, const String &priorityName, const String &mqttPayload);
        virtual ~GrowattPriorityPowerRatingConfigTask();
};

#endif