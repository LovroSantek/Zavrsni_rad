# Wireless Real-Time Human Motion Tracking System Using IMU Sensors

## Project Overview
This project demonstrates the development of a **wireless system for tracking human motion in real-time**. It uses **IMU (Inertial Measurement Unit) sensors** to precisely monitor and visualize the orientation of body parts or objects. The system is designed to provide real-time feedback on the orientation of a human hand but is adaptable for other applications.

### Key Features:
- **Real-Time Tracking**: The system provides live updates of motion data.
- **Wireless Communication**: Utilizes ESP32 microcontroller with Wi-Fi capabilities for seamless data transfer.
- **Visualization**: Displays orientation using a 3D model in the Unity development environment.
- **High Precision**: Combines accelerometer, gyroscope, and optional magnetometer data using advanced AHRS (Attitude and Heading Reference System) algorithms.

---

## Demo
Check out the demo of the project on [YouTube](https://youtu.be/GuiRu1RaZ3c).

---

## Hardware Components
1. **ESP32 Microcontroller**: Acts as the core processing and communication unit.
2. **ICM-20948 IMU Sensor** (on 9DOF 2 Click board): Measures:
   - Acceleration
   - Angular velocity
   - Magnetic field strength (not used in this project but available).
3. **Power Supply**: Stabilized 5V DC.

---

## Software Architecture
### Development Tools
- **ESP-IDF Framework**: Used to program the ESP32 in C.
- **Unity**: Used for real-time visualization in C#.
  
### Key Modules
1. **Sensor Data Processing**:
   - Raw IMU data is converted to orientation using the **AHRS Fusion Algorithm**.
   - Euler angles (roll, pitch, yaw) are calculated for visualization.
2. **Wireless Communication**:
   - Uses Wi-Fi for sending data to a local PC.
   - Data packets are transferred via HTTP in CSV format.
3. **3D Visualization**:
   - Unity-based application displays a 3D model that mimics real-time motion.

---

## Setup Instructions
### Hardware Setup
1. Connect the **ICM-20948 sensor** to the **ESP32** via SPI:
   - MOSI, MISO, SCLK, CS, VCC, GND.
2. Power the ESP32 using a stable 5V source.

### Software Setup
#### ESP32 Configuration
1. Install **ESP-IDF Framework** in Visual Studio Code.
2. Clone this repository and build the firmware:
   ```bash
   idf.py build
   idf.py flash
   ```
3. Configure Wi-Fi credentials in `wifi_init()`:
   ```c
   #define WIFI_SSID "Your_SSID"
   #define WIFI_PASS "Your_PASSWORD"
   ```
4. Flash the program to the ESP32 board.

#### Unity Visualization
1. Open the Unity project in the `/visualization` directory.
2. Set the ESP32 IP address in the Unity script.
3. Start the Unity simulation.

---

## Usage
1. Attach the sensor node (ESP32 + IMU) to the object/body part you want to track.
2. Power on the ESP32, and it will automatically connect to the configured Wi-Fi network.
3. Open the Unity application to view real-time motion tracking.

---
