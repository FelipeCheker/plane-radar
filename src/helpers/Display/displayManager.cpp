#include <Adafruit_SSD1306.h>
#include "../Domain/Aircraft.h"
#include "../Animations/animationsManager.h"
#include "../Services/AircraftService/aircraftService.h"

enum ScreenMode
{
    AIRCRAFT_LIST,
    AIRCRAFT_DETAIL,
    SEARCHING
};

const int HEADER_Y = 0;
const int FIRST_ROW_Y = 18;
const int ROW_HEIGHT = 16;
ScreenMode currentScreen = AIRCRAFT_LIST;

void initDisplay(Adafruit_SSD1306 &display)
{
    if(!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C))
    {
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

void showPlaneWithScroll(Aircraft p, String currentRegion, int planeCount, Adafruit_SSD1306 &display)
{
    if(planeCount == 0)
    {
        playWaitingAnimation(display);
        return;
    }
    
    for(int offset = 40; offset >= 0; offset -= 2)
    {
        display.clearDisplay();
        display.setTextColor(WHITE);

        int x = 0;

        display.setTextSize(2);
        display.setCursor(x, offset);
        display.println(p.callsign);

        display.setTextSize(1);

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

void showAircraftList(Aircraft planes[], int planeCount, int selected, Adafruit_SSD1306& display)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0,0);
    display.print("RADAR ");
    display.println(currentRegion);

    const int visibleRows = 3;
    const int middleRow = 1;

    int start = selected - middleRow;

    if(start < 0)
        start = 0;

    if(start > planeCount - visibleRows)
        start = max(0, planeCount - visibleRows);

    display.setTextSize(2);

    for(int row = 0; row < visibleRows; row++)
    {
        int index = start + row;

        if(index >= planeCount)
            break;

        int y = 12 + row * 18;

        display.setCursor(0, y);

        if(row == middleRow &&
           selected > 0 &&
           selected < planeCount - 1)
        {
            display.print(">");
        }
        else if(index == selected)
        {
            display.print(">");
        }
        else
        {
            display.print(" ");
        }

        display.print(planes[index].callsign);
    }

    display.display();
}

void showAircraftDetail(Aircraft p, Adafruit_SSD1306& display, int planeCount)
{
    if(planeCount == 0)
    {
        playWaitingAnimation(display);
        return;
    }

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0,0);
    display.println(p.callsign);

    display.setTextSize(1);

    display.print("Distance: ");
    display.print((int)p.distance);
    display.println("km");

    display.print("Altitude : ");
    display.print((int)(p.altitude * 3.28084));
    display.println("ft");

    display.print("Speed : ");
    display.print((int)(p.speed * 3.6));
    display.println("km/h");

    display.print("Heading : ");
    display.print((int)p.heading);
    display.print(" ");
    display.println(
        headingArrow(p.heading));

    display.print("Source: ");
    display.println(currentRegion);

    display.display();
}

void showPlaneDetail(Aircraft p, Adafruit_SSD1306& display, int planeCount)
{
    if(planeCount == 0)
    {
        playWaitingAnimation(display);
        return;
    }

    String info =
        "ALT:" + String((int)(p.altitude * 3.28084)) +
        "FT  SPD:" + String((int)(p.speed * 1.944)) +
        "KNT  HDG:" + String((int)p.heading) +
        "  D:" + String((int)p.distance) +
        "KM  SRC:" + currentRegion + "   ";

    display.setTextWrap(false);

    int16_t x1, y1;
    uint16_t w, h;

    display.setTextSize(2);
    display.getTextBounds(
        info,
        0,
        0,
        &x1,
        &y1,
        &w,
        &h
    );

    for(int x = SCREEN_WIDTH; x > -((int)w); x -= 2)
    {
        display.clearDisplay();

        // Callsign fijo
        display.setTextSize(2);
        display.setCursor(0,0);
        display.print(p.callsign);

        // Línea separadora
        display.drawLine(
            0,
            18,
            SCREEN_WIDTH,
            18,
            WHITE);

        // Datos deslizándose
        display.setCursor(x,30);
        display.print(info);

        display.display();

        delay(25);
    }
}

void animateAircraftList(Aircraft planes[], int planeCount, int oldSelected, int newSelected, Adafruit_SSD1306& display)
{
    const int FIRST_ROW_Y = 18;
    const int ROW_HEIGHT = 16;

    for(int offset = 0; offset <= ROW_HEIGHT; offset += 2)
    {
        display.clearDisplay();

        display.setTextSize(2);
        display.setCursor(0,0);

        display.print("RADAR ");
        display.print(currentRegion);

        display.setCursor(0, FIRST_ROW_Y);
        display.print(">");

        for(int i = 0; i < 4; i++)
        {
            int index = oldSelected + i;

            if(index >= planeCount)
                break;

            int y =
                FIRST_ROW_Y +
                i * ROW_HEIGHT -
                offset;

            display.setCursor(16, y);
            display.print(planes[index].callsign);
        }

        display.display();
        delay(20);
    }
}