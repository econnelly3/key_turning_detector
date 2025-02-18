#include "key_turn_model.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
Eloquent::ML::Port::RandomForest model;

const int WINDOW_SIZE = 70;  // Match the window size used in training
float window[WINDOW_SIZE * 3];  // Stores last 50 readings (X, Y, Z)
int windowIndex = 0;  // Track where to insert new data
bool windowFull = false;  // Track if we have enough data to predict

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    Serial.println("Initializing MPU6050...");
    if (!mpu.begin()) {
        Serial.println("MPU6050 not found!");
        while (1) { delay(10); }
    }
    
    Serial.println("MPU6050 detected. System ready.");
}

void loop() {
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

    // Store new values in the sliding window
    window[windowIndex * 3] = gyro.gyro.x * (180.0 / M_PI);
    window[windowIndex * 3 + 1] = gyro.gyro.y * (180.0 / M_PI);
    window[windowIndex * 3 + 2] = gyro.gyro.z * (180.0 / M_PI);

    // Update index
    windowIndex++;

    // Once the buffer is full, make a prediction
    if (windowIndex >= WINDOW_SIZE) {
        windowFull = true;
        windowIndex = 0;  // Reset index for circular buffer
    }

    // Only predict if we have a full window
    if (windowFull) {
        int prediction = model.predict(window);
        Serial.print("Prediction: "); Serial.println(prediction);

        if (prediction == 1) {
            Serial.println("⚠️ Key Turn Detected!");
        }
    }

    delay(10);  // Adjust based on sampling rate
}
