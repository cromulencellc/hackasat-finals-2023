#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "CfeEvsPayloads.hpp"
namespace gsfc::cfe_evs
{
    // Command messages
    // Telemetry messages
    typedef cromulence::TlmMessage< Event , CFEEVS_EVENT_TLM_ID  > EventMsg;
}