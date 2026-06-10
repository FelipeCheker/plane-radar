#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "secrets.h"
#include "helpers/ErrorsManager/Errors.h"
#include "helpers/Domain/Aircraft.h"
#include "helpers/Domain/Region.h"
#include "helpers/Location/LatLongCalculation/locationManager.h"
#include "helpers/Results/AircraftResult.h"
#include "helpers/Services/AircraftCacheService/aircraftCacheService.h"
#include <Arduino.h>
#include <config.h>

DynamicJsonDocument doc(65536);
Aircraft nearestPlanes[MAX_AIRCRAFT];
String currentRegion = "UY";

JsonArray openSkyAPI(String url, Adafruit_SSD1306 &display)
{
    HTTPClient http;

    http.begin(url);

    http.setAuthorization(
        OPENSKY_USER,
        OPENSKY_PASSWORD
    );

    int code = http.GET();

    Serial.print("HTTP CODE: ");
    Serial.println(code);

    if(code != 200)
    {
        showError("HTTP " + String(code), display);
    
        http.end();
    
        return JsonArray();
    }
    
    String payload = http.getString();
    Serial.print("Payload size: ");
    Serial.println(payload.length());
    Serial.println(payload.substring(0,500));
    DeserializationError error = deserializeJson(doc,payload);

    http.end();

    if(error)
    {
        Serial.print("JSON Error: ");
        Serial.println(error.c_str());
    
        showError(error.c_str(), display);
    
        http.end();
    
        return JsonArray();
    }

    return doc["states"].as<JsonArray>();
}

JsonArray getAircraftData(Adafruit_SSD1306 &display)
{
    for (const auto& region : REGIONS)
    {
        JsonArray states = openSkyAPI(region.url, display);

        if (!states.isNull() && states.size() > 0)
        {
            currentRegion = region.name;
            return states;
        }
    }

    return JsonArray();
}

String getAircraftModelFromApi(String icao24)
{
    HTTPClient http;

    String url =
        "https://api.adsbdb.com/v0/aircraft/" +
        icao24;

    Serial.println(url);

    http.begin(url);

    int code = http.GET();

    Serial.print("Metadata HTTP Code: ");
    Serial.println(code);

    if(code != 200)
    {
        http.end();
        return "";
    }

    String payload = http.getString();

    http.end();

    DynamicJsonDocument doc(4096);

    DeserializationError err =
        deserializeJson(doc, payload);

    if(err)
    {
        Serial.println(err.c_str());
        return "";
    }

    JsonObject aircraft =
        doc["response"]["aircraft"];

    if(aircraft.isNull())
    {
        return "";
    }

    String manufacturer =
        aircraft["manufacturer"] |
        "";

    String type =
        aircraft["type"] |
        "";

    if(manufacturer == "" &&
       type == "")
    {
        return "";
    }

    return manufacturer + " " + type;
}

void resolveAircraftMetadata(Aircraft& aircraft)
{
    if(aircraft.icao24 == "")
        return;

    HTTPClient http;

    String url =
    "https://api.adsbdb.com/v0/aircraft/" +
    aircraft.icao24 +
    "?callsign=" +
    aircraft.callsign;

    Serial.println(url);

    http.begin(url);

    int code = http.GET();

    Serial.print("Metadata HTTP Code: ");
    Serial.println(code);

    if(code != 200)
    {
        aircraft.model = "UNKNOWN AIRCRAFT";
        aircraft.origin = "";
        aircraft.destination = "";

        http.end();

        return;
    }

    String payload =
        http.getString();

    http.end();

    Serial.println(payload);

    DynamicJsonDocument doc(8192);

    DeserializationError error =
        deserializeJson(doc, payload);

    if(error)
    {
        Serial.println(error.c_str());
        return;
    }

    JsonObject response =
        doc["response"];

    if(response.isNull())
        return;

    // Modelo
    if(!response["aircraft"].isNull())
    {
        aircraft.model =
            response["aircraft"]["type"]
                .as<String>();
    }

    // Ruta
    if(!response["flightroute"].isNull())
    {
        aircraft.origin =
            response["flightroute"]
                   ["origin"]
                   ["iata_code"]
                   .as<String>();

        aircraft.destination =
            response["flightroute"]
                   ["destination"]
                   ["iata_code"]
                   .as<String>();
    }

    Serial.println(
        "MODEL: " +
        aircraft.model);

    Serial.println(
        "ROUTE: " +
        aircraft.origin +
        " > " +
        aircraft.destination);
}

AircraftResult updateNearestPlane(Adafruit_SSD1306 &display, int &planeCount)
{
    planeCount = 0;
    JsonArray states = getAircraftData(display);

    if(states.isNull())
    {
        showError("API ERROR", display);
        planeCount = 0;
        return AircraftResult();
    }

    if(states.size() == 0)
    {
        showError("NOAIRCRAFT", display);
        planeCount = 0;
        return AircraftResult();
    }

    for(JsonArray plane : states)
    {
        if(plane[5].isNull()) continue;
        if(plane[6].isNull()) continue;

        double lon = plane[5];
        double lat = plane[6];

        double distance =
            haversine(
                USER_LAT,
                USER_LON,
                lat,
                lon);

        if(planeCount < MAX_AIRCRAFT && distance < MAX_DISTANCE_KM)
        {
            nearestPlanes[planeCount].callsign = plane[1].isNull() ? "UNKNOWN" : plane[1].as<String>();
            nearestPlanes[planeCount].callsign.trim();
            nearestPlanes[planeCount].distance = distance;
            nearestPlanes[planeCount].altitude = plane[7].isNull() ? 0 : plane[7].as<double>();
            nearestPlanes[planeCount].speed = plane[9].isNull() ? 0 : plane[9].as<double>();
            nearestPlanes[planeCount].heading = plane[10].isNull() ? 0 : plane[10].as<double>();
            nearestPlanes[planeCount].icao24 = plane[0].isNull() ? "" : plane[0].as<String>();
            nearestPlanes[planeCount].model = "";
            nearestPlanes[planeCount].model = "";
            nearestPlanes[planeCount].origin = "";
            nearestPlanes[planeCount].destination = "";
            resolveAircraftMetadata(nearestPlanes[planeCount]);  
            planeCount++;
        }
    }

    for(int i=0;i<planeCount-1;i++)
    {
        for(int j=i+1;j<planeCount;j++)
        {
            if(nearestPlanes[j].distance <
                nearestPlanes[i].distance)
            {
                Aircraft tmp = nearestPlanes[i];
    
                nearestPlanes[i] = nearestPlanes[j];
    
                nearestPlanes[j] = tmp;
            }
        }
    }

    AircraftResult result;
    for(int i = 0; i < planeCount; i++)
    {
        result.planes[i] = nearestPlanes[i];
    }
    result.planeCount = planeCount;
    result.region = currentRegion;

    return result;
}