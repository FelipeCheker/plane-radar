#include <Arduino.h>
#include <Adafruit_SSD1306.h>

int radarFrame = 0;
unsigned long lastRadarFrame = 0;

void showSearchingAnimation(Adafruit_SSD1306 &display, String currentRegion)
{
    if(millis() - lastRadarFrame < 100)
        return;

    lastRadarFrame = millis();

    display.clearDisplay();

    int cx = 64;
    int cy = 28;
    int radius = 25;

    display.drawCircle(cx, cy, radius, WHITE);
    display.drawCircle(cx, cy, radius / 2, WHITE);

    display.drawLine(cx - radius, cy, cx + radius, cy, WHITE);
    display.drawLine(cx, cy - radius, cx, cy + radius, WHITE);

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

    display.setTextSize(1);

    display.setCursor(0,0);

    display.print("SRC:");
    display.print(currentRegion);

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

