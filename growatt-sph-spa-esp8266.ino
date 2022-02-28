/*
 Growatt SPH/SPA inverter data exporter
 Uses Modbus to communicate with the inverter's serial RS232 port
 Publishes data to a MQTT server
 
 This file is based on the "Basic ESP8266 MQTT example"
 
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 board in "Tools -> Board"
    This version of the code runs fine on a Wemos D1 or NodeMCU v3 (ESP-12) board

 Also add the following Libraries:
  - ModbusMaster 2.0.1+
  - PubSubClient 2.8.0+
  
  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/

#include <stdio.h>

#include <Arduino.h>
#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "GrowattInverter.h"

// Update wifi settings on settings.h
#include "settings.h"

WiFiClient espClient;
PubSubClient client(espClient);

// incoming topic deserialier buffer
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
long value = 0;

// tasks last run at millis
unsigned long lastReportSentAtMillis = 0;
unsigned long lastTeleSentAtMillis = 0;

// mqtt client id
String clientId;

// led status (0 = off, 1 = on, 2 = blink when publishing data)
uint8_t ledStatus = 2;

#define LOCAL_IP_MAX_SIZE 16
char localIpAddress[LOCAL_IP_MAX_SIZE];

GrowattInverter *inverter;

void setupInverter() {
#ifdef BOARD_IS_ESP_01
    Serial.begin(9600);
    inverter = new GrowattInverter((Stream &) Serial, MODBUS_INVERTER_SLAVE_ADDR);
#else
    #define PIN_RX D6
    #define PIN_TX D5
    Serial.begin(115200); // for debug information
    SoftwareSerial *_softSerial = new SoftwareSerial(PIN_RX, PIN_TX);
    _softSerial->begin(9600);
    inverter = new GrowattInverter((Stream &) (*_softSerial), MODBUS_INVERTER_SLAVE_ADDR);
#endif
}

void setupWifi() {

    delay(10);
    // We start by connecting to a WiFi network

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

#ifndef BOARD_IS_ESP_01
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
#endif

    randomSeed(micros());
}

void callback(char* topic, byte* payload, unsigned int length) {
#ifndef BOARD_IS_ESP_01
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
#endif

    String sTopic(topic);
  
    if (sTopic == SETTINGS_LED_TOPIC) {
        // Switch on the LED if an 1 was received as first character
        char cLedStatus = (char)payload[0];
        if (cLedStatus == '1') {
            digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
            ledStatus = 1;                    // but actually the LED is on; this is because
                                        // it is active low on the ESP-01)
        } else if (cLedStatus == '0') {
            digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
            ledStatus = 0;
        } else if (cLedStatus == '2') {
            digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
            ledStatus = 2;
        }
    }
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
#ifndef BOARD_IS_ESP_01
        Serial.print("Attempting MQTT connection...");
#endif

        // Create a random client ID
        clientId = "growatt-";
        clientId += String(random(0xffff), HEX);
    
        // Attempt to connect
        if (client.connect(clientId.c_str())) {
#ifndef BOARD_IS_ESP_01
            Serial.println("connected");
#endif      
            // Once connected, publish an announcement...
            reportTele();
      
            // ... and resubscribe
            client.subscribe(SETTINGS_LED_TOPIC);
        } else {
#ifndef BOARD_IS_ESP_01
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
#endif
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void publish(const char *topicSuffix, float value) {
    String fullTopic = "growatt/";
    fullTopic += topicSuffix;
    
    snprintf (msg, MSG_BUFFER_SIZE, "%.1f", value);
    
    client.publish(fullTopic.c_str(), msg);
}

void publish(const char *topicSuffix, uint16_t value) {
    String fullTopic = "growatt/";
    fullTopic += topicSuffix;
    
    snprintf (msg, MSG_BUFFER_SIZE, "%d", value);
    
    client.publish(fullTopic.c_str(), msg);
}

void publish(const char *topicSuffix, uint8_t value) {
    String fullTopic = "growatt/";
    fullTopic += topicSuffix;
    
    snprintf (msg, MSG_BUFFER_SIZE, "%d", value);
    
    client.publish(fullTopic.c_str(), msg);
}

void publish(const char *topicSuffix, const char * value) {
    String fullTopic = "growatt/";
    fullTopic += topicSuffix;
    
    strncpy (msg, value, MSG_BUFFER_SIZE);
    
    client.publish(fullTopic.c_str(), msg);
}

void reportTele() {
    client.publish("growatt/tele/IP", WiFi.localIP().toString().c_str());
    client.publish("growatt/tele/ClientID", clientId.c_str());
}

void report() {
    publish("status", inverter->status);
  
    publish("Ppv1", inverter->Ppv1);    
    publish("Vpv1", inverter->Vpv1);
    publish("Ipv1", inverter->Ipv1);
  
    publish("Ppv2", inverter->Ppv2);    
    publish("Vac1", inverter->Vac1);
    publish("Iac1", inverter->Iac1);
    publish("Pac1", inverter->Pac1);
  
    publish("Pac", inverter->Pac);
    publish("Fac", inverter->Fac);
  
  
    publish("Etoday", inverter->Etoday);
    publish("Etotal", inverter->Etotal);
    publish("Ttotal", inverter->Ttotal);
  
    publish("Temp1", inverter->temp1);
    publish("Temp2", inverter->temp2);
    publish("Temp3", inverter->temp3);
  
    switch (inverter->Priority) {
        case 0:
            publish("Priority", "Load First");
        break;
        case 1:
            publish("Priority", "Bat First");
        break;
        case 2:
            publish("Priority", "Grid First");
        break;
        default:
            publish("Priority", (String("Unknown ") + inverter->Priority).c_str());
    }

    switch (inverter->BatteryType) {
        case 0:
            publish("Battery", "LeadAcid");
        break;
        
        case 1:
            publish("Battery", "Lithium");
        break;
        default:
            publish("Battery", (String("Unknown type ") + inverter->BatteryType).c_str());
    }

    publish("Pdischarge", inverter->Pdischarge);
    publish("Pcharge", inverter->Pcharge);
    publish("Vbat", inverter->Vbat);
    publish("SOC", inverter->SOC);

    publish("EpsFac", inverter->EpsFac);
    publish("EpsPac1", inverter->EpsPac1);
    publish("EpsVac1", inverter->EpsVac1);
    publish("EpsIac1", inverter->EpsIac1);
    publish("EpsLoadPercent", inverter->EpsLoadPercent);
    publish("EpsPF", inverter->EpsPF);
}


void setup() {
    pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    setupWifi();
    
    setupInverter();

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    unsigned long now = millis();

    // inverter report
    if (now - lastReportSentAtMillis > 5000) {
        if (ledStatus == 2) digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on
        inverter->read();

        if (inverter->valid) {
            report();
        }

        lastReportSentAtMillis = now;
        if (ledStatus == 2) digitalWrite(BUILTIN_LED, HIGH);   // Turn the LED off
}

    // inverter tele report
    if (now - lastTeleSentAtMillis > 60000) {
        reportTele();
    
        lastTeleSentAtMillis = now;
    }
}
