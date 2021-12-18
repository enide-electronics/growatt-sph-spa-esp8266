#ifndef SETTINGS_H
#define SETTINGS_H

// Update these with values suitable for your network.

const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* mqtt_server = "IP_ADDRESS";

/*
 * You can set the ESP8266 LED working mode by publishing a value to this topic
 * 0: LED always off
 * 1: LED always on (default)
 * 2: LED blinks when reading data from the inverter and publishing it to MQTT
 */
#define SETTINGS_LED_TOPIC "growatt/settings/led"

#endif