# Aquaculture-Health-Monitoring-System
IoT Based Real-Time Aquaculture Health Monitoring System using Esp32
# Aquaculture Health Monitoring and Alert System

## Overview
The Aquaculture Health Monitoring and Alert System is an ESP32-based embedded project designed to monitor water quality parameters and identify abnormal conditions in an aquaculture environment. The system monitors pH, Total Dissolved Solids (TDS), and oxygen status using an IR sensor-based detection method. It displays sensor readings on an LCD and activates a buzzer and fan during low-oxygen status. Wi-Fi connectivity is used to send email alerts when abnormal conditions are detected.

## Features
- Real-time pH monitoring.
- Total Dissolved Solids (TDS) monitoring.
- IR sensor-based oxygen status detection.
- 16×2 LCD display with I2C interface.
- Automatic fan activation during low-oxygen status.
- Buzzer alert during abnormal oxygen status.
- Wi-Fi connectivity.
- Email notifications using SMTP (Gmail).
- Multitasking using FreeRTOS.
- Periodic sensor readings and LCD updates.
- Alert cooldown mechanism to reduce repeated email notifications.

## Hardware Components
- ESP32 Microcontroller
- pH Sensor
- TDS Sensor
- IR Sensor (oxygen status detection)
- 16×2 LCD Display with I2C module
- Buzzer
- Fan
- Power supply
- Connecting wires

## Pin Configuration

| Component | ESP32 Pin |
|---|---|
| pH Sensor | GPIO 34 |
| TDS Sensor | GPIO 35 |
| IR Sensor | GPIO 32 |
| Buzzer | GPIO 25 |
| Fan | GPIO 26 |
| LCD (I2C SDA/SCL) | Default ESP32 I2C pins |

## Software & Technologies
- Embedded C/C++ (Arduino framework)
- ESP32
- Arduino IDE
- FreeRTOS
- Wi-Fi
- SMTP Email Communication
- I2C Communication
- Sensor Interfacing

## Working Principle
1. The ESP32 reads analog values from the pH and TDS sensors.
2. Sensor readings are converted into estimated pH and TDS values using programmed conversion logic.
3. An IR sensor is used to identify the oxygen-status condition.
4. Sensor readings and oxygen status are displayed on the LCD.
5. When oxygen status indicates LOW, the system activates the fan and buzzer.
6. When monitored parameters exceed their configured limits, an alert message is generated.
7. The ESP32 connects to Wi-Fi and sends email notifications through Gmail SMTP.
8. Email sending is handled through a separate FreeRTOS task to avoid blocking the main monitoring loop for the entire email operation.

## Monitoring Thresholds

| Parameter | Configured Condition |
|---|---|
| pH | Below 6.5 or above 8.5 |
| TDS | Above 500 |
| Oxygen Status | Detected as LOW by IR sensor |

*Threshold values are configurable in the source code.*

## LCD Display
The LCD alternates between displaying:

- pH and TDS readings
- Oxygen status (OK / LOW)

The display refreshes periodically to provide updated monitoring information.

## Alert System
Alerts are triggered when:
- pH falls outside the configured range.
- TDS exceeds the configured limit.
- Oxygen status is detected as LOW.

During low-oxygen status, the fan and buzzer are activated. Email alerts are sent to the configured recipient, with a cooldown interval of 60 seconds between alert attempts.

## Libraries Used
- WiFi.h
- ESP_Mail_Client.h
- Wire.h
- LiquidCrystal_PCF8574.h
- time.h

## Project Structure
```text
Aquaculture-Health-Monitoring-System/
│
├── Aquaculture_Health_Monitoring.ino
├── README.md
└── images/
    ├── circuit_diagram.png
    └── project_setup.jpg
```

## Applications
- Aquaculture water quality monitoring
- Fish farming
- Smart fish ponds
- Embedded environmental monitoring
- IoT-based aquaculture management

## Future Enhancements
- Integration of a dedicated dissolved oxygen sensor.
- Cloud-based data logging and visualization.
- Mobile application for remote monitoring.
- Improved sensor calibration and measurement accuracy.
- Automated water quality control mechanisms.
