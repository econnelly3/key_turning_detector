#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

float angleX = 0.0, angleY = 0.0, angleZ = 0.0;
unsigned long lastTime = 0;
unsigned long time90CW = 0;  // Timestamp for 90° clockwise
unsigned long time90CCW = 0; // Timestamp for 90° counterclockwise
const int LED_PIN = 4;  // Change to the pin where your LED is connected

void setup() {
  
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  pinMode(LED_PIN, OUTPUT);  // Set LED pin as output


  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  lastTime = millis();
}

void loop() {
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

  // Check for ~90° rotation in X-axis
  if (angleX > 80.0) {
    if (fabs(angleY) < 30.0 && fabs(angleZ) < 30.0) {
      time90CW = millis(); // Store time of CW turn
      Serial.print("Key turned ~90° CW at time (ms): ");
      Serial.println(time90CW);
    }
    angleX = 0;
    angleY = 0;
    angleZ = 0;
  }
  else if (angleX < -80.0) {
    if (fabs(angleY) < 30.0 && fabs(angleZ) < 30.0) {
      time90CCW = millis(); // Store time of CCW turn
      Serial.print("Key turned ~90° CCW at time (ms): ");
      Serial.println(time90CCW);
    }
    angleX = 0;
    angleY = 0;
    angleZ = 0;
  }

  // Ensure both timestamps are valid before checking
  if (time90CW > 0 && time90CCW > 0) {
    float timeDifference = (time90CW > time90CCW) ? (time90CW - time90CCW) / 1000.0 : (time90CCW - time90CW) / 1000.0;

    if (timeDifference < 5.0) {
      Serial.println("TURNED KEY");
      time90CW = 0;  // Reset timestamps after detection
      time90CCW = 0;
      digitalWrite(LED_PIN, HIGH); // Turn on LED
      delay(3000); // Keep LED on for 3 seconds
      digitalWrite(LED_PIN, LOW);  // Turn off LED

    }
  }

  delay(10);
}
