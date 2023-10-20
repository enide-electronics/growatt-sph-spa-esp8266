/*
  GrowattPriorityTask.h - Library header for the ESP8266/ESP32 Arduino platform
  Changes the inverter priority to Load First, Battery First or Grid First
  
  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/
#include "Task.h"
#include "ModbusMaster.h"

#define TOPIC_SETTINGS_PRIORITY "settings/priority"
#define TOPIC_VALUE_PRIORITY_LOAD "load"
#define TOPIC_VALUE_PRIORITY_BAT "bat"
#define TOPIC_VALUE_PRIORITY_GRID "grid"

class GrowattPriorityTask : public Task {
    /*
     * 0 Load First
     * 1 Bat First
     * 2 Grid First
     */
    private:
        ModbusMaster * node;
        uint16_t priority;
        
    public:
        GrowattPriorityTask(ModbusMaster * node, uint16 priority);
        virtual ~GrowattPriorityTask();

        virtual String subtopic();
        virtual bool run();
};