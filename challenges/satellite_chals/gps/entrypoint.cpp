#include "Gps.hpp"
#include <thread>
#include <chrono>
extern "C" void GpsMain(void);

using namespace std::chrono_literals;
void GpsMain(void) {
    

    auto app_ = new cromulence::gps::GpsApp();
    app_->execute();
    delete app_;
}
