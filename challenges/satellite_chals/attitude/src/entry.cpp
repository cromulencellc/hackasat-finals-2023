#include "Attitude.hpp"



extern "C" void AttitudeMain(void);


void AttitudeMain(void) {
   //
   cromulence::attitude::AttitudeApp* obj = new cromulence::attitude::AttitudeApp();
   // start the main loop
   obj->execute();
   delete obj;

}