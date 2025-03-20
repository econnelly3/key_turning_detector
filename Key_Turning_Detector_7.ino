#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define BUZZER_PIN  15  

Adafruit_MPU6050 mpu;

float angleX = 0.0, angleY = 0.0, angleZ = 0.0;
unsigned long lastTime = 0;
unsigned long turnDetectedTime = 0;

const int LED1_PIN = 4;  
const int LED2_PIN = 27;  
const int LED3_PIN = 33;  
const int buttonPin = 32; 

enum TurnState { INITIAL, CROSSED_90, CROSSED_180, CROSSED_0 };
TurnState turnState = INITIAL;

// Gyro drift correction
float gyroZOffset = 0.0;
float previousGyroZ = 0.0;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(buttonPin, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) { delay(10); }
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    lastTime = millis();
    calibrateGyroZ();
    playSuccessSound();
}

void loop() {
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastTime) / 1000.0;
    lastTime = currentTime;

    // Swapping Y and Z axes in calculations
    float gyroXDegPerSec = gyro.gyro.x * (180.0 / M_PI);
    float gyroYDegPerSec = gyro.gyro.z * (180.0 / M_PI);  // Using Z gyro as new pitch
    float gyroZDegPerSec = gyro.gyro.y * (180.0 / M_PI);  // Using Y gyro as new yaw

    // Apply low-pass filter to stabilize gyro readings
    float alpha = 0.9;
    gyroZDegPerSec = alpha * gyroZDegPerSec + (1 - alpha) * previousGyroZ;
    previousGyroZ = gyroZDegPerSec;

    // Subtract drift offset
    gyroZDegPerSec -= gyroZOffset;

    // Compute new swapped angles from accelerometer
    float angleX_acc = atan2(accel.acceleration.y, accel.acceleration.z) * (180.0 / M_PI);
    float angleZ_acc = atan2(accel.acceleration.x, sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z)) * (180.0 / M_PI);  // Swap Y/Z
    float angleY_acc = atan2(-accel.acceleration.z, sqrt(accel.acceleration.x * accel.acceleration.x + accel.acceleration.y * accel.acceleration.y)) * (180.0 / M_PI);

    // Apply complementary filter (98% gyro, 2% accelerometer)
    angleX = 0.98 * (angleX + gyroXDegPerSec * deltaTime) + 0.02 * angleX_acc;
    angleZ = 0.98 * (angleZ + gyroZDegPerSec * deltaTime) + 0.02 * angleZ_acc; // New Z stabilization
    angleY = 0.98 * (angleY + gyroYDegPerSec * deltaTime) + 0.02 * angleY_acc; // New Y using previous Z logic

    int buttonState = digitalRead(buttonPin);

    if (buttonState == LOW) {  
        print_angle_data();
        trackAngleSequence(angleX);
    }

    if (turnDetectedTime > 0) {
        updateLEDs();
    }
    delay(10);
}

void trackAngleSequence(float angleX) {
    switch (turnState) {
        case INITIAL:
            if (angleX >= 90.0) {
                Serial.println("Crossed 90° - Waiting for next state...");
                turnState = CROSSED_90;
            }
            break;

        case CROSSED_90:
            if (angleX >= 180.0 && abs(angleZ) < 30) {
                Serial.println("✅ Detected 90 → 180 sequence!");
                playSuccessSound();
                turnState = INITIAL;
                turnDetectedTime = millis();
            } else if (angleX <= 0.0 && abs(angleZ) < 30) {
                Serial.println("✅ Detected 90 → 0 sequence!");
                playSuccessSound();
                turnState = INITIAL;
                turnDetectedTime = millis();
            }
            break;

        default:
            turnState = INITIAL;
            break;
    }
}

void updateLEDs() {
    unsigned long elapsedTime = millis() - turnDetectedTime;
    
    if (elapsedTime < 1000) {
        digitalWrite(LED1_PIN, HIGH);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW);
    } else if (elapsedTime < 5000) {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, HIGH);
        digitalWrite(LED3_PIN, LOW);
    } else if (elapsedTime < 10000) {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, HIGH);
    } else {
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW);
        turnDetectedTime = 0;
    }
}

void playTone(int pin, int frequency, int duration) {
  if (frequency <= 0) {
    delay(duration);
    return;
  }
  int period = 1000000 / frequency;
  int halfPeriod = period / 2;

  unsigned long startTime = millis();
  while ((millis() - startTime) < (unsigned long)duration) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(pin, LOW);
    delayMicroseconds(halfPeriod);
  }
}

void playSuccessSound() {
  playTone(BUZZER_PIN, 600, 150);  
  delay(50);
  playTone(BUZZER_PIN, 800, 150);  
  delay(50);
  playTone(BUZZER_PIN, 1000, 200); 
  delay(50);
  playTone(BUZZER_PIN, 1200, 250); 
  delay(100);
}

void print_angle_data() {
    Serial.print("Time: ");
    Serial.print(millis());
    Serial.print(", AngleX: ");
    Serial.print(angleX);
    Serial.print(", AngleY: ");
    Serial.print(angleY);
    Serial.print(", AngleZ: ");
    Serial.println(angleZ);
}

// Gyro drift calibration function
void calibrateGyroZ() {
    Serial.println("Calibrating gyro Z offset...");
    float sum = 0;
    int numSamples = 500;
    
    for (int i = 0; i < numSamples; i++) {
        sensors_event_t accel, gyro, temp;
        mpu.getEvent(&accel, &gyro, &temp);
        sum += gyro.gyro.z * (180.0 / M_PI);
        delay(2);
    }
    
    gyroZOffset = sum / numSamples;
    Serial.print("Calibrated gyro Z offset: ");
    Serial.println(gyroZOffset);
}
