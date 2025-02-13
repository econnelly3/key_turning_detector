#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define HISTORY_SIZE 500  // Store ~5 seconds of history (assuming 10ms loop delay)
#define MAX_DEVIATION 20.0
#define MIN_X_RANGE 80.0  // Require at least 160° of rotation

Adafruit_MPU6050 mpu;

float angleX = 0.0, angleY = 0.0, angleZ = 0.0;
unsigned long lastTime = 0;

const int LED_PIN = 4;  // LED to indicate a valid key turn

// Circular buffer for tracking angle history
struct AngleHistory {
    unsigned long timestamp;
    float angleX;
    float angleY;
    float angleZ;
};
AngleHistory history[HISTORY_SIZE];
int historyIndex = 0;
bool historyFull = false;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

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

    // Store history of X, Y & Z angles
    history[historyIndex] = { currentTime, angleX, angleY, angleZ };
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;  // Circular buffer
    if (historyIndex == 0) historyFull = true;  // Mark buffer full after first pass

    // Check if the range of X movement is greater than 160° within the detected X motion window
    if (checkXRangeAndYZDuringRotation()) {
        Serial.println("TURNED KEY - LED ON");
        digitalWrite(LED_PIN, HIGH);
        delay(3000);
        digitalWrite(LED_PIN, LOW);
    } else {
        Serial.println("Key motion invalid - NO LED");
    }

    delay(10);
}

// **Fix: This function now finds the min/max timestamps for X rotation, then checks Y/Z within that window**
bool checkXRangeAndYZDuringRotation() {
    if (!historyFull && historyIndex < 2) return false;  // Not enough data

    float minX = 1000.0, maxX = -1000.0;
    unsigned long minXTime = 0, maxXTime = 0;

    unsigned long minTime = millis() - 5000;  // Only check last 5 seconds

    for (int i = 0; i < HISTORY_SIZE; i++) {
        AngleHistory record = history[i];

        if (record.timestamp < minTime) continue;  // Ignore old data

        if (record.angleX < minX) {
            minX = record.angleX;
            minXTime = record.timestamp;
        }
        if (record.angleX > maxX) {
            maxX = record.angleX;
            maxXTime = record.timestamp;
        }
    }

    float rangeX = maxX - minX;
    Serial.print("MaxX-MinX: "); Serial.println(rangeX);

    if (rangeX < MIN_X_RANGE) {
        return false;  // Not enough X movement
    }

    // Now that we have minXTime and maxXTime, check Y/Z range within that exact period
    float minY = 1000.0, maxY = -1000.0;
    float minZ = 1000.0, maxZ = -1000.0;

    unsigned long lowerBound = min(minXTime, maxXTime);
    unsigned long upperBound = max(minXTime, maxXTime);

    for (int i = 0; i < HISTORY_SIZE; i++) {
        AngleHistory record = history[i];

        if (record.timestamp < lowerBound || record.timestamp > upperBound) continue;  // Ignore data outside X rotation window

        if (record.angleY < minY) minY = record.angleY;
        if (record.angleY > maxY) maxY = record.angleY;
        if (record.angleZ < minZ) minZ = record.angleZ;
        if (record.angleZ > maxZ) maxZ = record.angleZ;
    }

    float rangeY = maxY - minY;
    float rangeZ = maxZ - minZ;

    Serial.print("MaxY-MinY: "); Serial.print(rangeY);
    Serial.print("  MaxZ-MinZ: "); Serial.println(rangeZ);

    return (rangeY <= MAX_DEVIATION && rangeZ <= MAX_DEVIATION);
}
