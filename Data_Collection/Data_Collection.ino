#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

float angleX = 0.0, angleY = 0.0, angleZ = 0.0;
unsigned long lastTime = 0;
#define BUTTON_PIN 23

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  lastTime = millis();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

}

void loop() {
  bool buttonState = digitalRead(BUTTON_PIN);

  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // Calculate delta time in seconds
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  // Convert rad/s to deg/s
  float gyroXDegPerSec = gyro.gyro.x * (180.0 / M_PI);
  float gyroYDegPerSec = gyro.gyro.y * (180.0 / M_PI);
  float gyroZDegPerSec = gyro.gyro.z * (180.0 / M_PI);

  // Integrate to get angles
  angleX += gyroXDegPerSec * deltaTime;
  angleY += gyroYDegPerSec * deltaTime;
  angleZ += gyroZDegPerSec * deltaTime;

  // Print only the angles
  //Serial.print("X (Roll): ");
  Serial.print(angleX);
  Serial.print(", ");
  //Serial.print(", Y (Pitch): ");
  Serial.print(angleY);
  Serial.print(", ");
  //Serial.print(", Z (Yaw): ");
  Serial.print(angleZ);
  Serial.print(", ");
  if (buttonState == LOW) {
    Serial.print("1");
  } else {
    Serial.print("0");
  }
  Serial.println("");


  delay(10);
}
