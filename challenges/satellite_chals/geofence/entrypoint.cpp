#include "GeofenceApp.hpp"

extern "C" void GeofenceApp_Main(void);


void GeofenceApp_Main(void) {

    cromulence::geofence::GeofenceApp* app = new cromulence::geofence::GeofenceApp();
    app->execute();
    delete app;
}
