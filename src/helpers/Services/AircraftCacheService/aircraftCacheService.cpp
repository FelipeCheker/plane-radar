#include "aircraftCacheService.h"

#include <LittleFS.h>
#include <ArduinoJson.h>

const char* CACHE_FILE = "/aircraft.json";
const char* ROUTES_FILE = "/routes.json";

bool initCache()
{
    return LittleFS.begin();
}

String getModelByIcao24(String icao24)
{
    File file = LittleFS.open(CACHE_FILE, "r");

    if(!file)
        return "";

    DynamicJsonDocument doc(32768);

    deserializeJson(doc,file);

    file.close();

    if(doc.containsKey(icao24))
    {
        return doc[icao24].as<String>();
    }

    return "";
}

bool saveModel(String icao24, String model)
{
    DynamicJsonDocument doc(32768);

    File file = LittleFS.open(CACHE_FILE, "r");

    if(file)
    {
        deserializeJson(doc,file);

        file.close();
    }

    doc[icao24] = model;

    file = LittleFS.open(CACHE_FILE, "w");

    if(!file)
        return false;

    serializeJson(doc,file);

    file.close();

    return true;
}

bool getRouteByCallsign(
    String callsign,
    String& origin,
    String& destination)
{
    File file =
        LittleFS.open(
            ROUTES_FILE,
            "r");

    if(!file)
        return false;

    DynamicJsonDocument doc(32768);

    deserializeJson(doc,file);

    file.close();

    if(!doc.containsKey(callsign))
        return false;

    origin =
        doc[callsign]["origin"]
            .as<String>();

    destination =
        doc[callsign]["destination"]
            .as<String>();

    return true;
}

bool saveRoute(
    String callsign,
    String origin,
    String destination)
{
    DynamicJsonDocument doc(32768);

    File file =
        LittleFS.open(
            ROUTES_FILE,
            "r");

    if(file)
    {
        deserializeJson(doc,file);
        file.close();
    }

    doc[callsign]["origin"] =
        origin;

    doc[callsign]["destination"] =
        destination;

    file =
        LittleFS.open(
            ROUTES_FILE,
            "w");

    if(!file)
        return false;

    serializeJson(doc,file);

    file.close();

    return true;
}