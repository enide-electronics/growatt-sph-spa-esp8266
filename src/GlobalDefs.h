/*
  GlobalDefs.h - Library header for the ESP8266/ESP32 Arduino platform
  Macros and similar things
  
  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/
#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

#if defined(ARDUINO_ESP8266_NODEMCU) || defined(ARDUINO_ESP8266_WEMOS_D1MINIPRO) || defined(ARDUINO_ESP8266_WEMOS_D1MINILITE) || defined(ARDUINO_ESP8266_WEMOS_D1MINI)
#define LARGE_ESP_BOARD
#endif

#endif