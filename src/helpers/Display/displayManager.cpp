#include <Adafruit_SSD1306.h>
#include "helpers/Domain/Aircraft.h"
#include "helpers/Animations/animationsManager.h"

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
        showSearchingAnimation(display, currentRegion);
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