#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "GpsPayloads.hpp"
namespace cromulence::gps
{
    // Command messages
    typedef cromulence::CmdMessage< empty , GPS_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , GPS_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< Config , GPS_CMD_ID ,2 > ConfigMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< Position , GPS_ECEF_TLM_ID  > EcefMsg;
    typedef cromulence::TlmMessage< LLH , GPS_GEODETIC_TLM_ID  > GeodeticMsg;
}