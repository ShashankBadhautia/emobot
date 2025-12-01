#ifndef MOTORCMD_H
#define MOTORCMD_H

#include <Arduino.h>

extern const int L_IN1 ;
extern const int L_IN2 ;

extern const int R_IN1 ;
extern const int R_IN2 ;

extern const int SLEEP_PIN ;

extern const unsigned long LOOP_DELAY ; 

extern const bool IR_ACTIVE_LOW ;

void forwardBoth();
void reverseBoth() ;
void turnLeft() ;
void turnRight();
void stopMotor() ;
void stopBrake() ;


class IRSensor {
public:
    int pin;
    int currentValue;
    int lastValue;

    unsigned long lastChangeTime;
    float motionSpeed;

    // thresholds tuned for digital IR sensors
    float fastThreshold = 0.004;     // very fast hand movement
    float slowThreshold = 0.0004;    // slower approach

    IRSensor(int p) {
        pin = p;
        pinMode(pin, INPUT);
        currentValue = digitalRead(pin);
        lastValue = currentValue;
        lastChangeTime = millis();
        motionSpeed = 0;
    }

    void update() {
        lastValue = currentValue;
        currentValue = digitalRead(pin);

        unsigned long irnow = millis();

        // change happened → calculate speed
        if (currentValue != lastValue) {
            unsigned long dt = irnow - lastChangeTime;
            if (dt == 0) dt = 1;

            // Since digital: value change is always 1
            motionSpeed = 1.0 / dt;

            lastChangeTime = irnow;
        } 
        else {
            // decay speed back to zero
            motionSpeed *= 0.95;
            if (motionSpeed < 0.00001) motionSpeed = 0;
        }
    }

    // returns true if the sensor changed state
    bool changed() const {
        return currentValue != lastValue;
    }

    // very fast movement detected
    bool isFast() const {
        return motionSpeed > fastThreshold;
    }

    // slower entry/exit
    bool isSlow() const {
        return motionSpeed > slowThreshold && motionSpeed <= fastThreshold;
    }

    // sensor sees an object
    bool isObjectDetected() const {
        return currentValue == LOW;  // FlyingFish outputs LOW for detection
    }
};



#endif