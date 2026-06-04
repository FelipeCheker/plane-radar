#pragma once
#include <ArduinoJson.h>

JsonArray openSkyAPI(String url, Adafruit_SSD1306 &display);
JsonArray getAircraftData(Adafruit_SSD1306 &display);
void updateNearestPlane(Adafruit_SSD1306 &display, int planeCount);
