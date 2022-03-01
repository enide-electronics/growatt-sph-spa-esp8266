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
#include <WiFiManager.h>

#include "GrowattInverter.h"
#include "MqttPublisher.h"
#include "InverterData.h"

// Update wifi settings on settings.h
#include "settings.h"

WiFiClient espClient;

// tasks last run at millis
unsigned long lastReportSentAtMillis = 0;
unsigned long lastTeleSentAtMillis = 0;

// led status (0 = off, 1 = on, 2 = blink when publishing data)
uint8_t ledStatus = 2;

GrowattInverter *inverter;
MqttPublisher *mqtt;
WiFiManager wm;

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

    WiFi.mode(WIFI_STA);

#ifdef BOARD_IS_ESP_01
    wm.setDebugOutput(false); // disable serial debug on ESP-01 boards
#endif

    bool res = wm.autoConnect("growatt-sph-spa-esp8266", "12345678");
    if (!res) {
#ifndef BOARD_IS_ESP_01
        Serial.println("Failed to connect to wifi, restarting...");
#endif
        ESP.restart();
    } else {
        wm.startWebPortal();
    }
    

#ifndef BOARD_IS_ESP_01
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
#endif

    randomSeed(micros());
}

void setupMqtt() {
    mqtt = new MqttPublisher(espClient, MQTT_TOPIC, MQTT_SERVER);
    mqtt->setCallback(callback);
    mqtt->addSubscription(SETTINGS_LED_SUBTOPIC);
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
  
    if (sTopic == String(MQTT_TOPIC) + "/" + SETTINGS_LED_SUBTOPIC) {
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

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
 
    setupInverter();
    setupWifi();
    setupMqtt();
}

void loop() {
    wm.process();
    mqtt->loop();

    unsigned long now = millis();

    // inverter report
    if (now - lastReportSentAtMillis > 5000) {
        if (ledStatus == 2) digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on
        inverter->read();

        if (inverter->isDataValid()) {
            InverterData data = inverter->getData();
            mqtt->publishData(data);
        }

        lastReportSentAtMillis = now;
        if (ledStatus == 2) digitalWrite(BUILTIN_LED, HIGH);   // Turn the LED off
    }

    // inverter tele report
    if (now - lastTeleSentAtMillis > 60000) {
        mqtt->publishTele();
    
        lastTeleSentAtMillis = now;
    }
}
