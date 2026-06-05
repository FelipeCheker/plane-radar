#include <Arduino.h>
#include <math.h>

double haversine(double lat1, double lon1, double lat2, double lon2){
    const double R = 6371.0;
    double dLat = radians(lat2 - lat1);
    double dLon = radians(lon2 - lon1);
    double a = sin(dLat/2) * sin(dLat/2) +
        cos(radians(lat1)) *
        cos(radians(lat2)) *
        sin(dLon/2) *
        sin(dLon/2);

    double c = 2 * atan2(
            sqrt(a),
            sqrt(1-a));

    return R * c;
}