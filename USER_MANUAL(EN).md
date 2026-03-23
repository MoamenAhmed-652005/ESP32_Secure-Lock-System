# 🔐 User Manual: Moamen Secure Lock System

Welcome to the official user manual for the **Moamen Secure Lock System**. This project is a specialized embedded security solution designed for high reliability and ease of use.

---

## 🛠 I. Hardware Controls

### 1. Button Functions:
* **Count Button:** Used to increment the current digit on the display from **0 to 9**.
* **Enter Button:** Confirms the selected digit and moves to the next position. The system requires a **3-digit** security code.
* **Power Button:**
    * **Long Press (3 Seconds):** Enters **Deep Sleep Mode**. This shuts down the ESP32 and Wi-Fi module to conserve power.
    * **Single Press:** Instantly wakes up the system or performs a hardware reset.

### 2. Display Indicators (7-Segment):
* **Continuous Blinking:** The system is active and awaiting user input.
* **Screen OFF:** This indicates the system is either **Unlocked** (successful entry) or in **Power OFF** mode.

---

## 🌐 II. Web Admin Panel

The system features a built-in web server for remote configuration and monitoring.

### 📶 How to Connect:
1.  Search for Wi-Fi networks and connect to: **`Moamen_Secure_Lock`**.
2.  Enter the default network password: **`12345678`**.
3.  Open any web browser (Chrome, Safari, etc.) and go to:
    * `http://moamen.local` (Recommended)
    * `192.168.4.1` (Static IP)

### ✨ Features:
* **Change Access Code:** Remotely update the 3-digit hardware unlock code.
* **Admin Credentials:** Update the web panel's username and password for enhanced security.
* **Live Logs:** View the history of access attempts and system events.

---

## ⚠️ Important Support Note
> In case of forgotten admin credentials or system lockout, please contact the lead developer for a manual factory reset:
> 
> **Developer:** Moamen Ahmed El-Sayed  
> **Institution:** Faculty of Electronic Engineering, Menoufia University.

---
*Generated for the Moamen Secure Lock Project - 2026*
