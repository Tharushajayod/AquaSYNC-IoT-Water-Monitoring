# 💧 AquaSYNC: Smart IoT Water Quality & Level Management System

*Sense the flow. Save the future.* **AquaSYNC** is an advanced, integrated solution for automated water tank management. Combining precision hardware sensing with cutting-edge Internet of Things (IoT) technology, AquaSYNC ensures your water supply is continuously monitored, managed, and maintained at optimal levels and quality. 

This dual-microcontroller system utilizes an **Arduino Uno** for master sensor data processing and an **ESP32** as an IoT gateway to interface with the Blynk 2.0 platform.

---

## ✨ Key Features

* **Advanced Pump Automation:** Fully autonomous logic to control both the main water pump and the pH dosing pump based on real-time sensor data.
* **Smart Alarms & Indicators (LED & Buzzer):** Comprehensive visual and audio alert system to notify users of critical water levels and temperature warnings instantly.
* **Dual-Tank Ultrasonic Monitoring:** Real-time level measurement of both the Main storage tank and the Source tank.
* **Water Quality Sensing:** Precision monitoring of water pH level and Temperature.
* **Hardware Safety Interlocks:** The pH pump is logically disabled whenever the high-voltage water pump is running to ensure safe and accurate dosing.
* **IoT Integration (Blynk 2.0):** Remote real-time monitoring and control through a custom mobile dashboard.
* **Multi-Mode Operation & Overrides:** Switch seamlessly between Automatic and Manual control modes via the app, or use physical toggle switches to bypass software logic in emergencies.

---

## 🛠️ Hardware Components

The system integrates the following primary components:

1. **Main Controller:** Arduino Uno 
2. **IoT Gateway:** ESP32 
3. **Display:** 20x4 LCD Display 
4. **Level Sensors:** HC-SR04 Ultrasonic Sensors 
5. **Quality Sensors:** SEN0161 pH Sensor & DS18B20 Waterproof Temperature Sensor 
6. **Actuators:** Opto-isolated Relay Modules controlling a 230V Water Pump (P1) and a low-voltage Dosing Pump (P2) 
7. **Indicators:** Status LEDs (Green, Red, Yellow) and Buzzers 
8. **Switches:** Industrial-grade hardware override toggle switches 

---

## ⚙️ Core Automation Logic

AquaSYNC is designed to run completely autonomously. The core logic dictating the pump operations is outlined below:

### Water Pump (P1) Control Logic 
| Mode | Condition | Result |
| :--- | :--- | :--- |
| **AUTO** | Main Tank < 20% AND Source > 10% | **Pump starts (ON)** to refill storage. |
| **AUTO** | Main Tank > 80% OR Source < 10% | **Pump stops (OFF)** to prevent overflow or dry running. |
| **MANUAL** | App Power Switch activated | Direct control via App, regardless of sensors. |
| **OVERRIDE** | Physical switch on box set to ON | Hardware activation. Bypasses all software. |

### pH Dosing Pump (P2) Control Logic 
| Mode | Condition | Result |
| :--- | :--- | :--- |
| **ANY** | Water Pump (P1) is running | **P2 IS AUTOMATICALLY DISABLED.** (Safety Lock) |
| **AUTO** | P1 is OFF AND (pH < 6 OR pH > 8) | **Dosing pump starts (ON)** to correct water quality. |
| **AUTO** | pH is between 6 and 8 | **Pump stops (OFF)**. |
| **MANUAL** | P1 is OFF AND App Switch activated | Direct control via App. |

---

## 🚨 Smart Alarms (LED & Buzzer Logic)

The system features a robust physical alert mechanism for critical conditions, utilizing LEDs and Buzzers to keep users informed without needing to check the app:

* **🟢 System OK:** Green LED ON, No Sound. (All parameters are within normal ranges).
* **🔴 Source Tank Low (<10%):** Red LED ON, **Fast Beeping** Buzzer. (Pump P1 is auto-disabled to protect the motor).
* **🔴 Main Tank Very Low (<5%):** Red LED ON, **Slow Beeping** Buzzer. (Immediate refill required).
* **🟡 High Temperature (>30°C):** Yellow LED ON, **Intermittent Beep**. (Water temperature has exceeded safe limits).

---

## 📱 IoT Dashboard (Blynk)

The system connects to the internet via Wi-Fi to provide a comprehensive mobile dashboard. The dashboard includes visual gauges for tank capacities and water quality, LED status indicators for both pumps, and push notifications for critical alerts (e.g., "Warning: Source Tank water is below 10%!").

---

## 📂 Repository Structure

* `/AquaSYNC_ESP32` - Contains the logic, Wi-Fi, automation, and Blynk integration code for the ESP32.
* `/AquaSYNC_Uno` - Contains the sensor reading algorithms and LCD formatting logic for the Arduino Uno.

***Designed & Developed by Team AQUA SYNC***
