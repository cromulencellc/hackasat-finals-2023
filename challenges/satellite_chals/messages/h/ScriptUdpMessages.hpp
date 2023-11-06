#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "ScriptUdpPayloads.hpp"
namespace cromulence::script_udp
{
    // Command messages
    typedef cromulence::CmdMessage< empty , SCRIPTUDP_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , SCRIPTUDP_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< Configure , SCRIPTUDP_CMD_ID ,2 > ConfigureMsg;
    typedef cromulence::CmdMessage< Team , SCRIPTUDP_CMD_ID ,3 > TeamMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< HouseKeeping , SCRIPTUDP_HOUSEKEEPING_TLM_ID  > HouseKeepingMsg;
    typedef cromulence::TlmMessage< TxtMessage , SCRIPTUDP_KIDDIE_TLM_ID  > KiddieMsg;
}