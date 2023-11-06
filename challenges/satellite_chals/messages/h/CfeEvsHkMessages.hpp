#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "CfeEvsHkPayloads.hpp"
namespace gsfc::cfe_evs_hk
{
    // Command messages
    typedef cromulence::CmdMessage< empty , CFEEVSHK_CMD_ID ,0 > CfeEvsSendHkMsg;
    // Telemetry messages
}