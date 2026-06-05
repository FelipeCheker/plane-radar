#pragma once

#include "helpers/Domain/Aircraft.h"
#include "config.h"

struct AircraftResult
{
    Aircraft planes[MAX_AIRCRAFT];
    int planeCount;
    String region;
};