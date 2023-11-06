#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "CfeTimeHkPayloads.hpp"
namespace gsfc::cfe_time_hk
{
    // Command messages
    typedef cromulence::CmdMessage< empty , CFETIMEHK_CMD_ID ,0 > CfeTimeSendHkMsg;
    // Telemetry messages
}