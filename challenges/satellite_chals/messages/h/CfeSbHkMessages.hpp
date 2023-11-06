#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "CfeSbHkPayloads.hpp"
namespace gsfc::cfe_sb_hk
{
    // Command messages
    typedef cromulence::CmdMessage< empty , CFESBHK_CMD_ID ,0 > CfeSbSendHkMsg;
    // Telemetry messages
}