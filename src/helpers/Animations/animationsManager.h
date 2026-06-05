#pragma once

#include <Adafruit_SSD1306.h>

void showSearchingAnimation(Adafruit_SSD1306 &display, String currentRegion);
String headingArrow(double heading);
void playWaitingAnimation(Adafruit_SSD1306& display);