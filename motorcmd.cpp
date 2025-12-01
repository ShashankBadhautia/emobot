#include <Arduino.h>
#include "motorcmd.h"

#define L_IN1 15
#define L_IN2 16
#define R_IN1 17
#define R_IN2 18


enum MotorState {
  COAST = 0,
  FORWARD = 1,
  REVERSE = 2,
  BRAKE = 3
};




void setLeftMotor(MotorState st) {
  switch (st) {
    case FORWARD:
      digitalWrite(L_IN1, HIGH);
      digitalWrite(L_IN2, LOW);
      break;
    case REVERSE:
      digitalWrite(L_IN1, LOW);
      digitalWrite(L_IN2, HIGH);
      break;
    case BRAKE:
      digitalWrite(L_IN1, HIGH);
      digitalWrite(L_IN2, HIGH);
      break;
    case COAST:
    default:
      digitalWrite(L_IN1, LOW);
      digitalWrite(L_IN2, LOW);
      break;
  }
}


void setRightMotor(MotorState st) {
  switch (st) {
    case FORWARD:
      digitalWrite(R_IN1, HIGH);
      digitalWrite(R_IN2, LOW);
      break;
    case REVERSE:
      digitalWrite(R_IN1, LOW);
      digitalWrite(R_IN2, HIGH);
      break;
    case BRAKE:
      digitalWrite(R_IN1, HIGH);
      digitalWrite(R_IN2, HIGH);
      break;
    case COAST:
    default:
      digitalWrite(R_IN1, LOW);
      digitalWrite(R_IN2, LOW);
      break;
  }
}


void forwardBoth() {
  setLeftMotor(FORWARD);
  setRightMotor(FORWARD);
}
void reverseBoth() {
  setLeftMotor(REVERSE);
  setRightMotor(REVERSE);
}
void turnLeft() {
  setLeftMotor(REVERSE);
  setRightMotor(FORWARD);
}
void turnRight() {
  setLeftMotor(FORWARD);
  setRightMotor(REVERSE);
}
void stopMotor() {
  setLeftMotor(COAST);
  setRightMotor(COAST);
}
void stopBrake() {
  setLeftMotor(BRAKE);
  setRightMotor(BRAKE);
}



