#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "HeartbeatPayloads.hpp"
namespace cromulence::heartbeat
{
    // Command messages
    // Telemetry messages
    typedef cromulence::TlmMessage< empty , HEARTBEAT_PPS_TLM_ID  > PpsMsg;
}