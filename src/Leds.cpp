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
    analogWrite(LED_BUILTIN, 378);
}

void Leds::dimDefault()
{
    analogWrite(LED_BUILTIN, 32);
}

void Leds::turnOffDefault()
{
    analogWrite(LED_BUILTIN, 0);
}

void Leds::lightUpRed()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_RED, 378);
    #endif
}

void Leds::dimRed()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_RED, 32);
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
    analogWrite(LED_GREEN, 378);
    #endif
}

void Leds::dimGreen()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_GREEN, 32);
    #endif
}

void turnOffGreen()
{
    #ifdef LARGE_ESP_BOARD
    analogWrite(LED_GREEN, 0);
    #endif
}
