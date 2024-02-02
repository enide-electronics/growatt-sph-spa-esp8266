/*
  Task.h - Library header for the ESP8266/ESP32 Arduino platform
  Some task to be run on the inverter, invoked by an external entity (MQTT, etc.)
  
  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/
#ifndef TASK_H
#define TASK_H

#include <Arduino.h>
#include "InverterData.h"
#include "GLog.h"

class Task {
    private:
        InverterData responseData;
        bool successful;
        
    protected:
        virtual void setSuccessful(bool successful) {
            this->successful = successful;
        }
        
    public:
        virtual ~Task(){
            responseData.clear();
        };
        
        virtual String subtopic() = 0;
        virtual bool run() = 0;
        
        virtual bool isSuccessful() {
            return successful;
        }

        virtual InverterData& response() {
            return responseData;
        }
};

#endif