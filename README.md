# IntroductionToRobotics-Smart-Pomodoro-Privacy-Box
An ESP32-based Pomodoro timer and privacy indicator with RGB LEDs, LCD display, button control, and optional Wi-Fi configuration.

# Smart Pomodoro & Privacy Box

## General Description
The **Smart Pomodoro & Privacy Box** is a desk companion designed to help users focus using the Pomodoro productivity technique while also communicating their availability to others nearby.

When a work session is active, the device displays a countdown timer on an LCD screen and lights up **red** to indicate “Do Not Disturb”. When the session ends, the light turns **green**, signaling availability. A buzzer provides audio feedback at key moments (start/end of sessions).

The system is controlled via a physical button and can optionally be configured through a local web interface hosted on the ESP32. This makes the device both tangible and interactive, combining embedded systems with basic networking.

---

## Bill of Materials (BOM)

| Component | Quantity | Notes |
|---------|----------|------|
| ESP32 Development Board | 1 | Required for Wi-Fi and multitasking |
| WS2812B RGB LEDs (8 LEDs) or 1 RGB LED | 1 | Visual status indicator |
| 16x2 LCD Display with I2C adapter | 1 | Timer display |
| Push Button | 1 | Start / reset timer |
| Active Buzzer | 1 | Audio feedback |
| Resistors, wires, breadboard | — | Prototyping |
| USB cable | 1 | Power & programming |

---

## Tutorial Source
The project is **not directly based on a single tutorial**.

Individual components are inspired by:
- ESP32 Wi-Fi Web Server examples (Espressif documentation)
- Pomodoro technique concept
- LCD and RGB LED library examples

All components are **integrated and adapted** into a unique system.

---

## What Will Be Changed from Tutorials
- Combination of **physical UI + web interface**
- Non-blocking timer logic using `millis()`
- State-based LED behavior (working / break / idle)
- Real-world signaling use case (privacy indicator)

---

### Q1 – What is the system boundary?
The system includes:
- The physical device (ESP32, LEDs, LCD, button, buzzer)
- A local web server running on the ESP32 for configuration

External cloud services are **not required**.

---

### Q2 – Where does intelligence live?
The intelligence lives **locally on the ESP32**.

It manages:
- Time tracking
- State transitions (idle / work / break)
- User input handling
- LED and sound control
- Optional Wi-Fi communication

---

### Q3 – What is the hardest technical problem?
The hardest technical challenge is implementing **non-blocking logic**.

The device must:
- Count down time
- Respond to button presses
- Update the display
- Optionally run a web server  

This requires using `millis()` instead of `delay()` and designing a simple state machine.

---

### Q4 – What is the minimum demo?
The minimum working demo is:
1. Press a button
2. LED turns **red**
3. LCD starts counting down from 25 minutes
4. Buzzer sounds when the timer ends
5. LED turns **green**

---

### Q5 – Why is this not just a tutorial?
Unlike basic tutorials that demonstrate isolated components, this project integrates:
- Timing logic
- State management
- Physical interaction
- Visual and audio feedback
- Networking capabilities  

It represents a **complete embedded system solving a real-world problem**.

---

## Do You Need an ESP32?
**Yes.**

The ESP32 is required for:
- Wi-Fi connectivity
- Running a local web server
- Handling multiple tasks simultaneously  

An Arduino Uno would be insufficient for the networking and concurrency requirements.

---

## Possible Extensions (Bonus)
- Web interface to change Pomodoro durations
- Mobile notifications
- Statistics tracking (sessions completed)
- Enclosure design (3D printed box)
- Multiple LED color themes

---

## Conclusion
The Smart Pomodoro & Privacy Box is a compact, useful, and technically appropriate embedded systems project that combines hardware, software, and user interaction. It demonstrates system thinking, non-blocking programming, and real-world applicability.
