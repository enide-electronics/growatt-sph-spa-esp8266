/*
  GLog.cpp - Library for the ESP8266/ESP32 Arduino platform
  Global logging solution (used to prevent logging to the Serial interface)

  Written by JF enide.electronics (at) enide.net
  Licensed under GNU GPLv3
*/

#include "GLog.h"

Stream *GLOG::s = NULL;

void GLOG::println(unsigned char v) {
    if (GLOG::s) {
        GLOG::s->println(v);
    }
}

void GLOG::print(unsigned char v) {
    if (GLOG::s) {
        GLOG::s->print(v);
    }
}

void GLOG::println(char v) {
    if (GLOG::s) {
        GLOG::s->println(v);
    }
}

void GLOG::print(char v) {
    if (GLOG::s) {
        GLOG::s->print(v);
    }
}

void GLOG::println(int v) {
    if (GLOG::s) {
        GLOG::s->println(v);
    }
}

void GLOG::print(int v) {
    if (GLOG::s) {
        GLOG::s->print(v);
    }
}

void GLOG::println(const char * msg) {
    if (GLOG::s) {
        GLOG::s->println(msg);
    }
}

void GLOG::print(const char * msg) {
    if (GLOG::s) {
        GLOG::s->print(msg);
    }
}

void GLOG::println(const Printable &o) {
    if (GLOG::s) {
        GLOG::s->println(o);
    }
}

void GLOG::print(const Printable &o) {
    if (GLOG::s) {
        GLOG::s->print(o);
    }
}

void GLOG:: println(const String &o) {
    if (GLOG::s) {
        GLOG::s->println(o);
    }
}

void GLOG::print(const String &o) {
    if (GLOG::s) {
        GLOG::s->print(o);
    }
}
    
void GLOG::setup() {
    String arduinoBoard = String(ARDUINO_BOARD);
    if (arduinoBoard == "ESP8266_NODEMCU" || arduinoBoard == "ESP8266_WEMOS_D1MINIPRO" 
        || arduinoBoard =="ESP8266_WEMOS_D1MINILITE" || arduinoBoard =="ESP8266_WEMOS_D1MINI") {
        Serial.begin(115200);
        delay(10);
        GLOG::s = &Serial;
    } else {
        GLOG::s = NULL;
    }
}

bool GLOG::isLogEnabled() {
    return GLOG::s != NULL;
}

