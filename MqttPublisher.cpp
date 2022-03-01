/*
  MqttPublisher.cpp - Library for the ESP8266/ESP32 Arduino platform
  Publish Growatt SPH and SPA data to MQTT
  
  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/

#include "MqttPublisher.h"
        
MqttPublisher::MqttPublisher(WiFiClient &espClient, const char *baseTopic, const char *server) {
    this->client = new PubSubClient(espClient);
    this->client->setServer(server, 1883);  
    
    this->topic = baseTopic;
    this->clientId = "unknown";
}
       
void MqttPublisher::publishData(InverterData &data) {
    for (std::map<String, String>::iterator it = data.begin(); it != data.end(); ++it) {
        const String & name = it->first;
        const String & value = it->second;
        
        client->publish((topic + String("/") + name).c_str(), value.c_str());
    }
}

void MqttPublisher::publishTele() {
    client->publish((topic + "/tele/IP").c_str(), WiFi.localIP().toString().c_str());
    client->publish((topic + "/tele/ClientID").c_str(), clientId.c_str());
}


void MqttPublisher::setClientId(String &clientId) {
    this->clientId = clientId;
}


void MqttPublisher::setCallback(void (*callback)(char* topic, byte* payload, unsigned int length)) {
    client->setCallback(callback);
}

void MqttPublisher::addSubscription(const char *subtopic) {
    subscriptions.push_back(this->topic + "/" + subtopic);
}

void MqttPublisher::keepConnected() {
    // Loop until we're reconnected
    while (!client->connected()) {
#ifndef BOARD_IS_ESP_01
        Serial.print("Attempting MQTT connection...");
#endif

        // Create a random client ID
        clientId = "growatt-";
        clientId += String(random(0xffff), HEX);
    
        // Attempt to connect
        if (client->connect(clientId.c_str())) {
#ifndef BOARD_IS_ESP_01
            Serial.println("connected");
#endif      
            // Once connected, publish an announcement...
            publishTele();
      
            // ... and resubscribe
            for (String s : subscriptions) {
                client->subscribe(s.c_str());
            }
        } else {
#ifndef BOARD_IS_ESP_01
            Serial.print("failed, rc=");
            Serial.print(client->state());
            Serial.println(" try again in 5 seconds");
#endif
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void MqttPublisher::loop() {
    keepConnected();
    client->loop();
}



