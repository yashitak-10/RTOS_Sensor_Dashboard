# RTOS-Based EV Thermal Monitoring System with AI Risk Prediction

## Project Overview
An embedded systems project simulating a Battery Management System (BMS) 
for Electric Vehicles, built on ESP32 using FreeRTOS and AI-driven 
thermal risk prediction.
This project mirrors how real EV Battery Management Systems monitor 
battery temperature and predict thermal runaway risk in real time. 
It combines embedded RTOS architecture with a machine learning model 
trained on simulated EV thermal data.

## System Architecture

| Component | Role |
|-----------|------|
| ESP32 | Main microcontroller |
| DHT22 | Battery temperature + humidity sensor |
| Potentiometer | Motor load simulation |
| OLED Display | Real-time dashboard |
| Buzzer | Critical alert system |
| FreeRTOS | Multi-task scheduler |
| TinyML Model | Thermal risk prediction |

## FreeRTOS Task Structure

| Task | Function | Priority | Interval |
|------|----------|----------|----------|
| Battery Sensor | Reads DHT22 temperature and humidity | 1 | 2000ms |
| Motor Speed | Reads potentiometer load value | 1 | 500ms |
| AI Prediction | Runs thermal risk inference | 2 | 1000ms |
| Display Update | Updates OLED + triggers buzzer | 3 | 1000ms |

## AI/ML Component

A Random Forest classifier was trained on 1000 simulated EV thermal 
data points in Google Colab. Feature importance analysis showed 
temperature drives 98.23% of prediction accuracy. The trained model 
thresholds were extracted and deployed as an inference function 
directly on the ESP32 — a technique known as model distillation, 
widely used in automotive embedded systems.

### Risk Levels

| Risk | Temperature | Action |
|------|-------------|--------|
| SAFE | Below 45°C | Normal operation |
| WARNING | 45°C - 60°C | Caution mode |
| CRITICAL | Above 60°C | Buzzer alert triggered |

Motor load also contributes up to +15°C to effective temperature, 
mirroring real EV thermal behavior under high current draw.

## Real World Relevance

This project simulates core functionality found in production EV systems:
- Tesla BMS monitors cell temperature + current draw to predict thermal risk
- Bosch and Continental use RTOS-based ECU architecture for sensor fusion
- TinyML deployment on microcontrollers is an emerging automotive trend

## Tools and Technologies

- **Hardware:** ESP32 DevKit V1 (simulated via Wokwi)
- **IDE:** VS Code + PlatformIO
- **Simulation:** Wokwi Simulator
- **ML Training:** Google Colab, Scikit-learn
- **Libraries:** FreeRTOS, Adafruit DHT, Adafruit SSD1306
- **Language:** C++ (Arduino framework)

## How to Run

1. Clone this repository
2. Open in VS Code with PlatformIO installed
3. Add `-DWOKWI_SIM` to `build_flags` in `platformio.ini` for simulation
4. Run `pio run` to build firmware
5. Open `diagram.json` to launch Wokwi simulation
6. Press Play in Wokwi Diagram Editor

## Development Notes

This project was developed with partial assistance from Claude AI (Anthropic) 
for code structure and debugging support. The following were done independently:

- Project concept, architecture design, and EV concept
- Hardware component selection and circuit wiring (diagram.json)
- ML dataset design, model training, and feature analysis (Google Colab)
- Debugging and problem solving throughout the build
- Documentation and README writing

Claude assisted with:
- FreeRTOS task boilerplate code structure
- Adafruit library integration
- Resolving ESP32-specific build errors

All code has been independently studied, understood, and is explainable 
in full by the author.

## Author

Yashita Kukreja
ECE Graduate 
Interests: EV Systems, RTOS, Embedded AI, Motorsport Technology
