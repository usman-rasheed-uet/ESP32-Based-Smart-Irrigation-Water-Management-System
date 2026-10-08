# 🌱 Smart Irrigation & Water Management System

An embedded IoT-based smart irrigation system designed to monitor **soil moisture** and **water-tank level** while automatically controlling a water pump for plant irrigation.

The system uses an **ESP32** as the main controller and an **I2C OLED display** for real-time monitoring.

## 🚀 Project Overview

The system continuously monitors:

- 🌱 Soil moisture level
- 💧 Water availability in the storage tank
- ⚙️ Water pump status
- 📟 Real-time system information on an OLED display

Based on the soil and water conditions, the controller determines whether irrigation is required and operates the pump accordingly.

## 🧠 System Logic

```text
        ┌─────────────────┐
        │  Soil Moisture  │
        │     Sensor      │
        └────────┬────────┘
                 │
                 ▼
        ┌─────────────────┐
        │                 │
        │      ESP32      │
        │   Controller    │
        │                 │
        └─────┬─────┬─────┘
              │     │
       ┌──────┘     └─────────────┐
       ▼                          ▼
 ┌───────────┐              ┌───────────┐
 │ Water     │              │   OLED    │
 │ Level     │              │ Display   │
 │ Sensor    │              └───────────┘
 └───────────┘
       │
       ▼
 ┌─────────────┐
 │ Relay /     │
 │ Pump Control│
 └──────┬──────┘
        │
        ▼
   💧 Water Pump
        │
        ▼
      Plants
```

## ⚙️ Main Features

- Automatic soil-moisture monitoring
- Water-tank level monitoring
- Automatic pump control
- OLED-based status display
- ESP32-based embedded control
- Relay-controlled 12 V water pump
- Buzzer and LED status indication
- Expandable toward IoT/mobile monitoring

## 🔧 Hardware

| Component | Purpose |
|---|---|
| ESP32 Dev Module | Main controller |
| Soil Moisture Sensor | Measures soil moisture |
| Water Level Sensor | Monitors tank water level |
| 0.96" SSD1306 OLED | System display |
| 5 V Relay Module | Pump switching |
| 12 V Water Pump | Irrigation |
| Buzzer | Audible status indication |
| LEDs | Visual status indication |
| External Power Supply | Pump/load power |

## 📌 ESP32 Pin Configuration

| Function | GPIO |
|---|---:|
| Soil Moisture Sensor | GPIO34 |
| Water Level Sensor | GPIO35 |
| Pump Relay | GPIO26 |
| Buzzer | GPIO27 |
| Green LED | GPIO25 |
| Blue LED | GPIO32 |
| Red LED | GPIO33 |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |

> Pin assignments can be modified according to the final hardware implementation.

## 🔄 Operating Principle

1. The ESP32 reads the soil moisture sensor.
2. The water-tank level is monitored simultaneously.
3. Sensor values are displayed on the OLED.
4. If the soil requires irrigation and sufficient water is available, the pump is activated.
5. Water is supplied to the plants.
6. Once the required moisture condition is reached, the pump is switched OFF.
7. The system continuously repeats the monitoring cycle.

## 📊 Example Sensor Conditions

### Dry Soil

```text
Soil Moisture → LOW
Tank Level    → Available
Pump          → ON
```

### Moist Soil

```text
Soil Moisture → GOOD
Tank Level    → Available
Pump          → OFF
```

### Low Tank Level

```text
Tank Level → LOW
Pump       → OFF
Warning    → Activated
```

## 💻 Software

- Arduino IDE
- Embedded C/C++
- ESP32 Arduino Framework
- SSD1306 OLED library

## 🔮 Future Improvements

- 📱 Mobile/web-based pump control
- ☁️ Cloud monitoring
- 📈 Historical moisture data
- 🌦️ Weather-based irrigation
- 🔔 Remote notifications
- 🔋 Solar-powered operation
- 🤖 AI-based irrigation prediction

## 🎯 Applications

- Smart agriculture
- Home gardening
- Greenhouses
- Plant nurseries
- Automated irrigation
- Water conservation systems

## 👨‍💻 Project Focus

This project demonstrates the integration of:

**Embedded Systems + Sensors + IoT + Automation + Control Systems**

---

## 📜 License

This project is intended for educational, research, and portfolio purposes.
