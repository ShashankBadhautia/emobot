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
    bool state;

    IRSensor(int p) {
        pin = p;
        pinMode(pin, INPUT_PULLUP);   // FIX
        state = false;
    }

    void update() {
        state = (digitalRead(pin) == LOW);
    }

    bool isObjectDetected() {
        return state;
    }
};




#endif