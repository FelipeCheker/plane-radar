#include "secrets.h"
#include "config.h"
#include "helpers/Domain/Aircraft.h"
#include "helpers/ErrorsManager/Errors.h"
#include "helpers/Services/AircraftService/aircraftService.h"
#include "helpers/Display/displayManager.h"
#include "helpers/Services/AircraftService/aircraftService.cpp"
#include "helpers/Display/displayManager.cpp"
#include "helpers/Connection/WIFI/wifiManager.cpp"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

unsigned long lastApiUpdate = 0;
unsigned long lastScreenChange = 0;
int currentPlane = 0;
int planeCount = 0;

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1
);

void setup()
{
    Serial.begin(115200);

    Wire.begin(21,22);

    initDisplay(display);

    connectWifi(display);

    updateNearestPlane(display, planeCount);
}

void loop()
{
    if(millis() - lastApiUpdate > API_INTERVAL_MS)
    {
        updateNearestPlane(display, planeCount);
        lastApiUpdate = millis();
    }

    if(planeCount > 0 && millis() - lastScreenChange > SCREEN_INTERVAL_MS)
    {
        currentPlane++;
        if(currentPlane >= planeCount)
        {
            currentPlane = 0;
        }
        
        static int lastPlane = -1;
        if(lastPlane != currentPlane)
        {
            showPlaneWithScroll(nearestPlanes[currentPlane], currentRegion, planeCount, display);
            lastPlane = currentPlane;
        }
        lastScreenChange = millis();
    }
}