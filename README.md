
# Access Control with Sanitization using ESP32CAM and Arduino Uno

A project I worked on to implement an access control system that also included a way to sanitize
peoples hands as they went into a facility and also check their temperature with builtin camera
survillence

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

- **Camera Operations:** The camera uses an http server to stream image frames for taking pictures when a person is detected near the  sanitization module
- **HTTP Communication:** The ESP32CAM uses HTTP to receive triggers to capture an image and also control the flash light. It sort of acts like an API for the camera

## Sanitization Module

- **Sanitizer Level Monitoring:** Continuous monitoring of sanitizer levels through `checkSanitizerLevel()` function and associated sensor readings.
- **Temperature Monitoring:** Utilizes an infrared sensor (`Adafruit_MLX90614`) to gauge body temperature and trigger sanitation protocols (in this case trigger a buzzer) based on predefined temperature limits.
- **Proximity Sensing:** Utilizes ultrasonic sensors (`initSonar()`, `getDistance()`) to detect proximity and initiate sanitation measures based on predefined distance criteria.
