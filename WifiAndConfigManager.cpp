/*
  WifiAndConfigManager.cpp - Library for the ESP8266/ESP32 Arduino platform
  SPIFFS based configuration

  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/

#include "WifiAndConfigManager.h"
#include <FS.h>
#include "GLog.h"

#define SOFTAP_PASSWORD_K "web_password"
#define MQTT_SERVER_K "mqtt_server"
#define MQTT_PORT_K "mqtt_port"
#define MQTT_TOPIC_K "mqtt_topic"
#define MODBUS_ADDR_K "modbus_addr"
#define BOARD_K "is_board_esp01"

#define DEFAULT_TOPIC "growatt"
#define DEFAULT_SOFTAP_PASSWORD "12345678"
/*
#define BOARD_TYPE_CUSTOM_HTML "<br/><label for=\"boardEsp01\">Board is ESP-01</label><input type=\"checkbox\" name=\"boardEsp01\"";
#define BOARD_TYPE_CUSTOM_HTML_CHECKED_SUFFIX " checked />"
#define BOARD_TYPE_CUSTOM_HTML_UNCHECKED_SUFFIX " />"
 */

WifiAndConfigManager::WifiAndConfigManager() {
    saveRequired = false;
    softApPassword = DEFAULT_SOFTAP_PASSWORD;
    mqttServer = "";
    mqttPort = 1883;
    mqttBaseTopic = DEFAULT_TOPIC;
    modbusAddress = 1;
    boardEsp01 = false;
    
    softApPasswordParam = NULL;
    mqttServerParam = NULL;
    mqttPortParam = NULL;
    mqttBaseTopicParam = NULL;
    modbusAddressParam = NULL;
    boardEsp01Param = NULL;
}

void WifiAndConfigManager::saveConfigCallback() {
    GLOG::println("SAVE REQUIRED");
    saveRequired = true;
}

void WifiAndConfigManager::deleteConfigCallback() {
    GLOG::println("DELETE CONFIG");
    if (SPIFFS.exists("/config.json")) {
        SPIFFS.remove("/config.json");
    }
}

void WifiAndConfigManager::setupWifiAndConfig() {

    load();
    show();

    wm.setDebugOutput(false); // disable serial debug

    // create parameters
    softApPasswordParam = new WiFiManagerParameter("wifipass", "SoftAP Password", softApPassword.c_str(), 32);
    mqttServerParam = new WiFiManagerParameter("server", "MQTT server", mqttServer.c_str(), 40);
    mqttPortParam = new WiFiManagerParameter("port", "MQTT port", String(mqttPort).c_str(), 6);
    mqttBaseTopicParam = new WiFiManagerParameter("topic", "MQTT base topic", mqttBaseTopic.c_str(), 24);
    modbusAddressParam = new WiFiManagerParameter("modbus", "Modbus address", String(modbusAddress).c_str(), 24);
    boardEsp01Param = new WiFiManagerParameter("esp01", "Board type (0 wemos/nodemcu, 1 esp-01)", String(boardEsp01 ? 1 : 0).c_str(), 1);
    /*
    boardEsp01CustomHtml = BOARD_TYPE_CUSTOM_HTML;
    if (boardEsp01) {
        boardEsp01CustomHtml += BOARD_TYPE_CUSTOM_HTML_CHECKED_SUFFIX;
    } else {
        boardEsp01CustomHtml += BOARD_TYPE_CUSTOM_HTML_UNCHECKED_SUFFIX;
    }
    boardEsp01Param = new WiFiManagerParameter(boardEsp01CustomHtml.c_str());
    */

    //set config callbacks
    wm.setSaveConfigCallback(std::bind(&WifiAndConfigManager::saveConfigCallback, this));
    wm.setSaveParamsCallback(std::bind(&WifiAndConfigManager::saveConfigCallback, this));
    wm.setConfigResetCallback(std::bind(&WifiAndConfigManager::deleteConfigCallback, this));
    
    std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart", "exit"};
    wm.setMenu(menu);

    //add all your parameters here
    wm.addParameter(softApPasswordParam);
    wm.addParameter(mqttServerParam);
    wm.addParameter(mqttPortParam);
    wm.addParameter(mqttBaseTopicParam);
    wm.addParameter(modbusAddressParam);
    wm.addParameter(boardEsp01Param);

    WiFi.mode(WIFI_STA);
    WiFi.hostname("growatt-sph-spa-esp8266");
    wm.setHostname("growatt-sph-spa-esp8266");
    
    wm.setConfigPortalTimeout(30); // auto close configportal after n seconds
    wm.setAPClientCheck(true); // avoid timeout if client connected to softap
    wm.setShowInfoUpdate(false); // don't show OTA button on info page

    bool res = wm.autoConnect("growatt-sph-spa-esp8266", softApPassword.c_str());
    if (!res) {
        GLOG::println("Failed to connect to wifi, restarting...");
        ESP.restart();
    } else {
        wm.startWebPortal();
    }

    GLOG::println("");
    GLOG::println("WiFi connected");
    GLOG::println("IP address: ");
    GLOG::println(WiFi.localIP());

    randomSeed(micros());

    // copy values back to our variables
    softApPassword = String(softApPasswordParam->getValue());
    mqttServer = String(mqttServerParam->getValue());
    mqttPort = String(mqttPortParam->getValue()).toInt();
    mqttBaseTopic = String(mqttBaseTopicParam->getValue());
    modbusAddress = String(modbusAddressParam->getValue()).toInt();
    boardEsp01 = String(boardEsp01Param->getValue()).toInt() > 0;

    // now save it to the SPIFFS file
    if (saveRequired) {
        save();
        saveRequired = false;
    }
}

void WifiAndConfigManager::load() {
    //read configuration from FS json
    GLOG::println("mounting FS...");

    if (SPIFFS.begin()) {
        GLOG::println("mounted file system");
        if (SPIFFS.exists("/config.json")) {
            //file exists, reading and loading
            GLOG::println("reading config file");
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile) {
                GLOG::println("opened config file");
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

                    if (json.containsKey(BOARD_K)) {
                        boardEsp01 = json[BOARD_K] > 0;
                    } else {
                        boardEsp01 = false;
                    }
                } else {
                    GLOG::println("failed to load json config");
                }
                configFile.close();
            }
        }
    } else {
        GLOG::println("failed to mount FS");
    }
    //end read

}

void WifiAndConfigManager::save() {
    //save the custom parameters to FS

    GLOG::println("saving config");

#if ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(1024);
#else
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
#endif

    json[SOFTAP_PASSWORD_K] = softApPassword.c_str();
    json[MQTT_SERVER_K] = mqttServer.c_str();
    json[MQTT_PORT_K] = mqttPort;
    json[MQTT_TOPIC_K] = mqttBaseTopic.c_str();
    json[MODBUS_ADDR_K] = modbusAddress;
    json[BOARD_K] = boardEsp01 ? 1 : 0;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
        GLOG::println("failed to open config file for writing");
    }

#if ARDUINOJSON_VERSION_MAJOR >= 6
    serializeJson(json, configFile);
#else
    json.printTo(configFile);
#endif
    configFile.close();

    //end save
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
    GLOG::print("ESP01         : ");
    GLOG::println(boardEsp01);
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

bool WifiAndConfigManager::isBoardEsp01() {
    return boardEsp01;
}


WiFiManager & WifiAndConfigManager::getWM() {
    return wm;
}

bool WifiAndConfigManager::checkforConfigUpdate() {
    if (saveRequired) {
        
        // copy values back to our variables
        softApPassword = String(softApPasswordParam->getValue());
        mqttServer = String(mqttServerParam->getValue());
        mqttPort = String(mqttPortParam->getValue()).toInt();
        mqttBaseTopic = String(mqttBaseTopicParam->getValue());
        modbusAddress = String(modbusAddressParam->getValue()).toInt();
        boardEsp01 = String(boardEsp01Param->getValue()).toInt() > 0;
        //boardEsp01 = getParam("boardEsp01") == "on";
        
        save();
        saveRequired = false;
        show();
        
        return true;
    } else {
        return false;
    }
}

