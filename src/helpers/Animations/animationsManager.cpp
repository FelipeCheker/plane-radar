#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "helpers/Animations/Frames/frame01.h"
#include "helpers/Animations/Frames/frame02.h"
#include "helpers/Animations/Frames/frame03.h"
#include "helpers/Animations/Frames/frame04.h"
#include "helpers/Animations/Frames/frame05.h"
#include "helpers/Animations/Frames/frame06.h"
#include "helpers/Animations/Frames/frame07.h"
#include "helpers/Animations/Frames/frame08.h"
#include "helpers/Animations/Frames/frame09.h"
#include "helpers/Animations/Frames/frame10.h"
#include "helpers/Animations/Frames/frame11.h"
#include "helpers/Animations/Frames/frame12.h"


int radarFrame = 0;
unsigned long lastRadarFrame = 0;

enum SearchAnimationState
{
    RADAR_SEARCH,
    F18_FAR,
    F18_APPROACH_1,
    F18_APPROACH_2,
    F18_APPROACH_3,
    F18_COCKPIT,
    F18_RADAR
};

void playWaitingAnimation(Adafruit_SSD1306& display)
{
    static int frame = 0;
    static unsigned long lastFrame = 0;

    if(millis() - lastFrame < 120)
        return;

    lastFrame = millis();

    display.clearDisplay();

    switch(frame)
    {
        case 0:
            display.drawBitmap(0,0,frame01,128,64,WHITE);
            break;

        case 1:
            display.drawBitmap(0,0,frame02,128,64,WHITE);
            break;

        case 2:
            display.drawBitmap(0,0,frame03,128,64,WHITE);
            break;

        case 3:
            display.drawBitmap(0,0,frame04,128,64,WHITE);
            break;

        case 4:
            display.drawBitmap(0,0,frame05,128,64,WHITE);
            break;

        case 5:
            display.drawBitmap(0,0,frame06,128,64,WHITE);
            break;

        case 6:
            display.drawBitmap(0,0,frame07,128,64,WHITE);
            break;

        case 7:
            display.drawBitmap(0,0,frame08,128,64,WHITE);
            break;

        case 8:
            display.drawBitmap(0,0,frame09,128,64,WHITE);
            break;

        case 9:
            display.drawBitmap(0,0,frame10,128,64,WHITE);
            break;

        case 10:
            display.drawBitmap(0,0,frame11,128,64,WHITE);
            break;

        case 11:
            display.drawBitmap(0,0,frame12,128,64,WHITE);
            break;
    }

    display.display();

    frame++;

    if(frame >= 12)
        frame = 0;
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

