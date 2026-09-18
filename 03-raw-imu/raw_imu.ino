#include <Wire.h>

#define SDA_PIN 11
#define SCL_PIN 12
#define MPU_ADDR 0x68

#define PWR_MGMT_1   0x6B
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG  0x1B
#define ACCEL_XOUT_H 0x3B

int16_t AcX, AcY, AcZ;
int16_t GyX, GyY, GyZ;

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

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  writeMPU(PWR_MGMT_1, 0x00);
  delay(100);

  writeMPU(ACCEL_CONFIG, 0x00);
  writeMPU(GYRO_CONFIG, 0x00);

  Serial.println();
  Serial.println("MPU6050 Raw IMU Data");
  Serial.println("--------------------");
}

void loop() {
  readMPU();

  float ax = AcX / 16384.0;
  float ay = AcY / 16384.0;
  float az = AcZ / 16384.0;

  float gx = GyX / 131.0;
  float gy = GyY / 131.0;
  float gz = GyZ / 131.0;

  Serial.print("ACC  X: ");
  Serial.print(ax, 3);
  Serial.print(" g | Y: ");
  Serial.print(ay, 3);
  Serial.print(" g | Z: ");
  Serial.print(az, 3);
  Serial.print(" g");

  Serial.print("    GYRO  X: ");
  Serial.print(gx, 2);
  Serial.print(" | Y: ");
  Serial.print(gy, 2);
  Serial.print(" | Z: ");
  Serial.print(gz, 2);
  Serial.println(" deg/s");

  delay(10);
}
