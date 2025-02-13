#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// This variable holds our estimated rotation about X in degrees.
float angleX = 0.0;

unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial){ delay(10); }

  if(!mpu.begin()){
    Serial.println("Failed to find MPU6050 chip");
    while(1){ delay(10); }
  }

  // Recommended ranges/filters — adjust if needed
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  lastTime = millis();
}

void loop() {
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // Time difference in seconds
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  // gyro.x is in rad/s; convert to deg/s
  float gyroXDegPerSec = gyro.gyro.x * (180.0 / M_PI);

  // Integrate to find total rotation in degrees
  angleX += gyroXDegPerSec * deltaTime;

  // Check thresholds (±90°)
  if(angleX > 90.0) {
    Serial.println("Key turned ~90° clockwise");
    angleX = 0; // Reset or handle as needed
  }
  else if(angleX < -90.0) {
    Serial.println("Key turned ~90° counterclockwise");
    angleX = 0; // Reset or handle as needed
  }

  // (Optional) Print the integrated angle for debugging
  Serial.print("AngleX: ");
  Serial.println(angleX);

  delay(10);
}
