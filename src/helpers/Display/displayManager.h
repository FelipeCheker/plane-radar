#pragma once

#include <Adafruit_SSD1306.h>
#include "helpers/Domain/Aircraft.h"

void initDisplay(Adafruit_SSD1306 &display);
void showPlaneWithScroll(Aircraft p, String currentRegion, int planeCount, Adafruit_SSD1306 &display);