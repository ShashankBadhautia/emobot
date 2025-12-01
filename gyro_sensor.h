#ifndef GYRO_SENSOR_H
#define GYRO_SENSOR_H

#include <Arduino.h>
#include <MPU9250_asukiaaa.h>

class GyroSensor {
public:
    bool tilt = false;
    bool shake = false;

private:
    MPU9250_asukiaaa mpu;

    float baseAx = 0, baseAy = 0;
    float baseGx = 0, baseGy = 0, baseGz = 0;

    bool calibrated = false;

public:

    void begin() {
        mpu.setWire(&Wire);
        mpu.beginAccel();
        mpu.beginGyro();
    }

    void calibrate() {
        float sumAx = 0, sumAy = 0;
        float sumGx = 0, sumGy = 0, sumGz = 0;

        const int samples = 200;

        for (int i = 0; i < samples; i++) {
            mpu.accelUpdate();
            mpu.gyroUpdate();

            sumAx += mpu.accelX();
            sumAy += mpu.accelY();

            sumGx += mpu.gyroX();
            sumGy += mpu.gyroY();
            sumGz += mpu.gyroZ();

            delay(1);
        }

        baseAx = sumAx / samples;
        baseAy = sumAy / samples;

        baseGx = sumGx / samples;
        baseGy = sumGy / samples;
        baseGz = sumGz / samples;

        calibrated = true;
    }

    void update() {
        if (!calibrated) return;

        mpu.accelUpdate();
        mpu.gyroUpdate();

        float ax = mpu.accelX() - baseAx;
        float ay = mpu.accelY() - baseAy;

        float gx = mpu.gyroX() - baseGx;
        float gy = mpu.gyroY() - baseGy;
        float gz = mpu.gyroZ() - baseGz;

        static float fax = 0, fay = 0;
        static float fgx = 0, fgy = 0, fgz = 0;

        fax = fax * 0.9 + ax * 0.1;
        fay = fay * 0.9 + ay * 0.1;

        fgx = fgx * 0.8 + gx * 0.2;
        fgy = fgy * 0.8 + gy * 0.2;
        fgz = fgz * 0.8 + gz * 0.2;

        tilt = (abs(fax) > 0.6 || abs(fay) > 0.6);
        shake = (abs(fgx) > 60 || abs(fgy) > 60 || abs(fgz) > 60);
    }
};

#endif