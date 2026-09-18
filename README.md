# Part 1: Hardware & Sensor Validation

The first stage of my TinyML Research Reproduction project.

This stage focuses on validating the sensing hardware and establishing a reliable embedded data pipeline before moving into vibration signal processing and TinyML inference.

## Hardware

- ESP32-S3 N8R8
- MPU6050 IMU
- 1.3" 128×64 SH1106 OLED

## I²C Configuration

| Device | Address | SDA | SCL |
|---|---|---|---|
| MPU6050 | 0x68 | GPIO 11 | GPIO 12 |
| SH1106 OLED | 0x3C | GPIO 11 | GPIO 12 |

Both devices share the same I²C bus.

## Development Progression

### 01 · I²C Scanner

Verified communication between the ESP32-S3 and connected I²C devices.

### 02 · MPU6050 Register Test

Validated direct communication with the MPU6050 registers.

The sensor responded at:

`0x68`

The device returned:

`WHO_AM_I = 0x70`

Although this differed from the commonly expected value, the sensor's measurement and configuration registers responded correctly.

### 03 · Raw IMU

Implemented direct acquisition of:

- Accelerometer X/Y/Z
- Gyroscope X/Y/Z

### 04 · OLED Orientation

Implemented:

- Gyroscope calibration
- Accelerometer-based roll/pitch estimation
- Gyroscope integration
- Complementary filtering
- Roll/Pitch/Yaw estimation
- 3D wireframe cube rendering
- SH1106 OLED visualization

The complementary filter uses:

`α = 0.98`

## Libraries

- Wire
- Adafruit GFX
- Adafruit SH110X

The MPU6050 is accessed directly through its registers.

## Final Hardware Pipeline

MPU6050

↓

Accelerometer + Gyroscope

↓

Sensor Fusion

↓

Roll / Pitch / Yaw

↓

3D Renderer

↓

SH1106 OLED

## Repository Contents

`01-i2c-scanner/`  
I²C device detection.

`02-mpu6050-register-test/`  
Low-level MPU6050 register validation.

`03-raw-imu/`  
Raw accelerometer and gyroscope acquisition.

`04-oled-orientation/`  
Final ESP32-S3 orientation visualization.

`hardware/`  
Prototype and wiring documentation.

`results/`  
Captured output and implementation results.

## Result

The ESP32-S3 can continuously acquire MPU6050 motion data, process the measurements using sensor fusion, and visualize the resulting orientation as a real-time 3D wireframe cube on the SH1106 OLED.

## Next Stage

Part 2 will move from orientation visualization to vibration signal processing:

100 Hz Sampling  
→ Windowing  
→ 16-point FFT  
→ Frequency Analysis  
→ Feature Extraction
