#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <ESP32Servo.h>

// --------------------------------------------------------
// SERVO CONTROLLER FOR ESP32
// --------------------------------------------------------
class ServoMotor {
public:
    Servo s;
    int pin;
    int currentAngle;
    int targetAngle;
    int speed;
    unsigned long lastUpdate;

    void attach(int p, int startAngle = 90, int moveSpeed = 3) {
        pin = p;

        // FIX for ESP32 PWM
        s.setPeriodHertz(50); // standard 50Hz servo
        s.attach(pin, 500, 2400); // pulse width limits

        currentAngle = startAngle;
        targetAngle = startAngle;
        speed = moveSpeed;
        lastUpdate = millis();
        s.write(startAngle);
    }

    void moveTo(int angle) {
        targetAngle = constrain(angle, 0, 180);
    }

    void update() {
        unsigned long now = millis();
        if (now - lastUpdate < 15) return;
        lastUpdate = now;

        if (currentAngle == targetAngle) return;

        if (currentAngle < targetAngle) currentAngle += speed;
        else currentAngle -= speed;

        currentAngle = constrain(currentAngle, 0, 180);
        s.write(currentAngle);
    }
};

class ServoController {
public:
    ServoMotor headServo;
    ServoMotor leftArm;
    ServoMotor rightArm;

    void begin(int headPin, int leftPin, int rightPin) {
        headServo.attach(headPin, 90, 2);
        leftArm.attach(leftPin, 0, 3);
        rightArm.attach(rightPin, 180, 3);
    }

    void update() {
        headServo.update();
        leftArm.update();
        rightArm.update();
    }

    void armsUp() { leftArm.moveTo(90); rightArm.moveTo(90); }
    void armsDown() { leftArm.moveTo(0); rightArm.moveTo(180); }
    void nod() { headServo.moveTo(60); }
    void lookLeft() { headServo.moveTo(120); }
    void lookRight() { headServo.moveTo(60); }
    void centerHead() { headServo.moveTo(90); }

    // ======= SERVO REACTION FUNCTIONS =======

void happyReaction() {
    headServo.moveTo(70);      // small tilt
    leftArm.moveTo(90);
    rightArm.moveTo(90);
}

void surprisedReaction() {
    headServo.moveTo(130);     // quick left
    leftArm.moveTo(120);
    rightArm.moveTo(60);
}

void patReaction() {
    headServo.moveTo(80);      // slight nod
    leftArm.moveTo(70);
    rightArm.moveTo(110);
}

void sadReaction() {
    headServo.moveTo(100);     // down tilt
    leftArm.moveTo(0);
    rightArm.moveTo(180);
}

void dizzyReaction() {
    headServo.moveTo(60);      // will oscillate in loop
    leftArm.moveTo(110);
    rightArm.moveTo(70);
}

void idleScan() {
    static bool dir = false;
    headServo.moveTo(dir ? 110 : 70);
    leftArm.moveTo(0);
    rightArm.moveTo(180);
    dir = !dir;
}

void cliffReaction() {
    headServo.moveTo(130);
    leftArm.moveTo(120);
    rightArm.moveTo(60);
}

void liftedReaction() {
    headServo.moveTo(90);
    leftArm.moveTo(120);
    rightArm.moveTo(60);
}

};



#endif
