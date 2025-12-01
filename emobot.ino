#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <frames.h>
#include <touchSensors.h>
#include <MPU9250_asukiaaa.h>
#include <gyro_sensor.h>
#include <ESP32Servo.h>
#include <servoControl.h>
#include <motorcmd.h>


// ---------------- SCREEN ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SDA_PIN 8
#define SCL_PIN 9
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET -1

// ---------------- IR PINS ----------------
#define IR_FRONT_LEFT   10
#define IR_FRONT_RIGHT  11
#define IR_BACK_LEFT    12
#define IR_BACK_RIGHT   13
#define IR_BOTTOM       21
#define IR_FRONT        22
// #define IR_HEAD         -1

// ---------------- TOUCH ----------------
#define TOUCH_HEAD 7
// #define TOUCH_BACK -1

// ---------------- SERVO PINS ----------------
#define HEAD_SERVO_PIN 3
#define LEFT_SERVO_PIN 4
#define RIGHT_SERVO_PIN 5

// ---------------- IDLE ----------------
unsigned long lastActivityTime = 0;
bool idle = false;

//--------------- CLIFF --------------------
bool cliffCheckActive = false;
unsigned long cliffStartTime = 0;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --------------------------------------------------------
// FRAME ARRAYS (UNCHANGED)
// --------------------------------------------------------
extern const int emo_dizzyallArray_LEN;
extern const unsigned char* emo_dizzyallArray[98];
extern const unsigned char* emo_surprisedallArray[15];
extern const int emo_surprisedallArray_LEN;
extern const unsigned char* emo_idleallArray[45];
extern const int emo_idleallArray_LEN;
extern const unsigned char* emo_toAngry_frameallArray[64];
extern const int emo_toAngry_frameallArray_LEN;
extern const unsigned char* emo_happyallArray[60];
extern const int emo_happyallArray_LEN;
extern const int emo_toHappyallArray_LEN;
extern const unsigned char* emo_toHappyallArray[emo_toHappyallArray_LEN];
extern const int emo_sleepingallArray_LEN;
extern const unsigned char* emo_sleepingallArray[30];
extern const int emo_sadallArray_LEN;
extern const unsigned char* emo_sadallArray[emo_sadallArray_LEN];
extern const int emo_patallArray_LEN;
extern const unsigned char* emo_patallArray[emo_patallArray_LEN];

// --------------------------------------------------------
// ANIMATION ENGINE (UNCHANGED)
// --------------------------------------------------------
const unsigned char * const *gPreludeArr = nullptr;
int gPreludeLen = 0;
int gPreludeIndex = 0;

const unsigned char * const *gLoopArr = nullptr;
int gLoopLen = 0;
int gLoopIndex = 0;

bool gInPrelude = false;
bool gPlaying = false;

unsigned long gLastFrameTime = 0;
unsigned int gFrameDelay = 60;
unsigned int gPreludeFrameDelay = 40;

static inline void drawFrame(const unsigned char *bmp) {
  display.clearDisplay();
  display.drawBitmap(0, 0, bmp, 128, 64, 1);
  display.display();
}

void startAnimation(const unsigned char * const prelude[], int preludeLen,
                    const unsigned char * const loopArr[], int loopLen,
                    bool restartPrelude = true)
{
  gPreludeArr = prelude;
  gPreludeLen = preludeLen;
  gLoopArr = loopArr;
  gLoopLen = loopLen;

  if (gPreludeLen > 0 && restartPrelude) {
    gInPrelude = true;
    gPreludeIndex = 0;
  } else {
    gInPrelude = (gPreludeLen > 0 && restartPrelude);
    gPreludeIndex = restartPrelude ? 0 : gPreludeLen;
  }

  gLoopIndex = 0;
  gPlaying = true;
  gLastFrameTime = millis();
}

void updateAnimation() {
  if (!gPlaying) return;
  unsigned long now = millis();
  unsigned int delayMs = gInPrelude ? gPreludeFrameDelay : gFrameDelay;

  if ((now - gLastFrameTime) < delayMs) return;
  gLastFrameTime = now;

  if (gInPrelude && gPreludeArr && gPreludeLen > 0) {
    if (gPreludeIndex >= gPreludeLen) {
      gInPrelude = false;
      gLoopIndex = 0;
    } else {
      drawFrame(gPreludeArr[gPreludeIndex]);
      gPreludeIndex++;
      return;
    }
  }

  if (gLoopArr && gLoopLen > 0) {
    if (gLoopIndex >= gLoopLen) gLoopIndex = 0;
    drawFrame(gLoopArr[gLoopIndex]);
    gLoopIndex++;
  }
}

// --------------------------------------------------------
// COMMANDS
// --------------------------------------------------------
void handleCommand(const String &cmd) {
  if (cmd == "idle")
    return startAnimation(nullptr, 0, emo_idleallArray, emo_idleallArray_LEN, true);

  if (cmd == "surprised")
    return startAnimation(emo_surprisedallArray, emo_surprisedallArray_LEN, emo_happyallArray, emo_happyallArray_LEN, true);

  if (cmd == "sleep")
    return startAnimation(nullptr, 0, emo_sleepingallArray, emo_sleepingallArray_LEN, true);

  if (cmd == "dizzy")
    return startAnimation(nullptr, 0, emo_dizzyallArray, emo_dizzyallArray_LEN, true);

  if (cmd == "happy")
    return startAnimation(emo_toHappyallArray, emo_toHappyallArray_LEN, emo_happyallArray, emo_happyallArray_LEN, true);

  if (cmd == "angry") {
    static const unsigned char *angryHold[1] = { emo_toAngry_frameallArray[63] };
    return startAnimation(emo_toAngry_frameallArray, emo_toAngry_frameallArray_LEN, angryHold, 1, true);
  }

  if (cmd == "pat")
    return startAnimation(nullptr, 0, emo_patallArray, emo_patallArray_LEN, true);

  if (cmd == "sad")
    return startAnimation(nullptr, 0, emo_sadallArray, emo_sadallArray_LEN, true);
}

// --------------------------------------------------------
// TOUCH + IR + GYRO OBJECTS
// --------------------------------------------------------
TouchSensor headTouch(TOUCH_HEAD);
// TouchSensor backTouch(TOUCH_BACK);

GyroSensor gyro;

IRSensor front(IR_FRONT);
IRSensor frontLeft(IR_FRONT_LEFT);
IRSensor frontRight(IR_FRONT_RIGHT);
IRSensor backLeft(IR_BACK_LEFT);
IRSensor backRight(IR_BACK_RIGHT);
IRSensor bottom(IR_BOTTOM);
// IRSensor head(IR_HEAD);


ServoController servos;

// --------------------------------------------------------
// IDLE TIMER
// --------------------------------------------------------
void registerActivity() {
  lastActivityTime = millis();
  idle = false;
}

void checkIdleTimer() {
  if (millis() - lastActivityTime > 60000) {
    idle = true;
    handleCommand("idle");
  }
}
//---------------------------------------------------------
// Cliff detection 
//---------------------------------------------------------
void handleCliffLogic() {
    // Step 1: bottom IR sees no ground
    if (!bottom.isObjectDetected() && !cliffCheckActive) {
        stopMotor();
        reverseBoth();
        cliffCheckActive = true;
        cliffStartTime = millis();
        return;
    }

    // Step 2: after 400 ms of reversing, check again
    if (cliffCheckActive && millis() - cliffStartTime > 400) {

        stopMotor();
        cliffCheckActive = false;

        if (bottom.isObjectDetected()) {
            // Cliff successfully avoided
            handleCommand("surprised");
            servos.cliffReaction();

            turnLeft();   // or turnRight()
        } 
        else {
            // Still no ground → LIFTED
            handleCommand("sad");
            servos.liftedReaction();
            stopMotor();
            idle = false;    // Bot is no longer walking
        }
    }
}


// --------------------------------------------------------
// SETUP
// --------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
  delay(100);

  gyro.begin();
  gyro.calibrate();

  servos.begin(HEAD_SERVO_PIN, LEFT_SERVO_PIN, RIGHT_SERVO_PIN);

  startAnimation(nullptr, 0, emo_idleallArray, emo_idleallArray_LEN, true);
  lastActivityTime = millis();
}

// --------------------------------------------------------
// LOOP
// --------------------------------------------------------
void loop() {
  headTouch.update();
  // backTouch.update();

  front.update();
  frontLeft.update();
  frontRight.update();
  backLeft.update();
  backRight.update();
  bottom.update();
  // head.update();

  gyro.update();
  servos.update();

  bool activity =
      headTouch.singleTap() || headTouch.doubleTap() || headTouch.longPress() ||
      gyro.tilt || gyro.shake ;

  if (activity) {
    registerActivity();
    stopMotor();
    servos.centerHead();
  }

  if (headTouch.singleTap()) { handleCommand("happy"); servos.happyReaction(); }
  if (headTouch.doubleTap()) { handleCommand("surprised"); servos.surprisedReaction(); }
  if (headTouch.longPress()) { handleCommand("pat"); servos.patReaction(); }
  // if (backTouch.singleTap()) { handleCommand("happy"); servos.happyReaction(); }
  // if (backTouch.longPress()) { handleCommand("pat"); servos.patReaction(); }

  if (gyro.shake) { handleCommand("dizzy"); servos.dizzyReaction(); }
  if (gyro.tilt)  { handleCommand("sad");   servos.sadReaction(); }

  if (!idle) {
    if (front.isFast()) {
        handleCommand("surprised");
        servos.surprisedReaction();
    }
  }

  if (idle) {
    handleCliffLogic();
    if (cliffCheckActive) return;

    servos.centerHead();

    if (front.isObjectDetected() || frontLeft.isObjectDetected() || frontRight.isObjectDetected()) {
      stopMotor();
      if (frontLeft.isObjectDetected()) turnRight();
      else if (frontRight.isObjectDetected()) turnLeft();
      else turnLeft();
    }
    else if (backLeft.isObjectDetected() || backRight.isObjectDetected()) {
      forwardBoth();
    }
    else {
      static unsigned long nextMoveTime = 0;
      unsigned long now = millis();
      if (now > nextMoveTime) {
        int r = random(0, 10);
        if (r < 6) forwardBoth();
        else if (r == 6) turnLeft();
        else if (r == 7) turnRight();
        else stopMotor();
        nextMoveTime = now + random(800, 2000);
      }
    }
  }

  updateAnimation();
  checkIdleTimer();
}
