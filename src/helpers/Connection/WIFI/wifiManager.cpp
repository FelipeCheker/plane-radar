#include <WiFi.h>
#include "secrets.h"
#include <Adafruit_SSD1306.h>

void connectWifi(Adafruit_SSD1306 &display)
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