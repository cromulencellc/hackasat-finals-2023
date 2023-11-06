#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::geofence
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t PATH_SIZE = 64;
    static const size_t MAX_NAME_SIZE = 64;
    static const size_t ZONE_NAME_SIZE = 32;
    static const size_t TEAM_NAME_SIZE = 16;
    static const size_t MAX_POLYGON_POINTS = 10;
    // data structure
    struct OS_PACK TimeOffsets
    {
        double leapseconds;
        double deltaUT1;
    };
    struct OS_PACK LoadGeofence
    {
        char pathname[PATH_SIZE];
    };
    struct OS_PACK SnapRequest
    {
        uint8_t image_id;
    };
    struct OS_PACK GeoPoint
    {
        double lat;
        double lon;
    };
    struct OS_PACK Zone
    {
        char name[ZONE_NAME_SIZE];
        uint8_t nZones;
        GeoPoint polygon[MAX_POLYGON_POINTS];
    };
    struct OS_PACK GeofenceName
    {
        double tai;
        char name[ZONE_NAME_SIZE];
    };
    struct OS_PACK SnapMeta
    {
        double timeJd;
        float gpsPosition[3];
        float qAxis[3];
        float qScalar;
        char team[TEAM_NAME_SIZE];
    };
    struct OS_PACK Team
    {
        char name[TEAM_NAME_SIZE];
    };
}