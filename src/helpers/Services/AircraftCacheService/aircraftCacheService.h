#pragma once

#include <Arduino.h>

bool initCache();
String getModelByIcao24(String icao24);
bool saveModel(String icao24, String model);
bool getRouteByCallsign(
    String callsign,
    String& origin,
    String& destination);

bool saveRoute(
    String callsign,
    String origin,
    String destination);