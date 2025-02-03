#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
/**
 * LED class
 * Controls the existing LEDs of the adapter board.
 *  - default LED can be OFF, ON of BLINK on activity (read from inverter)
 *  - red LED will light up when there are write commands to the inverter and turn off once they complete
 *  - green LED will blink on success of write command
 * 
 * The red and green LEDs will only work on large boards, and not on the ESP-01.
 */
class Leds {
    public:
        Leds();
        ~Leds();

        void lightUpDefault();
        void dimDefault();
        void turnOffDefault();
    
        void lightUpRed();
        void dimRed();
        void turnOffRed();
    
        void lightUpGreen();
        void dimGreen();
        void turnOffGreen();
};
#endif
