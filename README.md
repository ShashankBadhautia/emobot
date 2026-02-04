
🤖 EMO Bot

An Interactive Emotional Desktop Robot

EMO Bot is a compact, expressive robot designed to simulate emotional reactions using animations, touch input, infrared proximity sensing, and servo-driven body language.
The project focuses on non-blocking real-time behavior, modular hardware abstraction, and scalable emotion logic.

✨ Features

OLED-based facial animations (128×64)

Touch-based interaction (single tap, double tap, long press)

Infrared proximity sensing (rear interaction detection)

Expressive servo-driven head and arm movements

Idle detection with autonomous behavior

Fully non-blocking animation and input handling

Modular, class-based architecture (easy to extend)

🧠 System Architecture

EMO Bot is structured into four logical layers:

1. Perception Layer

Handles all sensory input:

Capacitive touch sensor (head)

Infrared sensors (back-left, back-right)

Each sensor is wrapped in its own class to keep logic isolated and testable.

2. Emotion & State Layer

Manages:

Active vs Idle state

Emotion transitions

Priority handling (touch overrides IR, etc.)

Idle state is triggered after a configurable inactivity timeout and reset immediately on interaction.

3. Expression Layer

Controls how emotions are expressed:

OLED facial animations

Servo motor body language (head + arms)

Animations and servo movements are decoupled, allowing them to run independently.

4. Actuation Layer

Handles physical movement:

Servo motors using ESP32-compatible PWM

Smooth, non-blocking interpolation for realistic motion

🧩 Hardware Components
Component	Description
MCU	ESP32
Display	0.96" OLED (SSD1306, I²C)
Touch Sensor	Capacitive touch module
IR Sensors	Digital IR obstacle sensors
Servos	3× Micro servo (Head + Arms)
Power	5V regulated supply
🔌 Pin Configuration (Example)
I²C
SDA → GPIO 8
SCL → GPIO 9

Touch Sensor
Head Touch → GPIO 7

IR Sensors
Back Left  → GPIO 15
Back Right → GPIO 16

Servo Motors
Head Servo  → GPIO 11
Left Arm   → GPIO 12
Right Arm  → GPIO 13


(Pins can be reassigned depending on ESP32 variant)

🖥️ Software Design
Non-Blocking Core Principle

EMO Bot never uses delay() for logic control.
All timing is handled via millis() ensuring:

Smooth animations

Responsive touch

Continuous servo motion

Animation Engine

Bitmap frame arrays stored in flash

Frame stepping controlled by time deltas

Loop-based emotion playback

updateAnimation(); // Called every loop()

Touch Interpretation

Touch input is classified into:

Single Tap → Happy

Double Tap → Dizzy

Long Press → Pat

All events are edge-triggered, not level-triggered.

IR Sensor Logic

Rear IR sensors trigger reactions once per detection, preventing animation lockups.

This avoids the common issue where:

“Animation keeps restarting while object stays in front of sensor”

🔄 Behavior Flow
Active Mode

Responds immediately to touch and IR input

Plays corresponding animation

Triggers servo reactions

Idle Mode

Enters after inactivity timeout

Loops idle animation

Optional head scanning behavior

🛠️ Manufacturing & Build Process
Step 1 – Electronics Prototyping

Breadboard testing for OLED, touch, IR, servos

PWM validation for ESP32

Step 2 – Software Bring-up

OLED driver validation

Animation engine testing

Sensor class isolation

Step 3 – Mechanical Integration

Servo mounting for expressive motion

Cable strain relief

Compact internal layout

Step 4 – Behavior Tuning

Threshold calibration

Servo speed tuning

Emotion priority balancing

🚀 Extensibility

Future expansions:

Front IR sensors

Gyroscope-based shake detection

Speech synthesis

Battery management

Wi-Fi / Bluetooth interaction

The architecture is already designed to support these additions.

📄 License

Open-source, educational and personal use friendly.
