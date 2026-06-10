#pragma once
#include <ArduinoJson.h>
#include "helpers/Results/AircraftResult.h"

extern Aircraft nearestPlanes[10];
extern String currentRegion;

JsonArray openSkyAPI(String url, Adafruit_SSD1306 &display);
JsonArray getAircraftData(Adafruit_SSD1306 &display);
AircraftResult updateNearestPlane(Adafruit_SSD1306 &display, int &planeCount);
String getAircraftModel(String icao24);
String getAircraftModelFromApi(String icao24);
void resolveAircraftMetadata(Aircraft& aircraft);