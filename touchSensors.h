#ifndef TOUCHSENSORS_H
#define TOUCHSENSORS_H

#include <Arduino.h>


class TouchSensor {
public:
    int pin;
    bool lastState;
    unsigned long touchStartTime;
    unsigned long lastTapTime;
    bool longPressTriggered;

    // thresholds (you can tune)
    unsigned long tapMaxTime = 300;     // max ms for a tap
    unsigned long doubleTapGap = 350;   // ms allowed between taps
    unsigned long longPressTime = 700;  // ms for long press

    // event flags (cleared after read)
    bool singleTapFlag;
    bool doubleTapFlag;
    bool longPressFlag;

    TouchSensor(int p) {
        pin = p;
        pinMode(pin, INPUT);
        lastState = false;
        touchStartTime = 0;
        lastTapTime = 0;
        longPressTriggered = false;

        singleTapFlag = false;
        doubleTapFlag = false;
        longPressFlag = false;
    }

    void update() {
        bool current = digitalRead(pin);    // HIGH = touch (most modules)
        unsigned long now = millis();

        // ---- TOUCH START ----
        if (current && !lastState) {
            touchStartTime = now;
            longPressTriggered = false;
        }

        // ---- LONG PRESS DETECTION ----
        if (current && !longPressTriggered &&
            (now - touchStartTime > longPressTime)) {
            longPressTriggered = true;
            longPressFlag = true;
        }

        // ---- TOUCH RELEASE ----
        if (!current && lastState) {
            unsigned long pressDuration = now - touchStartTime;

            // if long press already happened, do nothing
            if (!longPressTriggered) {
                // SHORT TAP
                if (pressDuration < tapMaxTime) {
                    if (now - lastTapTime < doubleTapGap) {
                        doubleTapFlag = true;
                        lastTapTime = 0; // reset
                    } else {
                        singleTapFlag = true;
                        lastTapTime = now;
                    }
                }
            }
        }

        lastState = current;
    }

    // ---- Public getters: return true once per event ----
    bool singleTap() {
        if (singleTapFlag) {
            singleTapFlag = false;
            return true;
        }
        return false;
    }

    bool doubleTap() {
        if (doubleTapFlag) {
            doubleTapFlag = false;
            return true;
        }
        return false;
    }

    bool longPress() {
        if (longPressFlag) {
            longPressFlag = false;
            return true;
        }
        return false;
    }
};

#endif