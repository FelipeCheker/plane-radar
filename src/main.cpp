#include "secrets.h"
#include "config.h"
#include "helpers/Domain/Aircraft.h"
#include "helpers/ErrorsManager/Errors.h"
#include "helpers/Services/AircraftService/aircraftService.h"
#include "helpers/Display/displayManager.h"
#include "helpers/Services/AircraftService/aircraftService.h"
#include "helpers/Display/displayManager.h"
#include "helpers/Connection/WIFI/wifiManager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "helpers/Services/AircraftCacheService/aircraftCacheService.h"

unsigned long lastApiUpdate = 0;
unsigned long lastScreenChange = 0;
int currentPlane = 0;
int planeCount = 0;
Aircraft planes[MAX_AIRCRAFT];
unsigned long screenTimer = 0;

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
    initCache();
    AircraftResult result = updateNearestPlane(display, planeCount);
    planeCount = result.planeCount;

    for (int i = 0; i < planeCount && i < MAX_AIRCRAFT; ++i) {
        planes[i] = result.planes[i];
    }
}

void loop()
{
    if(millis() - lastApiUpdate > API_INTERVAL_MS)
    {
        AircraftResult result = updateNearestPlane(display, planeCount);
        planeCount = result.planeCount;
        for (int i = 0; i < planeCount && i < MAX_AIRCRAFT; ++i) {
            planes[i] = result.planes[i];
        }
        lastApiUpdate = millis();
    }

    if(currentScreen == AIRCRAFT_LIST)
    {
        int oldPlane = currentPlane;

        static int previousPlane = 0;

        if(currentPlane != previousPlane)
        {
            animateAircraftList(planes,
                planeCount,
                previousPlane,
                currentPlane,
                display);

            previousPlane = currentPlane;
        }
        else
        {
            showAircraftList(planes, planeCount, currentPlane, display);
        }

        if(millis() - screenTimer > 3000)
        {
            currentScreen = AIRCRAFT_DETAIL;
            screenTimer = millis();
        }
    }
    else
    {
        showPlaneDetail(planes[currentPlane], display, planeCount);
        
        if(millis() - screenTimer > 5000)
        {
            currentPlane++;

            if(currentPlane >= planeCount)
                currentPlane = 0;

            currentScreen = AIRCRAFT_LIST;

            screenTimer = millis();
        }
    }
}