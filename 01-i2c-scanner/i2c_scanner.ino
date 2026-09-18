#include <Wire.h>

#define SDA_PIN 11
#define SCL_PIN 12

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  Serial.println();
  Serial.println("ESP32-S3 I2C Scanner");
  Serial.println("--------------------");
}

void loop() {
  byte error;
  int devicesFound = 0;

  Serial.println("Scanning I2C bus...");

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      devicesFound++;
    }
  }

  if (devicesFound == 0) {
    Serial.println("No I2C devices found.");
  } else {
    Serial.print("Devices found: ");
    Serial.println(devicesFound);
  }

  Serial.println("--------------------");
  delay(3000);
}
