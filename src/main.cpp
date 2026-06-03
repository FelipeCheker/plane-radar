#include "secrets.h"
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <math.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const double USER_LAT = -34.8553040;
const double USER_LON = -55.9908691;
unsigned long lastApiUpdate = 0;
const unsigned long API_INTERVAL = 30000;
unsigned long lastScreenChange = 0;
const unsigned long SCREEN_INTERVAL = 6000;
DynamicJsonDocument doc(65536);
String currentRegion = "UY";
String URL_URUGUAY = "https://opensky-network.org/api/states/all?lamin=-35.5&lomin=-57.5&lamax=-33.5&lomax=-54.0";
String URL_REGIONAL = "https://opensky-network.org/api/states/all?lamin=-40&lomin=-65&lamax=-25&lomax=-45";
String URL_WORLD = "https://opensky-network.org/api/states/all";
int radarFrame = 0;
unsigned long lastRadarFrame = 0;

struct Aircraft
{
    String callsign;
    double distance;
    String model;
    double altitude;
    double speed;
    double heading;
    String icao24;
};

Aircraft nearestPlanes[10];
int planeCount = 0;
int currentPlane = 0;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1
);

void showError(String message)
{
    Serial.println(message);

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0,0);

    display.println("ERROR");
    display.println();
    display.println(message);

    display.display();
}

double haversine(double lat1, double lon1, double lat2, double lon2){
    const double R = 6371.0;
    double dLat = radians(lat2 - lat1);
    double dLon = radians(lon2 - lon1);
    double a = sin(dLat/2) * sin(dLat/2) +
        cos(radians(lat1)) *
        cos(radians(lat2)) *
        sin(dLon/2) *
        sin(dLon/2);

    double c = 2 * atan2(
            sqrt(a),
            sqrt(1-a));

    return R * c;
}

String headingArrow(double heading)
{
    if (heading < 22.5) return "^";
    if (heading < 67.5) return "/";
    if (heading < 112.5) return ">";
    if (heading < 157.5) return "\\";
    if (heading < 202.5) return "v";
    if (heading < 247.5) return "/";
    if (heading < 292.5) return "<";
    if (heading < 337.5) return "\\";

    return "^";
}

void initDisplay()
{
    if(!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C))
    {
        Serial.println("OLED no encontrado");

        while(true)
        {
            delay(1000);
        }
    }

    display.clearDisplay();
    display.setTextColor(WHITE);

    display.setTextSize(2);
    display.setCursor(0,0);
    display.println("BOOT OK");

    display.display();

    delay(2000);
}

void connectWifi()
{
    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Connecting WiFi");

    display.display();

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi conectado");

    Serial.print("IP: ");
    Serial.println(
        WiFi.localIP()
    );

    display.clearDisplay();

    display.setCursor(0,0);
    display.println("WiFi OK");

    display.print("IP:");

    display.println(
        WiFi.localIP()
    );

    display.display();

    delay(2000);
}

JsonArray openSkyAPI(String url)
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
        showError("HTTP " + String(code));
    
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
    
        showError(error.c_str());
    
        http.end();
    
        return JsonArray();
    }

    return doc["states"].as<JsonArray>();
}

JsonArray getAircraftData()
    {
        JsonArray states;
    
        states = openSkyAPI(URL_URUGUAY);
    
        if(!states.isNull() && states.size() > 0)
        {
            currentRegion = "UY";
            return states;
        }
    
        states = openSkyAPI(URL_REGIONAL);
    
        if(!states.isNull() && states.size() > 0)
        {
            currentRegion = "REG";
            return states;
        }
    
        states = openSkyAPI(URL_WORLD);
    
        if(!states.isNull() && states.size() > 0)
        {
            currentRegion = "WORLD";
            return states;
        }
    
        currentRegion = "NONE";
    
        return JsonArray();
}

void showSearchingAnimation()
{
    if(millis() - lastRadarFrame < 100)
        return;

    lastRadarFrame = millis();

    display.clearDisplay();

    int cx = 64;
    int cy = 28;
    int radius = 25;

    // Radar
    display.drawCircle(cx, cy, radius, WHITE);
    display.drawCircle(cx, cy, radius / 2, WHITE);

    display.drawLine(cx - radius, cy, cx + radius, cy, WHITE);
    display.drawLine(cx, cy - radius, cx, cy + radius, WHITE);

    // Sweep
    float angle =
        radians(radarFrame * 12);

    int x =
        cx +
        cos(angle) * radius;

    int y =
        cy +
        sin(angle) * radius;

    display.drawLine(
        cx,
        cy,
        x,
        y,
        WHITE);

    // Ecos falsos
    if(radarFrame % 2 == 0)
    {
        display.fillCircle(
            cx + 10,
            cy - 15,
            2,
            WHITE);
    }

    if(radarFrame % 3 == 0)
    {
        display.fillCircle(
            cx - 18,
            cy + 8,
            2,
            WHITE);
    }

    if(radarFrame % 4 == 0)
    {
        display.fillCircle(
            cx + 20,
            cy + 12,
            2,
            WHITE);
    }

    // Región
    display.setTextSize(1);

    display.setCursor(0,0);

    display.print("SRC:");
    display.print(currentRegion);

    // Hora UTC aproximada
    unsigned long uptime =
        millis() / 1000;

    int sec =
        uptime % 60;

    int min =
        (uptime / 60) % 60;

    int hour =
        (uptime / 3600) % 24;

    display.setCursor(80,0);

    char timeBuffer[10];

    sprintf(
        timeBuffer,
        "%02d:%02d:%02d",
        hour,
        min,
        sec);

    display.print(timeBuffer);

    display.setCursor(22,55);

    display.print("SEARCHING");

    display.display();

    radarFrame++;

    if(radarFrame >= 30)
        radarFrame = 0;
}

void updateDisplay()
{
    if(planeCount == 0)
    {
        showSearchingAnimation();
        return;
    }

    Aircraft p = nearestPlanes[currentPlane];

    display.clearDisplay();
    display.setCursor(0,0);

    display.setTextColor(WHITE);

    display.setTextSize(2);
    display.println(p.callsign);

    display.setTextSize(1);
    
//    display.print("Aircraft: ");
//    loadModelIfNeeded(p);
//    display.println(p.model);

    display.print("Distance: ");
    display.print((int)p.distance);
    display.println("km");

    display.print("Altitude: ");
    display.print((int)(p.altitude * 3.28084));
    display.println("ft");

    float speedKmh = p.speed * 3.6;
    display.print("Speed: ");
    display.print((int)speedKmh);
    display.println(" km/h");

    display.print("Heading: ");
    display.print((int)p.heading);
    display.print(" ");
    display.println(headingArrow(p.heading));

    display.print("Source: ");
    display.println(currentRegion);

    display.display();
}

void showPlaneWithScroll(Aircraft p)
{
    if(planeCount == 0)
    {
        showSearchingAnimation();
        return;
    }
    
    for(int offset = 40; offset >= 0; offset -= 2)
    {
        display.clearDisplay();
        display.setTextColor(WHITE);

        int x = 0;

        // Callsign (queda arriba fijo relativo)
        display.setTextSize(2);
        display.setCursor(x, offset);
        display.println(p.callsign);

        display.setTextSize(1);

//        display.setCursor(0, offset + 12);
//        display.print("Aircraft: ");
//        loadModelIfNeeded(p);
//        display.println(p.model);

        display.setCursor(x, offset + 18);
        display.print("Distance: ");
        display.print((int)p.distance);
        display.println("km");

        display.setCursor(x, offset + 28);
        display.print("Altitude: ");
        display.print((int)(p.altitude * 3.28084));
        display.println("ft");

        display.setCursor(x, offset + 38);
        float speedKmh = p.speed * 3.6;
        display.print("Speed: ");
        display.print((int)speedKmh);
        display.println(" km/h");

        display.setCursor(x, offset + 48);
        display.print("Heading: ");
        display.print((int)p.heading);
        display.print(" ");
        display.println(headingArrow(p.heading));

        display.setCursor(x, offset + 58);
        display.print("Source: ");
        display.println(currentRegion);

        display.display();
        delay(20);
    }
}

void updateNearestPlane()
{
  planeCount = 0;
  JsonArray states = getAircraftData();

  if(states.size() == 0){
    showError("NOAIRCRAFT");
    return;
  }
  else if(states.isNull())
  {
      showError("API ERROR");
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
          //nearestPlanes[planeCount].model = "";
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

  //updateDisplay();
  showPlaneWithScroll(nearestPlanes[currentPlane]);
}

void transitionToNextPlane(int nextIndex)
{
    Aircraft from = nearestPlanes[currentPlane];
    Aircraft to = nearestPlanes[nextIndex];

    for(int i = 0; i <= 8; i++)
    {
        display.clearDisplay();

        int offsetFrom = -i * 16;   // se va a la izquierda
        int offsetTo   = (8 - i) * 16; // entra desde la derecha

        display.setTextColor(WHITE);

        // -------- AVION SALIENDO --------
        display.setCursor(offsetFrom, 0);
        display.setTextSize(2);
        display.println(from.callsign);

        display.setTextSize(1);
        display.setCursor(offsetFrom, 20);
        display.print("D:");
        display.print((int)from.distance);
        display.print("km");

        display.setCursor(offsetFrom, 32);
        display.print("SPD:");
        display.print((int)(from.speed * 3.6));

        // -------- AVION ENTRANDO --------
        display.setCursor(offsetTo, 0);
        display.setTextSize(2);
        display.println(to.callsign);

        display.setTextSize(1);
        display.setCursor(offsetTo, 20);
        display.print("D:");
        display.print((int)to.distance);
        display.print("km");

        display.setCursor(offsetTo, 32);
        display.print("SPD:");
        display.print((int)(to.speed * 3.6));

        display.display();
        delay(25);
    }

    currentPlane = nextIndex;
}

void setup()
{
    Serial.begin(115200);

    Wire.begin(21,22);

    initDisplay();

    connectWifi();

    updateNearestPlane();
}

void loop()
{
    if(millis() - lastApiUpdate > API_INTERVAL)
    {
        updateNearestPlane();

        lastApiUpdate = millis();
    }

    if(
        planeCount > 0 && millis() - lastScreenChange > SCREEN_INTERVAL)
    {
        int next = currentPlane + 1;
        if(next >= planeCount)
          next = 0;
        
        transitionToNextPlane(next);
        //updateDisplay();
        static int lastPlane = -1;
        if(lastPlane != currentPlane)
        {
            showPlaneWithScroll(nearestPlanes[currentPlane]);
            lastPlane = currentPlane;
        }
        lastScreenChange = millis();
    }
}