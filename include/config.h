#pragma once

#include "helpers/Domain/Region.h"
// ====================
// USER LOCATION
// ====================

constexpr double USER_LAT = -34.8553040;
constexpr double USER_LON = -55.9908691;

// ====================
// API REFRESH
// ====================

constexpr unsigned long API_INTERVAL_MS = 300000;
constexpr unsigned long SCREEN_INTERVAL_MS = 6000;

// ====================
// SEARCH REGIONS
// ====================

constexpr const char* DEFAULT_REGION = "UY";

constexpr Region REGIONS[] =
{
    {
        "UY",
        "https://opensky-network.org/api/states/all?lamin=-35.5&lomin=-57.5&lamax=-33.5&lomax=-54.0"
    },
    {
        "REG",
        "https://opensky-network.org/api/states/all?lamin=-40&lomin=-65&lamax=-25&lomax=-45"
    }
    // {
    //     "WORLD",
    //     "https://opensky-network.org/api/states/all"
    // }
};

// ====================
// AIRCRAFT FILTERS
// ====================

constexpr int MAX_AIRCRAFT = 10;
constexpr double MAX_DISTANCE_KM = 500.0;

// ====================
// OLED
// ====================

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int OLED_ADDRESS = 0x3C;

// ====================
// I2C
// ====================

constexpr int SDA_PIN = 21;
constexpr int SCL_PIN = 22;