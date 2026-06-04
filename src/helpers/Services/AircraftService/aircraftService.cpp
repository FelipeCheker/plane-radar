#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "secrets.h"
#include "helpers/ErrorsManager/Errors.h"
#include "helpers/Domain/Aircraft.h"
#include "helpers/Domain/Region.h"
#include "helpers/Location/LatLongCalculation/locationManager.h"
#include <Arduino.h>
#include <config.h>

DynamicJsonDocument doc(65536);
Aircraft nearestPlanes[10];
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

void updateNearestPlane(Adafruit_SSD1306 &display, int planeCount)
{
  planeCount = 0;
  JsonArray states = getAircraftData(display);

  if(states.size() == 0){
    showError("NOAIRCRAFT", display);
    return;
  }
  else if(states.isNull())
  {
      showError("API ERROR", display);
      return;
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

      if(planeCount < 10 && distance < 300)
      {
          nearestPlanes[planeCount].callsign = plane[1].isNull() ? "UNKNOWN" : plane[1].as<String>();
          nearestPlanes[planeCount].callsign.trim();
          nearestPlanes[planeCount].distance = distance;
          nearestPlanes[planeCount].altitude = plane[7].isNull() ? 0 : plane[7].as<double>();
          nearestPlanes[planeCount].speed = plane[9].isNull() ? 0 : plane[9].as<double>();
          nearestPlanes[planeCount].heading = plane[10].isNull() ? 0 : plane[10].as<double>();
          nearestPlanes[planeCount].icao24 = plane[0].isNull() ? "" : plane[0].as<String>();
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
}