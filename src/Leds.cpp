#include "Leds.h"
#include "GlobalDefs.h"

#ifdef LARGE_ESP_BOARD
#define LED_RED   D7
#define LED_GREEN D8
#endif

Leds::Leds() {
    pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
    
    #ifdef LARGE_ESP_BOARD
        pinMode(LED_RED, OUTPUT);         // Initialize other LED pins on larger boards
        pinMode(LED_GREEN, OUTPUT);
    #endif
    
    analogWriteRange(512);
}

Leds::~Leds() {

}

void Leds::lightUpDefault()
{
    analogWrite(LED_BUILTIN, 256);
}

void Leds::dimDefault()
{
    analogWrite(LED_BUILTIN, 4);
}

void Leds::turnOffDefault()
{
    analogWrite(LED_BUILTIN, 0);
}

void Leds::lightUpRed()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_RED, 384);
    #endif
}

void Leds::dimRed()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_RED, 8);
    #endif
}

void Leds::turnOffRed()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_RED, 0);
    #endif
}

void Leds::lightUpGreen()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_GREEN, 384);
    #endif
}

void Leds::dimGreen()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_GREEN, 8);
    #endif
}

void turnOffGreen()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_GREEN, 0);
    #endif
}
