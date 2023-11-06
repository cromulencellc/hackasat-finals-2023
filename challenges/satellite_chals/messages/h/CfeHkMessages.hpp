#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "CfeHkPayloads.hpp"
namespace gsfc::cfe_hk
{
    // Command messages
    typedef cromulence::CmdMessage< empty , CFEHK_CMD_ID ,0 > CfeSendHkMsg;
    // Telemetry messages
}