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

#include "GlobalDefs.h"
#include "Leds.h"
#include "WifiAndConfigManager.h"
#include "Inverter.h"
#include "GrowattInverter.h"
#include "MqttPublisher.h"
#include "InverterData.h"
#include "GLog.h"

/*
 * You can set the ESP8266 LED working mode by publishing a value to this topic
 * 0: LED always off
 * 1: LED always on
 * 2: LED blinks when reading data from the inverter and publishing it to MQTT (default)
 */
#define SETTINGS_LED_SUBTOPIC "settings/led"
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

WiFiClient espClient;
Leds leds;

// tasks last run at millis
unsigned long lastReportSentAtMillis = 0;
unsigned long lastTeleSentAtMillis = 0;
unsigned long lastWifiCheckAtMillis = 0;

// led status (0 = off, 1 = on, 2 = blink when publishing data)
uint8_t ledStatus = 2;
char mqttValueBuffer16[16];
uint8_t tasksRedLedCounter = 0;

Inverter *inverter = NULL;
SoftwareSerial *_softSerial = NULL;
MqttPublisher *mqtt = NULL;
WifiAndConfigManager wcm;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    GLOG::logMqtt(topic, payload, length);

    String subTopic(topic);
    subTopic.replace(wcm.getMqttTopic() + "/", "");

    if (subTopic == SETTINGS_LED_SUBTOPIC) {
        // Switch on the LED if an 1 was received as first character
        char cLedStatus = (char)payload[0];
        if (cLedStatus == '1') {
            leds.lightUpDefault();   // Turn the LED on 
            ledStatus = 1;
        } else if (cLedStatus == '0') {
            leds.turnOffDefault();  // Turn the LED off
            ledStatus = 0;
        } else if (cLedStatus == '2') {
            leds.dimDefault();  // Dim the LED 
            ledStatus = 2;
        }
    } else {
        leds.lightUpRed(); // RED lights up
        tasksRedLedCounter++;
        
        int safeLength = MIN(length, 15);
        memcpy(mqttValueBuffer16, payload, safeLength);
        mqttValueBuffer16[safeLength] = '\0';
        
        String sPayload = String(mqttValueBuffer16);
        sPayload.trim();
        inverter->setIncomingTopicData(subTopic, sPayload);
    }
}

void setupInverter() {
#ifdef LARGE_ESP_BOARD
    #define PIN_RX D6
    #define PIN_TX D5
    _softSerial = new SoftwareSerial(PIN_RX, PIN_TX);
    _softSerial->begin(9600);
    inverter = new GrowattInverter((Stream &) (*_softSerial), wcm.getModbusAddress());
#else
    Serial.begin(9600);
    inverter = new GrowattInverter((Stream &) Serial, wcm.getModbusAddress());
#endif
}

void setupMqtt(std::list<String> inverterSettingsTopics) {
    mqtt = new MqttPublisher(espClient, wcm.getMqttUsername().c_str(), wcm.getMqttPassword().c_str(), wcm.getMqttTopic().c_str(), wcm.getMqttServer().c_str(), wcm.getMqttPort());
    mqtt->setCallback(mqttCallback);
    mqtt->addSubscription(SETTINGS_LED_SUBTOPIC);
    
    for (std::list<String>::iterator it = inverterSettingsTopics.begin(); it != inverterSettingsTopics.end(); ++it) {
        mqtt->addSubscription((*it).c_str());
    }
    
}

void setupLogger() {
    GLOG::setup();
    wcm.getWM().setDebugOutput(GLOG::isLogEnabled());
}

void applyNewConfiguration() {
    delay(1000);
    
    GLOG::println(F("LOOP: New config, no restart... deleting old objects"));
    
    // delete old objects
    delete mqtt;
    delete inverter;
    espClient.stop();
    
    if (_softSerial) {
        delete _softSerial;
        _softSerial = NULL;
    }
    
    GLOG::println(F("LOOP: New config, no restart... creating new objects"));
    
    // set them up again
    setupLogger();
    setupInverter();
    setupMqtt(inverter->getTopicsToSubscribe());
}

void setup() {

    setupLogger();
    wcm.setupWifiAndConfig();
    setupInverter();
    setupMqtt(inverter->getTopicsToSubscribe());
}

void loop() {
    wcm.getWM().process(); // wm web config portal

    // handle config changes
    if (wcm.checkforConfigChanges()) {
        if (wcm.isRestartRequired()) {
            GLOG::println(F("LOOP: New config, RESTARTING!"));
            delay(1000);
            ESP.restart();
        } else {
            applyNewConfiguration();
        }
    }
    
    mqtt->loop();

    unsigned long now = millis();

    // inverter report
    if (mqtt->isConnected() && now - lastReportSentAtMillis > wcm.getModbusPollingInSeconds() * (unsigned)1000) {
        if (ledStatus == 2) leds.lightUpDefault(); // Turn the LED on
        GLOG::print(F("LOOP: Polling inverter"));
        inverter->read();

        if (inverter->isDataValid()) {
            GLOG::print(F(", publishing"));
            InverterData data = inverter->getData();
            mqtt->publishData(data);
            GLOG::println(F(", done!"));
        } else {
            GLOG::println(F(", failed!"));
        }

        lastReportSentAtMillis = now;
        
        if (ledStatus == 2) leds.dimDefault(); // Turn the LED off
        if (tasksRedLedCounter > 0) tasksRedLedCounter--;
        if (tasksRedLedCounter == 0) leds.dimRed(); // Dim RED led
    }

    // inverter tele report
    if (now - lastTeleSentAtMillis > 60000) {
        GLOG::println(F("LOOP: Publishing telemetry"));
        mqtt->publishTele();

        lastTeleSentAtMillis = now;
    }

    if (now - lastWifiCheckAtMillis > 5000) {
        if (!wcm.isWifiConnected()) {
            leds.turnOffDefault(); // LED should recover once Wifi reconnects
        }
    
        lastTeleSentAtMillis = now;
    }
}
