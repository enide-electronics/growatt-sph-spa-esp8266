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

#include "WifiAndConfigManager.h"
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

WiFiClient espClient;

// tasks last run at millis
unsigned long lastReportSentAtMillis = 0;
unsigned long lastTeleSentAtMillis = 0;

// led status (0 = off, 1 = on, 2 = blink when publishing data)
uint8_t ledStatus = 2;

GrowattInverter *inverter = NULL;
SoftwareSerial *_softSerial = NULL;
MqttPublisher *mqtt = NULL;
WifiAndConfigManager wcm;


void mqttCallback(char* topic, byte* payload, unsigned int length) {
    GLOG::print("Message arrived [");
    GLOG::print(topic);
    GLOG::print("] ");
    for (unsigned int i = 0; i < length; i++) {
        GLOG::print((char)payload[i]);
    }
    GLOG::println("");

    String sTopic(topic);

    if (sTopic == wcm.getMqttTopic() + "/" + SETTINGS_LED_SUBTOPIC) {
        // Switch on the LED if an 1 was received as first character
        char cLedStatus = (char)payload[0];
        if (cLedStatus == '1') {
            digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
            ledStatus = 1;                    // but actually the LED is on; this is because
                                        // it is active low on the ESP-01)
        } else if (cLedStatus == '0') {
            digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
            ledStatus = 0;
        } else if (cLedStatus == '2') {
            digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
            ledStatus = 2;
        }
    }
}

void setupInverter() {
    String arduinoBoard = String(ARDUINO_BOARD);
    if (arduinoBoard == "ESP8266_NODEMCU" || arduinoBoard == "ESP8266_WEMOS_D1MINIPRO" || arduinoBoard =="ESP8266_WEMOS_D1MINILITE") {
        #if !defined(D5) || !defined(D6)
        #define D5 14 //pin mapping of nodemcu and d1_mini
        #define D6 12
        #endif
        
        #define PIN_RX D6
        #define PIN_TX D5
        _softSerial = new SoftwareSerial(PIN_RX, PIN_TX);
        _softSerial->begin(9600);
        inverter = new GrowattInverter((Stream &) (*_softSerial), wcm.getModbusAddress());
    } else {
        Serial.begin(9600);
        inverter = new GrowattInverter((Stream &) Serial, wcm.getModbusAddress());
    }
}

void setupMqtt() {
    mqtt = new MqttPublisher(espClient, wcm.getMqttTopic().c_str(), wcm.getMqttServer().c_str(), wcm.getMqttPort());
    mqtt->setCallback(mqttCallback);
    mqtt->addSubscription(SETTINGS_LED_SUBTOPIC);
}

void setupLogger() {
    GLOG::setup();
    wcm.getWM().setDebugOutput(GLOG::isLogEnabled());
}

void applyNewConfiguration() {
    delay(1000);
    
    // delete old objects
    delete inverter;
    delete mqtt;
    espClient.stop();
    
    if (_softSerial) {
        delete _softSerial;
        _softSerial = NULL;
    }
    
    // set them up again
    setupLogger();
    setupInverter();
    setupMqtt();
}

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

    setupLogger();
    wcm.setupWifiAndConfig();
    setupInverter();
    setupMqtt();
}

void loop() {
    wcm.getWM().process(); // wm web config portal
    
    // handle config changes
    if (wcm.checkforConfigChanges()) {
        if (wcm.isRestartRequired()) {
            delay(1000);
            ESP.restart();
        } else {
            applyNewConfiguration();
        }
    }
    
    mqtt->loop();

    unsigned long now = millis();

    // inverter report
    if (mqtt->isConnected() && now - lastReportSentAtMillis > 5000) {
        if (ledStatus == 2) digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
        inverter->read();

        if (inverter->isDataValid()) {
            InverterData data = inverter->getData();
            mqtt->publishData(data);
        }

        lastReportSentAtMillis = now;
        if (ledStatus == 2) digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED off
    }

    // inverter tele report
    if (now - lastTeleSentAtMillis > 60000) {
        mqtt->publishTele();

        lastTeleSentAtMillis = now;
    }
}
