<p align="center">
  <img src="https://github.com/user-attachments/assets/fda74bc5-b167-4562-8f9b-aa43e8d8ba85" width="250" alt="AquaSYNC Logo">
</p>
<div align="center">

<br/>

```
   █████╗  ██████╗ ██╗   ██╗ █████╗ ███████╗██╗   ██╗███╗   ██╗ ██████╗
  ██╔══██╗██╔═══██╗██║   ██║██╔══██╗██╔════╝╚██╗ ██╔╝████╗  ██║██╔════╝
███████║██║   ██║██║   ██║███████║███████╗ ╚████╔╝ ██╔██╗ ██║██║
██╔══██║██║▄▄ ██║██║   ██║██╔══██║╚════██║  ╚██╔╝  ██║╚██╗██║██║
  ██║  ██║╚██████╔╝╚██████╔╝██║  ██║███████║   ██║   ██║ ╚████║╚██████╗
  ╚═╝  ╚═╝ ╚══▀▀═╝  ╚═════╝ ╚═╝  ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═══╝ ╚═════╝
```

### Intelligent IoT Water Quality & Volume Management System

<br/>

[![Arduino](https://img.shields.io/badge/Arduino_Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white)](https://www.espressif.com/)
[![Blynk](https://img.shields.io/badge/Blynk_2.0-15B876?style=for-the-badge&logo=icloud&logoColor=white)](https://blynk.io/)
[![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![MIT License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)](LICENSE)
[![SLIIT](https://img.shields.io/badge/SLIIT-IT1140_IoT-003366?style=for-the-badge)](https://www.sliit.lk/)

<br/>

> *"Sense the flow. Save the future."*

**AquaSYNC** is a fault-tolerant, dual-microcontroller IoT system that fully automates water tank ecosystems — preventing overflow, dry-runs, and chemical imbalances through real-time edge intelligence and cloud telemetry.

<br/>

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [System Architecture](#-system-architecture)
- [Key Engineering Innovations](#-key-engineering-innovations)
- [Hardware Specifications](#-hardware-specifications)
- [Core Automation Engine](#-core-automation-engine)
  - [Volume Control](#volume-control)
  - [Quality Control](#quality-control)
  - [Local Diagnostics](#local-diagnostics)
- [Getting Started](#-getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Configuration](#configuration)
- [Project Structure](#-project-structure)
- [Team](#-team)
- [Future Roadmap](#-future-roadmap)
- [Acknowledgments](#-acknowledgments)

---

## 🌊 Overview

AquaSYNC is a highly fault-tolerant, dual-microcontroller IoT solution engineered to fully automate water tank ecosystems. The system continuously monitors **water volume**, **pH levels**, and **temperature** across multiple tanks — providing real-time telemetry and autonomous corrective actions accessible globally via a Blynk Cloud dashboard.

<br/>

<div align="center">

| Feature | Description |
|:---|:---|
| 🪣 **Dual-Tank Monitoring** | Independent volume tracking for main storage and source tanks |
| 🧪 **pH Quality Control** | Automated chemical dosing with hardware safety interlocks |
| 🌡️ **Temperature Sensing** | Thermal monitoring with multi-stage alerting |
| ⚙️ **Autonomous Operation** | Fully automated pump control with manual override capability |
| ☁️ **Cloud Connectivity** | Real-time dashboard accessible from any device, anywhere |
| 📟 **Local Feedback** | 20×4 LCD display and 6-LED audio/visual alert matrix |

</div>

---

## 🏗️ System Architecture

AquaSYNC employs a **distributed processing architecture** that separates real-time sensor acquisition from network-intensive cloud operations.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          EDGE LAYER                                      │
│                                                                          │
│   ┌──────────────────┐   UART Serial   ┌──────────────────────────┐    │
│   │   Arduino Uno    │ ──────────────► │         ESP32            │    │
│   │  (Sensor Hub)    │                 │   (Gateway & Control)    │    │
│   │                  │                 │                          │    │
│   │  • HC-SR04 (×2)  │                 │  • DS18B20 (1-Wire)      │    │
│   │  • pH Probe      │                 │  • Relay Module (×2)     │    │
│   │  • EMI Filtering │                 │  • Wi-Fi 2.4GHz          │    │
│   └──────────────────┘                 └──────────┬───────────────┘    │
│                                                    │                    │
└────────────────────────────────────────────────────┼────────────────────┘
                                                     │
          ┌──────────────────────────────────────────┼──────────────────┐
          │                CLOUD LAYER               │                  │
          │                                          ▼                  │
          │                              ┌──────────────────────┐       │
          │                              │   Blynk 2.0 Cloud    │       │
          │                              │   (WebSocket Sync)   │       │
          │                              └──────────┬───────────┘       │
          │                                         │                   │
          │                              ┌──────────▼───────────┐       │
          │                              │  Mobile & Web Dashboard│      │
          │                              └──────────────────────┘       │
          └──────────────────────────────────────────────────────────────┘
          
          ┌──────────────────────────────────────────────────────────────┐
          │              LOCAL INTERFACE LAYER                           │
          │     20×4 I2C LCD Display  │  6-LED Matrix  │  Dual Buzzer   │
          └──────────────────────────────────────────────────────────────┘
```

### Data Flow

```
[Arduino Uno] ──(1Hz polling)──► Ultrasonic + pH ──► Noise Filter ──► UART Packet
     ↓
[ESP32] ──► Reads DS18B20 + UART stream ──► Evaluates automation logic ──► Relay output
     ↓
[Blynk Cloud] ──► Syncs virtual pins ──► Mobile / Web Dashboard
     ↓
[Local Interface] ──► LCD + LEDs + Buzzer (network-independent)
```

---

## ⚡ Key Engineering Innovations

<details>
<summary><strong>🔀 Dual-MCU Architecture</strong></summary>
<br/>

The system offloads heavy analog sensor processing to the **Arduino Uno** while utilizing the **ESP32** as an agile IoT gateway. This separation prevents processing bottlenecks during concurrent Wi-Fi transmission and analog-to-digital conversion, ensuring deterministic sensor polling intervals even under high network load.

</details>

<details>
<summary><strong>🛡️ EMI Noise Suppression</strong></summary>
<br/>

Custom firmware implements a **digital isolator pattern**. When the 230V water pump activates, analog sensor polling is suspended for **200ms** to protect against electromagnetic interference induced on sensor lines. This eliminates false data spikes that would otherwise trigger erroneous automation decisions.

</details>

<details>
<summary><strong>🔒 Hardware Safety Interlocks</strong></summary>
<br/>

The pH dosing pump is governed by a **hardcoded safety latch** that immediately disables dosing operation whenever the high-voltage main pump is active. This prevents catastrophic chemical dosing errors during tank refill cycles and guarantees operator safety by design — not just by software.

</details>

---

## 🔧 Hardware Specifications

<div align="center">

| Component | Model | Specification | Function |
|:---|:---|:---|:---|
| **MCU — Gateway** | ESP32-WROOM-32 | 240MHz dual-core, Wi-Fi/BT | Cloud integration & relay control |
| **MCU — Sensor Hub** | Arduino Uno R3 | ATmega328P, 16MHz | Analog acquisition & filtering |
| **Distance Sensor** | HC-SR04 × 2 | 2cm – 400cm, ±3mm | Non-contact water level measurement |
| **Chemical Sensor** | pH Probe + BNC | 0–14 pH, ±0.1 accuracy | Water quality monitoring |
| **Temperature Sensor** | DS18B20 Waterproof | −55°C to +125°C, ±0.5°C | Thermal monitoring via 1-Wire |
| **Local Display** | 20×4 I2C LCD | 4-row × 20-char, I2C bus | Real-time on-site status |
| **Relay Module** | 2-Ch Opto-Isolated | 230V AC / 12V DC switching | Pump actuation |
| **Power Supply** | 5V / 2A External | Regulated DC | Stable dual-MCU power delivery |

</div>

### 📌 Pin Mapping

<div align="center">

**ESP32 Pinout**

| GPIO Pin | Signal | Notes |
|:---|:---|:---|
| GPIO 16 (RX2) | UART RX from Arduino | Serial data reception |
| GPIO 17 (TX2) | UART TX to Arduino | Serial data transmission |
| GPIO 4 | DS18B20 Data | 1-Wire protocol, 4.7kΩ pull-up |
| GPIO 18 | Main Pump Relay | 230V AC control |
| GPIO 19 | Dosing Pump Relay | 12V DC control |
| GPIO 21 | I2C SDA (LCD) | Display data line |
| GPIO 22 | I2C SCL (LCD) | Display clock line |

**Arduino Uno Pinout**

| Pin | Signal | Notes |
|:---|:---|:---|
| TX (Pin 1) | UART TX to ESP32 | Serial data transmission |
| RX (Pin 0) | UART RX from ESP32 | Serial data reception |
| A0 | pH Sensor Analog | 0–5V analog input |
| D9 / D10 | HC-SR04 #1 (Trig/Echo) | Main tank ultrasonic |
| D11 / D12 | HC-SR04 #2 (Trig/Echo) | Source tank ultrasonic |

</div>

---

## ⚙️ Core Automation Engine

AquaSYNC operates in **fully autonomous mode** with deterministic state transitions based on real-time environmental thresholds.

### Volume Control

> Controls the **230V main water pump** to maintain optimal tank levels.

| Condition | Trigger Logic | System Response |
|:---|:---|:---|
| 🟢 **Auto-Refill** | Main tank < 20% **AND** source tank > 10% | `PUMP ON` — Begin refill cycle |
| 🔴 **Safety Stop** | Main tank > 80% **OR** source tank < 10% | `PUMP OFF` — Prevent overflow/dry-run |
| 🟡 **Manual Override** | Hardware toggle or app command | Software logic bypassed |

### Quality Control

> Controls the **12V pH dosing pump** to maintain safe water chemistry.

| Condition | Trigger Logic | System Response |
|:---|:---|:---|
| 🧪 **Dose Active** | Main pump **OFF** AND (pH < 6.0 OR pH > 8.0) | `DOSING ON` — Chemical correction |
| ✅ **Ideal Range** | pH ≥ 6.0 AND pH ≤ 8.0 | `DOSING OFF` — Chemistry balanced |
| 🔒 **Safety Lock** | Main pump **ON** | `DOSING DISABLED` — Interlock engaged |

### Local Diagnostics

> On-site feedback via **6-LED matrix** and **dual active buzzers** — fully network-independent.

| LED Color | Behaviour | Condition |
|:---|:---|:---|
| 🟢 **Green** | Steady ON | Main tank ≥ 80% (optimal fill) |
| 🔴 **Red** | Flashing + Beep | Critical water level detected |
| 🔵 **Blue** | Steady ON | Temperature within safe range |
| 🟡 **Yellow** | Flashing | Thermal warning: temperature > 30°C |
| ⚪ **White** | Steady ON | pH balanced (6.0–8.0) |
| 🟠 **Orange** | Flashing | pH imbalance outside safe range |

> **Note:** Beep frequency distinguishes between source tank and main tank critical alerts.

---

## 🚀 Getting Started

### Prerequisites

Before you begin, ensure you have the following:

- [ ] **Arduino IDE** 1.8.x or later *(or PlatformIO)*
- [ ] USB data cable for programming
- [ ] **5V/2A minimum** external power supply *(required for stable dual-MCU operation)*
- [ ] Active **Blynk account** *(free tier supported)*
- [ ] **2.4GHz Wi-Fi** network credentials

### Installation

#### Step 1 — Install Library Dependencies

Open Arduino IDE → **Sketch → Include Library → Manage Libraries** and install:

| Library | Min. Version | Purpose |
|:---|:---|:---|
| `Blynk` by Volodymyr Shymanskyy | ≥ 1.1.0 | Blynk 2.0 cloud connectivity |
| `OneWire` by Paul Stoffregen | ≥ 2.3.0 | 1-Wire protocol for DS18B20 |
| `DallasTemperature` by Miles Burton | ≥ 3.9.0 | Temperature sensor parsing |
| `LiquidCrystal I2C` by Frank de Brabander | ≥ 1.1.2 | 20×4 LCD over I2C bus |

#### Step 2 — Configure Credentials

Edit `AquaSYNC_ESP32/config.h` with your credentials:

```cpp
// ── Blynk Cloud Configuration ──────────────────────────────────────────
#define BLYNK_TEMPLATE_ID     "TMPLxxxxxxxxx"   // From Blynk Console
#define BLYNK_TEMPLATE_NAME   "AquaSYNC"
#define BLYNK_AUTH_TOKEN      "YourAuthTokenHere"

// ── Wi-Fi Credentials ───────────────────────────────────────────────────
const char* WIFI_SSID = "YourNetworkName";
const char* WIFI_PASS = "YourNetworkPassword";
```

> 💡 Obtain your `BLYNK_AUTH_TOKEN` from [Blynk Console](https://blynk.cloud) after creating your device template.

#### Step 3 — Flash Firmware

> ⚠️ **CRITICAL:** Disconnect RX ↔ TX jumper wires between Arduino Uno and ESP32 **before** flashing either board.

```
1. Open  AquaSYNC_Uno/AquaSYNC_Uno.ino
   Board: Arduino Uno  →  Upload

2. Open  AquaSYNC_ESP32/AquaSYNC_ESP32.ino
   Board: ESP32 Dev Module  →  Upload

3. Power off both boards
   Reconnect RX ↔ TX jumpers
   Apply stable 5V/2A external power

4. Monitor Serial @ 115200 baud to verify successful initialization
```

---

## 📁 Project Structure

```
AquaSYNC/
│
├── 📂 AquaSYNC_ESP32/              # ESP32 gateway firmware
│   ├── AquaSYNC_ESP32.ino          # Main gateway sketch
│   ├── config.h                    # Wi-Fi and Blynk credentials
│   ├── relays.h                    # Pump control & interlock logic
│   └── cloud.h                     # Blynk virtual pin handlers
│
├── 📂 AquaSYNC_Uno/                # Arduino sensor hub firmware
│   ├── AquaSYNC_Uno.ino            # Main sensor hub sketch
│   ├── sensors.h                   # Ultrasonic and pH acquisition
│   └── filters.h                   # EMI noise suppression algorithms
│
├── 📂 docs/                        # Project documentation
│   ├── proposal.pdf
│   ├── progress-report.pdf
│   └── final-report.pdf
│
├── 📂 assets/                      # Media assets
│   ├── aquasync-banner.png
│   ├── hardware-front.jpg
│   ├── hardware-internal.jpg
│   ├── blynk-dashboard.jpg
│   └── lcd-display.jpg
│
├── 📂 schematics/                  # Circuit diagrams
│   ├── wiring-diagram.pdf
│   └── pcb-layout.pdf
│
└── README.md
```

---

## 👥 Team

<div align="center">

*AquaSYNC — IT1140 Internet of Things | SLIIT*

</div>

<br/>

<div align="center">

| Student ID | Core Contribution | Details |
|:---:|:---|:---|
| **IT25100021** | ⚙️ Hardware Integration & Pump Control | Set up the physical water transport system by connecting pumps and relays to the ESP32, ensuring reliable actuation logic |
| **IT25100204** | 📟 LCD Interface & Display Logic | Wired and programmed the 20×4 LCD display to render real-time sensor data streamed from the ESP32 |
| **IT25100393** | 📡 Ultrasonic Sensing & Cloud Dashboard | Calibrated the HC-SR04 ultrasonic sensors for accurate tank level calculation and engineered the Blynk IoT dashboard |
| **IT25101860** | 🧪 pH Sensing & Water Quality Analytics | Integrated the pH sensor module to monitor water chemistry and transmit quality data readings to the ESP32 |
| **IT25101879** | 🌡️ Temperature Sensing & Data Transmission | Connected and configured the DS18B20 waterproof temperature probe and implemented reliable 1-Wire data transmission to the ESP32 |

</div>

---

## 🗺️ Future Roadmap

| Phase | Feature | Description | Timeline |
|:---:|:---|:---|:---:|
| **v2.0** | 🧂 TDS Sensor Integration | Total Dissolved Solids monitoring for comprehensive water purity assessment | Q3 2024 |
| **v2.1** | 📊 Historical Telemetry | Blynk SuperCharts for 30-day rolling data visualization and trend analysis | Q4 2024 |
| **v3.0** | 🤖 Predictive Analytics | ML-based tank depletion forecasting using historical usage pattern analysis | Q2 2025 |
| **v3.1** | 🔗 Multi-Tank Expansion | Support for up to 4 independent tank zones with distributed sensor nodes | Q4 2025 |

---

## 🙏 Acknowledgments

This project was developed as part of the **IT1140 — Internet of Things** coursework at the **Sri Lanka Institute of Information Technology (SLIIT)**.

Special thanks to the faculty of the **Department of Information Technology** for their guidance and technical oversight throughout the development lifecycle.

---

<div align="center">

<br/>

**AquaSYNC** — Engineered with precision at SLIIT.

<br/>

[![Made with Arduino](https://img.shields.io/badge/Made%20with-Arduino-00979D?style=flat-square&logo=arduino)](https://www.arduino.cc/)
[![Powered by Blynk](https://img.shields.io/badge/Powered%20by-Blynk-15B876?style=flat-square)](https://blynk.io/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow?style=flat-square)](LICENSE)

<br/>

*© 2024 AquaSYNC Team — SLIIT, Sri Lanka*

</div>
