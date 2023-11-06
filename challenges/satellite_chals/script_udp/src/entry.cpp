#include "ScriptUdp.hpp"
#include <thread>
#include <chrono>
extern "C" void ScriptUdpMain(void);

using namespace std::chrono_literals;
void ScriptUdpMain(void) {
    cromulence::script_udp::ScriptUdpApp *app_ = new cromulence::script_udp::ScriptUdpApp();
    app_->execute();
    printf("Exiting LUA UDP App\n");
    delete app_;
}
