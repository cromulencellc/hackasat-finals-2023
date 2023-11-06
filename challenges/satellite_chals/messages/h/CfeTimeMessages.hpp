#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "CfeTimePayloads.hpp"
namespace gsfc::cfe_time
{
    // Command messages
    typedef cromulence::CmdMessage< Time , CFETIME_CMD_ID ,7 > SetTimeMsg;
    // Telemetry messages
}