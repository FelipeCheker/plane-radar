#pragma once

#include <Adafruit_SSD1306.h>
#include "helpers/Domain/Aircraft.h"

enum ScreenMode
{
    AIRCRAFT_LIST,
    AIRCRAFT_DETAIL
};

extern ScreenMode currentScreen;

void initDisplay(Adafruit_SSD1306 &display);
void showPlaneWithScroll(Aircraft p, String currentRegion, int planeCount, Adafruit_SSD1306 &display);
void showAircraftList(Aircraft planes[], int planeCount, int selected, Adafruit_SSD1306& display);
void showPlaneDetail(Aircraft p, Adafruit_SSD1306& display, int planeCount);
void animateAircraftList(Aircraft planes[], int planeCount, int oldSelected, int newSelected, Adafruit_SSD1306& display);
// void animateListToDetail(Aircraft planes[],int planeCount,int selected,Adafruit_SSD1306& display);
// void animateDetailToList(Aircraft planes[],int planeCount,int selected,Adafruit_SSD1306& display);
