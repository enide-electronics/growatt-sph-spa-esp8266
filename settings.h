#ifndef SETTINGS_H
#define SETTINGS_H

// Update these with values suitable for your network.

#define SSID "SSID"
#define PASSWORD "PASSWORD"
#define MQTT_SERVER "IP_ADDRESS"

// Inverter address on modbus, usually 1
#define MODBUS_INVERTER_SLAVE_ADDR 1

// base topic name (prefix)
#define MQTT_TOPIC "growatt"
/*
 * You can set the ESP8266 LED working mode by publishing a value to this topic
 * 0: LED always off
 * 1: LED always on
 * 2: LED blinks when reading data from the inverter and publishing it to MQTT (default)
 */
#define SETTINGS_LED_SUBTOPIC "settings/led"

// uncomment this line to remove serial debug and use an ESP-01 board (WiFi232 modem or similar clone)
//#define BOARD_IS_ESP_01 1

#endif