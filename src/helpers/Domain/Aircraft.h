#pragma once

#include <Arduino.h>

struct Aircraft
{
    String callsign;
    double distance;
    String model;
    double altitude;
    double speed;
    double heading;
    String icao24;
    String origin;
    String destination;
};