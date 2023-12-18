
# Access Control with Sanitization using ESP32CAM and Arduino Uno

This academic project integrates an access control system with a sanitization module, designed during the COVID-19 period. The project consists of several interconnected modules, each serving distinct functionalities:

## Access Control Module

### RFID and Biometric Access
The Access Control module handles secure entry using RFID cards and fingerprint sensors. The code segments within this module facilitate:

- **RFID Authentication:** The system verifies access using specific RFID card tags (VISITOR_TAG_1 and VISITOR_TAG_2) assigned to visitors.
- **Biometric Access:** Fingerprint-based authentication is implemented using functions such as `fpsAccessMode()` and `enrollFingerprints()`.

### User Interface and Control
Additionally, this module manages the user interface and control mechanisms, comprising:

- **Servo Control:** Functions like `initServo()`, `openMode()`, and `closeMode()` control servo motors for managing the entry door.
- **LCD Interface:** LCD functions (`initLCD()`, `lcdPrintString()`, etc.) display access-related messages and prompts for users.

## Camera Module

### Camera and HTTP Handling
The Camera module encompasses functionalities related to camera integration and HTTP request management. Although specific details are limited, the code segments suggest:

- **Camera Operations:** Code snippets like `app_httpd.cpp`, `Camera.ino`, and `camera_pins.h` imply camera-related operations and setup.
- **HTTP Communication:** This module potentially handles HTTP requests, possibly for image capture or transmission, enhancing security measures.

## Sanitization Module

### Hygiene Maintenance
The Sanitization Module focuses on maintaining hygiene within the controlled environment. The code segments indicate the inclusion of:

- **Sanitizer Level Monitoring:** Continuous monitoring of sanitizer levels through `checkSanitizerLevel()` function and associated sensor readings.
- **Temperature Monitoring:** Utilizes an infrared sensor (`Adafruit_MLX90614`) to gauge body temperature and trigger sanitation protocols based on predefined temperature limits.
- **Proximity Sensing:** Utilizes ultrasonic sensors (`initSonar()`, `getDistance()`) to detect proximity and initiate sanitation measures based on predefined distance criteria.
