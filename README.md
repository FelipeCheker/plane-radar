# ESP32 Aircraft Radar Display

A real-time aircraft tracking system built with an ESP32 and a 0.96" OLED display. The device connects to Wi-Fi, retrieves live aircraft data from the OpenSky Network API, and displays nearby aircraft flying around the user's location.

## Features
- Real-time aircraft tracking using the OpenSky Network API
- Wi-Fi connectivity through ESP32
- OLED display interface (128x64 I2C)
- Automatic detection of nearby aircraft

**Display**:
- Callsign (Aircraft Id like AAL907)
- Distance from user (Calculated by the lat and long provided)
- Altitude (feet)
- Speed (km/h)
- Source (Set the region its getting from the aircrafts)
- Heading and direction (As a compass)

**Animations**
- Multiple aircraft browsing with animated transitions
- Radar-style search animation when no aircraft are detected

**Logic to search near aircrafts**
The system search for aircraft near the latitude and longitud provided and if there is no aircraf, it does a fallback and a request with other region setted, in this case is South America and in case there is none there it searchs world wide.

### Automatic fallback between:
- Local airspace (Uruguay - Lat and Long setted in this case)
- Regional airspace (South America)
- Worldwide search
- Smart API usage management to reduce OpenSky credit consumption

**Error handling for:**
- Wi-Fi connection failures
- API errors
- Empty responses
- JSON parsing issues
- Hardware
- ESP32 Development Board
- 0.96" OLED Display (SSD1306, I2C)
- USB power supply or wall adapter
- Software Stack
- PlatformIO
- Arduino Framework
- ArduinoJson
- Adafruit SSD1306
- Adafruit GFX
- ESP32 WiFi Library

## How It Works

The ESP32 periodically queries the OpenSky Network API for aircraft positions. It calculates the distance between each aircraft and the configured user coordinates using the Haversine formula. The closest aircraft are sorted and displayed on the OLED screen.

When no nearby aircraft are found, the system automatically expands its search area and shows an animated radar screen while waiting for traffic to appear.

<img width="369" height="327" alt="image" src="https://github.com/user-attachments/assets/f35dda62-06dd-4fc5-bcac-d16d10fb952a" />



**Future Improvements**
- Aircraft model lookup (B737, A320, B777, etc.)
- Origin and destination airports
- ADS-B receiver integration
- Weather information
- Larger TFT display support
- Touchscreen interface
- Aircraft map visualization
- Bluetooth configuration mode
- Battery-powered portable version

### Use Cases
- Aviation enthusiasts
- Flight spotting
- Airport monitoring
- Educational IoT projects
- ESP32 and API integration learning
- Real-time embedded systems demonstrations

Built with ESP32, OpenSky Network, and a passion for aviation. ✈️
