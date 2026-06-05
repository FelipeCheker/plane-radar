#include "Errors.h"

void showError(
    String message,
    Adafruit_SSD1306 &display)
{
    Serial.println(message);

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0,0);

    display.println("ERROR");
    display.println(message);

    display.display();
}