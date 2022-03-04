/*
  WifiAndConfigManager.h - Library header for the ESP8266/ESP32 Arduino platform
  SPIFFS based configuration

  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/

#ifndef WIFI_AND_CONFIG_MANAGER_H
#define WIFI_AND_CONFIG_MANAGER_H

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>


class WifiAndConfigManager {
    private:
        WiFiManager wm;
        
        WiFiManagerParameter *deviceNameParam;
        WiFiManagerParameter *softApPasswordParam;
        WiFiManagerParameter *mqttServerParam;
        WiFiManagerParameter *mqttPortParam;
        WiFiManagerParameter *mqttBaseTopicParam;
        WiFiManagerParameter *modbusAddressParam;
        
        String deviceName;
        String softApPassword;
        String mqttServer;
        int mqttPort;
        String mqttBaseTopic;
        int modbusAddress;
        
        bool saveRequired;
        bool rebootRequired;
        
        void load();
        void save();
        void show();
        void saveConfigCallback();
        void handleEraseAll();
        String getParam(String name);

    public:
        WifiAndConfigManager();

        void setupWifiAndConfig();

        String getDeviceName();
        String getMqttServer();
        int getMqttPort();
        String getMqttTopic();
        int getModbusAddress();

        WiFiManager & getWM();
        
        bool checkforConfigChanges();
        bool isRestartRequired();
};

#endif
