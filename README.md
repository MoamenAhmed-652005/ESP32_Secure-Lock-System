# ESP32_Secure-Lock-System
# Moamen Secure Lock System v15.0 🔒
### Advanced Embedded Security Terminal | Hybrid Hardware-Software Architecture

Developed by: **Eng. Moamen Ahmed El Sayed**

---

## 🌟 Project Concepts & Philosophy

### 📡 1. Isolated Networking (Web Engine)
* **Access Point Mode:** The ESP32 creates a local, private WiFi network. This ensures the security system is physically isolated from the internet, preventing remote hacking.
* **Captive Admin Dashboard:** A built-in web server provides a responsive UI to manage credentials without needing to re-flash the hardware.

### ⚡ 2. Real-Time Responsiveness (Hardware Interrupts)
* **Concept:** Utilizing Hardware Interrupts (ISRs) for the "Count" button. 
* **The Logic:** This allows the MCU to detect button presses instantly in the background. Even if the system is busy processing web data, it will never miss a physical click.

### 🔌 3. Digital Logic & Hardware Offloading
* **Counter Logic (SN74HC393N):** Offloading the counting process to a dedicated IC reduces CPU overhead and demonstrates distributed logic design.
* **BCD Decoding (SN74LS48N):** Converts binary data to 7-segment format using only 4 pins (BCD), showcasing efficient resource management.
* **Galvanic Isolation (Optocoupler):** Protects the ESP32 from electrical noise or voltage spikes from the external locks.

---

## 🛠 Hardware Specifications
* **MCU:** ESP32 (Dual-Core).
* **Logic ICs:** SN74HC393N (Counter) & SN74LS48N (BCD Decoder).
* **Output Isolation:** PC817 Optocoupler.
* **Interface:** 3-Button Control (Count, Enter, Stealth Power).

---

## 🔌 Pin Mapping Table

| Component | ESP32 Pin | Function |
| :--- | :--- | :--- |
| BCD Data (A-D) | 35, 33, 25, 32 | Reading Binary State |
| Clock Signal | 27 | Incrementing IC Counter |
| Reset Signal | 19 | Zeroing IC Counter |
| Count Button | 17 | User Input (Interrupt) |
| Enter Button | 16 | Digit Confirmation |
| Power Button | 4 | Deep Sleep / Wake Up |
| System Output | 2 | Optocoupler Trigger |

---

## 📂 Repository Structure
* `/src`: Optimized C++ firmware.
* `/hardware`: Altium Designer files (Schematics & PCB).
* `/docs`: Technical Datasheets for components.
* `/media`: Demo video and high-res photos.

---
**License:** MIT License. Designed for professional portfolio demonstration.
