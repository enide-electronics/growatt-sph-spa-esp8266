/*
  WifiAndConfigManager.cpp - Library for the ESP8266/ESP32 Arduino platform
  SPIFFS based configuration

  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/

#include "WifiAndConfigManager.h"
#include <FS.h>
#include "GLog.h"

#define DEVICE_NAME_K "device_name"
#define SOFTAP_PASSWORD_K "web_password"
#define MQTT_SERVER_K "mqtt_server"
#define MQTT_PORT_K "mqtt_port"
#define MQTT_TOPIC_K "mqtt_topic"
#define MODBUS_ADDR_K "modbus_addr"
#define MODBUS_POLLING_K "modbus_poll_secs"

#define DEFAULT_TOPIC "growatt"
#define DEFAULT_SOFTAP_PASSWORD "12345678"
#define DEFAULT_DEVICE_NAME "growatt-sph-spa-esp8266"

WifiAndConfigManager::WifiAndConfigManager() {
    saveRequired = false;
    rebootRequired = false;
    
    // config vars
    deviceName = DEFAULT_DEVICE_NAME;
    softApPassword = DEFAULT_SOFTAP_PASSWORD;
    mqttServer = "localhost";
    mqttPort = 1883;
    mqttBaseTopic = DEFAULT_TOPIC;
    modbusAddress = 1;
    modbusPollingInSeconds = 5;
    
    // config var web params
    deviceNameParam = NULL;
    softApPasswordParam = NULL;
    mqttServerParam = NULL;
    mqttPortParam = NULL;
    mqttBaseTopicParam = NULL;
    modbusAddressParam = NULL;
}

void WifiAndConfigManager::saveConfigCallback() {
    GLOG::println("SAVE IS REQUIRED");
    saveRequired = true;
}

void WifiAndConfigManager::handleEraseAll() {
    GLOG::println("DELETE SPIFFS CONFIG");
    if (SPIFFS.exists("/config.json")) {
        SPIFFS.remove("/config.json");
    }

    GLOG::println("DELETE WIFI CONFIG");
    ESP.eraseConfig();
    
    wm.server->send(200, F("text/plain"), F("Done! Rebooting now, please wait a few seconds."));
    
    delay(2000);
    
    ESP.restart();
}

void WifiAndConfigManager::setupWifiAndConfig() {

    load();
    show();

    // create parameters
    deviceNameParam = new WiFiManagerParameter("devicename", "Device Name", deviceName.c_str(), 32);
    softApPasswordParam = new WiFiManagerParameter("wifipass", "SoftAP Password", softApPassword.c_str(), 32);
    mqttServerParam = new WiFiManagerParameter("server", "MQTT server", mqttServer.c_str(), 40);
    mqttPortParam = new WiFiManagerParameter("port", "MQTT port", String(mqttPort).c_str(), 6);
    mqttBaseTopicParam = new WiFiManagerParameter("topic", "MQTT base topic", mqttBaseTopic.c_str(), 24);
    modbusAddressParam = new WiFiManagerParameter("modbus", "Modbus address", String(modbusAddress).c_str(), 3);
    modbusPollingInSecondsParam = new WiFiManagerParameter("modbuspoll", "Modbus polling (secs)", String(modbusPollingInSeconds).c_str(), 3);

    //set config callbacks
    wm.setSaveConfigCallback(std::bind(&WifiAndConfigManager::saveConfigCallback, this));
    wm.setSaveParamsCallback(std::bind(&WifiAndConfigManager::saveConfigCallback, this));
    
    std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart", "exit"};
    wm.setMenu(menu);

    //add all your parameters here
    wm.addParameter(deviceNameParam);
    wm.addParameter(softApPasswordParam);
    wm.addParameter(mqttServerParam);
    wm.addParameter(mqttPortParam);
    wm.addParameter(mqttBaseTopicParam);
    wm.addParameter(modbusAddressParam);

    WiFi.mode(WIFI_STA);
    WiFi.hostname(deviceName.c_str());
    wm.setHostname(deviceName.c_str());
    
    wm.setConfigPortalTimeout(60); // auto close configportal after n seconds
    wm.setAPClientCheck(true); // avoid timeout if client connected to softap
    wm.setShowInfoUpdate(false); // don't show OTA button on info page
    

    bool res = wm.autoConnect(deviceName.c_str(), softApPassword.c_str());
    if (!res) {
        GLOG::println("Failed to connect to wifi, restarting...");
        delay(1000);
        
        ESP.restart();
    } else {
        wm.startWebPortal();
        wm.server->on((String(FPSTR("/eraseall")).c_str()), std::bind(&WifiAndConfigManager::handleEraseAll, this));
    }

    GLOG::println("");
    GLOG::println("WiFi connected");
    GLOG::print("IP address: ");
    GLOG::println(WiFi.localIP());

    randomSeed(micros());

    copyFromParamsToVars();

    // now save it to the SPIFFS file
    if (saveRequired) {
        save();
        saveRequired = false;
    }
}

void WifiAndConfigManager::load() {
    //read configuration from FS json
    GLOG::println("Mounting FS...");

    if (SPIFFS.begin()) {
        GLOG::println("Mounted file system");
        if (SPIFFS.exists("/config.json")) {
            //file exists, reading and loading
            GLOG::println("Reading config file");
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile) {
                GLOG::println("Opened config file");
                size_t size = configFile.size();
                // Allocate a buffer to store contents of the file.
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);

#if ARDUINOJSON_VERSION_MAJOR >= 6
                DynamicJsonDocument json(1024);
                auto deserializeError = deserializeJson(json, buf.get());
                
                if ( ! deserializeError ) {
#else
                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject(buf.get());
                
                if (json.success()) {
#endif
                    GLOG::println("\nparsed json");

                    if (json.containsKey(DEVICE_NAME_K)) {
                        deviceName = json[DEVICE_NAME_K].as<String>();
                    } else {
                        deviceName = DEFAULT_DEVICE_NAME;
                    }
                    
                    if (json.containsKey(SOFTAP_PASSWORD_K)) {
                        softApPassword = json[SOFTAP_PASSWORD_K].as<String>();
                    } else {
                        softApPassword = DEFAULT_SOFTAP_PASSWORD;
                    }
                    
                    if (json.containsKey(MQTT_SERVER_K)) {
                        mqttServer = json[MQTT_SERVER_K].as<String>();
                    } else {
                        mqttServer = "";
                    }

                    if (json.containsKey(MQTT_PORT_K)) {
                        mqttPort = json[MQTT_PORT_K];
                    } else {
                        mqttPort = 1883;
                    }

                    if (json.containsKey(MQTT_TOPIC_K)) {
                        mqttBaseTopic = json[MQTT_TOPIC_K].as<String>();
                    } else {
                        mqttBaseTopic = DEFAULT_TOPIC;
                    }

                    if (json.containsKey(MODBUS_ADDR_K)) {
                        modbusAddress = json[MODBUS_ADDR_K];
                    } else {
                        modbusAddress = 1;
                    }
                    
                    if (json.containsKey(MODBUS_POLLING_K)) {
                        modbusPollingInSeconds = json[MODBUS_POLLING_K];
                    } else {
                        modbusPollingInSeconds = 5;
                    }
                } else {
                    GLOG::println("Failed to load json config");
                }
                configFile.close();
            }
        }
    } else {
        GLOG::println("Failed to mount FS");
    }
    //end read

}

void WifiAndConfigManager::save() {
    //save the custom parameters to FS

    GLOG::println("Saving config");

#if ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(1024);
#else
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
#endif

    json[DEVICE_NAME_K] = deviceName.c_str();
    json[SOFTAP_PASSWORD_K] = softApPassword.c_str();
    json[MQTT_SERVER_K] = mqttServer.c_str();
    json[MQTT_PORT_K] = mqttPort;
    json[MQTT_TOPIC_K] = mqttBaseTopic.c_str();
    json[MODBUS_ADDR_K] = modbusAddress;
    json[MODBUS_POLLING_K] = modbusPollingInSeconds;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
        GLOG::println("Failed to open config file for writing");
    }

#if ARDUINOJSON_VERSION_MAJOR >= 6
    serializeJson(json, configFile);
#else
    json.printTo(configFile);
#endif
    configFile.close();

    //end save
}

void WifiAndConfigManager::copyFromParamsToVars() {
    // copy values back to our variables
    deviceName = String(deviceNameParam->getValue());
    softApPassword = String(softApPasswordParam->getValue());
    mqttServer = String(mqttServerParam->getValue());
    mqttPort = String(mqttPortParam->getValue()).toInt();
    mqttBaseTopic = String(mqttBaseTopicParam->getValue());
    modbusAddress = String(modbusAddressParam->getValue()).toInt();
    modbusPollingInSeconds = String(modbusPollingInSecondsParam->getValue()).toInt();
}

String WifiAndConfigManager::getParam(String name){
    //read parameter from server, for custom hmtl input
    String value;
    if(wm.server->hasArg(name)) {
        value = wm.server->arg(name);
    }
    return value;
}

void WifiAndConfigManager::show() {
    GLOG::println("---------------------------");
    GLOG::print("Device name   : ");
    GLOG::println(deviceName);
    
    GLOG::print("SoftAP pass   : ");
    GLOG::println(softApPassword);
    
    GLOG::print("Mqtt server   : ");
    GLOG::println(mqttServer);
 
    GLOG::print("Mqtt port     : ");
    GLOG::println(mqttPort);
    
    GLOG::print("Mqtt Topic    : ");
    GLOG::println(mqttBaseTopic);
    
    GLOG::print("Modbus Address: ");
    GLOG::println(modbusAddress);
    
    GLOG::print("Modbus Poll(s): ");
    GLOG::println(modbusPollingInSeconds);
}

String WifiAndConfigManager::getDeviceName() {
    return deviceName;
}

String WifiAndConfigManager::getMqttServer() {
    return mqttServer;
}

int WifiAndConfigManager::getMqttPort() {
    return mqttPort;
}

String WifiAndConfigManager::getMqttTopic() {
    return mqttBaseTopic;
}

int WifiAndConfigManager::getModbusAddress() {
    return modbusAddress;
}

int WifiAndConfigManager::getModbusPollingInSeconds() {
    return modbusPollingInSeconds;
}

WiFiManager & WifiAndConfigManager::getWM() {
    return wm;
}

bool WifiAndConfigManager::checkforConfigChanges() {
    if (saveRequired) {
        
        String newDeviceName = String(deviceNameParam->getValue());
        if (newDeviceName != deviceName) {
            GLOG::println(String("New device name : ") + newDeviceName);
            rebootRequired = true;
        }
        
        copyFromParamsToVars();
        
        save();
        saveRequired = false;
        
        show();
        
        return true;
    } else {
        return false;
    }
}

bool WifiAndConfigManager::isRestartRequired() {
    return rebootRequired;
}

