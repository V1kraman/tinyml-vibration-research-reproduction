#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define SDA_PIN 11
#define SCL_PIN 12

#define MPU_ADDR 0x68

#define PWR_MGMT_1   0x6B
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG  0x1B
#define ACCEL_XOUT_H 0x3B

int16_t AcX, AcY, AcZ;
int16_t GyX, GyY, GyZ;

float gyroOffsetX = 0;
float gyroOffsetY = 0;
float gyroOffsetZ = 0;

float roll = 0;
float pitch = 0;
float yaw = 0;

unsigned long lastTime = 0;

const float alpha = 0.98;

struct Point3D {
  float x;
  float y;
  float z;
};

struct Point2D {
  int x;
  int y;
};

Point3D cube[8] = {
  {-1, -1, -1},
  { 1, -1, -1},
  { 1,  1, -1},
  {-1,  1, -1},
  {-1, -1,  1},
  { 1, -1,  1},
  { 1,  1,  1},
  {-1,  1,  1}
};

void writeMPU(byte reg, byte data) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, 14, true);

  if (Wire.available() == 14) {
    AcX = Wire.read() << 8 | Wire.read();
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();

    Wire.read();
    Wire.read();

    GyX = Wire.read() << 8 | Wire.read();
    GyY = Wire.read() << 8 | Wire.read();
    GyZ = Wire.read() << 8 | Wire.read();
  }
}

void calibrateGyro() {
  long sumX = 0;
  long sumY = 0;
  long sumZ = 0;

  const int samples = 500;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(0, 0);
  display.println("Calibrating gyro...");

  display.setCursor(0, 15);
  display.println("Keep sensor still");

  display.display();

  for (int i = 0; i < samples; i++) {
    readMPU();

    sumX += GyX;
    sumY += GyY;
    sumZ += GyZ;

    delay(5);
  }

  gyroOffsetX = (float)sumX / samples;
  gyroOffsetY = (float)sumY / samples;
  gyroOffsetZ = (float)sumZ / samples;
}

void calculateAccelerometerAngles(
  float &accelRoll,
  float &accelPitch
) {
  accelRoll =
    atan2((float)AcY, (float)AcZ) * 180.0 / PI;

  accelPitch =
    atan2(
      -(float)AcX,
      sqrt((float)AcY * AcY + (float)AcZ * AcZ)
    ) * 180.0 / PI;
}

void updateOrientation(float dt) {
  float gx = ((float)GyX - gyroOffsetX) / 131.0;
  float gy = ((float)GyY - gyroOffsetY) / 131.0;
  float gz = ((float)GyZ - gyroOffsetZ) / 131.0;

  float gyroRoll = roll + gx * dt;
  float gyroPitch = pitch + gy * dt;

  yaw += gz * dt;

  float accelRoll;
  float accelPitch;

  calculateAccelerometerAngles(accelRoll, accelPitch);

  roll =
    alpha * gyroRoll +
    (1.0 - alpha) * accelRoll;

  pitch =
    alpha * gyroPitch +
    (1.0 - alpha) * accelPitch;
}

Point3D rotatePoint(Point3D p) {
  float rx = roll * PI / 180.0;
  float ry = pitch * PI / 180.0;
  float rz = yaw * PI / 180.0;

  float y1 = p.y * cos(rx) - p.z * sin(rx);
  float z1 = p.y * sin(rx) + p.z * cos(rx);

  p.y = y1;
  p.z = z1;

  float x2 = p.x * cos(ry) + p.z * sin(ry);
  float z2 = -p.x * sin(ry) + p.z * cos(ry);

  p.x = x2;
  p.z = z2;

  float x3 = p.x * cos(rz) - p.y * sin(rz);
  float y3 = p.x * sin(rz) + p.y * cos(rz);

  p.x = x3;
  p.y = y3;

  return p;
}

Point2D projectPoint(Point3D p) {
  const float distance = 5.0;
  const float scale = 20.0;

  float factor =
    scale * distance / (distance + p.z);

  Point2D result;

  result.x = (int)(64 + p.x * factor);
  result.y = (int)(35 + p.y * factor);

  return result;
}

void drawCube() {
  Point2D projected[8];

  for (int i = 0; i < 8; i++) {
    Point3D rotated = rotatePoint(cube[i]);
    projected[i] = projectPoint(rotated);
  }

  display.drawLine(
    projected[0].x, projected[0].y,
    projected[1].x, projected[1].y,
    SH110X_WHITE
  );

  display.drawLine(
    projected[1].x, projected[1].y,
    projected[2].x, projected[2].y,
    SH110X_WHITE
  );

  display.drawLine(
    projected[2].x, projected[2].y,
    projected[3].x, projected[3].y,
    SH110X_WHITE
  );

  display.drawLine(
    projected[3].x, projected[3].y,
    projected[0].x, projected[0].y,
    SH110X_WHITE
  );

  display.drawLine(
    projected[4].x, projected[4].y,
    projected[5].x, projected[5].y,
    SH110X_WHITE
  );

  display.drawLine(
    projected[5].x, projected[5].y,
    projected[6].x, projected[6].y,
    SH110X_WHITE
  );

  display.drawLine(
    projected[6].x, projected[6].y,
    projected[7].x, projected[7].y,
    SH110X_WHITE
  );

  display.drawLine(
    projected[7].x, projected[7].y,
    projected[4].x, projected[4].y,
    SH110X_WHITE
  );

  for (int i = 0; i < 4; i++) {
    display.drawLine(
      projected[i].x, projected[i].y,
      projected[i + 4].x, projected[i + 4].y,
      SH110X_WHITE
    );
  }

  for (int i = 0; i < 8; i++) {
    display.fillCircle(
      projected[i].x,
      projected[i].y,
      1,
      SH110X_WHITE
    );
  }

  display.fillCircle(64, 35, 1, SH110X_WHITE);
}

void drawInformation() {
  float gx = ((float)GyX - gyroOffsetX) / 131.0;
  float gy = ((float)GyY - gyroOffsetY) / 131.0;
  float gz = ((float)GyZ - gyroOffsetZ) / 131.0;

  float gyroMagnitude =
    sqrt(gx * gx + gy * gy + gz * gz);

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(0, 0);

  display.print("R:");
  display.print(roll, 1);

  display.print(" P:");
  display.print(pitch, 1);

  display.print(" Y:");
  display.print(yaw, 1);

  display.setCursor(0, 55);

  display.print("G:");
  display.print(gyroMagnitude, 1);
  display.print(" d/s");
}

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  writeMPU(PWR_MGMT_1, 0x00);
  delay(100);

  writeMPU(ACCEL_CONFIG, 0x00);
  writeMPU(GYRO_CONFIG, 0x00);

  if (!display.begin(OLED_ADDR, true)) {
    Serial.println("OLED initialization failed!");
    while (1);
  }

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(0, 0);
  display.println("TinyML IMU");
  display.println("Orientation");

  display.display();

  delay(1000);

  calibrateGyro();

  lastTime = micros();
}

void loop() {
  readMPU();

  unsigned long currentTime = micros();

  float dt =
    (currentTime - lastTime) / 1000000.0;

  lastTime = currentTime;

  if (dt > 0.1) {
    dt = 0.01;
  }

  updateOrientation(dt);

  Serial.print("Roll: ");
  Serial.print(roll);

  Serial.print(" | Pitch: ");
  Serial.print(pitch);

  Serial.print(" | Yaw: ");
  Serial.println(yaw);

  display.clearDisplay();

  drawCube();
  drawInformation();

  display.display();

  delay(10);
}
