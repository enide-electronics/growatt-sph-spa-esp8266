/*
  MqttPublisher.h - Library header for the ESP8266/ESP32 Arduino platform
  Publish Growatt SPH and SPA data to MQTT
  
  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/

#ifndef _MQTT_PUBLISHER_H
#define _MQTT_PUBLISHER_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "InverterData.h"

class MqttPublisher {
    private:
        PubSubClient *client;
        String topic;
        String clientId;
        std::vector<String> subscriptions;
        
        void keepConnected();
        
    public:
        MqttPublisher(WiFiClient &espClient, const char *baseTopic, const char *server);
       
        void publishData(InverterData &data);
        void publishTele();
        
        void setClientId(String &clientId);
        void setCallback(void (*callback)(char* topic, byte* payload, unsigned int length));
        void addSubscription(const char *subtopic);

        void loop();
};

#endif