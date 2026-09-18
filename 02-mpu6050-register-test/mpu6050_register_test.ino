#include <Wire.h>

#define SDA_PIN 11
#define SCL_PIN 12
#define MPU_ADDR 0x68

#define WHO_AM_I     0x75
#define PWR_MGMT_1   0x6B
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG  0x1B

byte readRegister(byte reg) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, 1, true);

  if (Wire.available()) return Wire.read();
  return 0xFF;
}

void writeRegister(byte reg, byte value) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  Serial.println();
  Serial.println("MPU6050 Register Test");
  Serial.println("---------------------");

  writeRegister(PWR_MGMT_1, 0x00);
  delay(100);

  byte whoAmI = readRegister(WHO_AM_I);

  Serial.print("WHO_AM_I: 0x");
  if (whoAmI < 16) Serial.print("0");
  Serial.println(whoAmI, HEX);

  byte accelConfig = readRegister(ACCEL_CONFIG);
  byte gyroConfig  = readRegister(GYRO_CONFIG);
  byte powerConfig = readRegister(PWR_MGMT_1);

  Serial.print("PWR_MGMT_1:   0x");
  Serial.println(powerConfig, HEX);

  Serial.print("ACCEL_CONFIG: 0x");
  Serial.println(accelConfig, HEX);

  Serial.print("GYRO_CONFIG:  0x");
  Serial.println(gyroConfig, HEX);

  Serial.println();

  if (whoAmI == 0x68) {
    Serial.println("Standard MPU6050 identity detected.");
  } else if (whoAmI == 0x70) {
    Serial.println("Device responded with WHO_AM_I = 0x70.");
    Serial.println("Measurement registers can still be tested.");
  } else {
    Serial.println("Unexpected WHO_AM_I value.");
  }
}

void loop() {
  byte whoAmI = readRegister(WHO_AM_I);

  Serial.print("WHO_AM_I = 0x");
  if (whoAmI < 16) Serial.print("0");
  Serial.println(whoAmI, HEX);

  delay(2000);
}
