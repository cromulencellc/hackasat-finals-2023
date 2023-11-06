#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "ScriptPayloads.hpp"
namespace cromulence::script
{
    // Command messages
    typedef cromulence::CmdMessage< empty , SCRIPT_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , SCRIPT_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< Script , SCRIPT_CMD_ID ,2 > RunMsg;
    typedef cromulence::CmdMessage< empty , SCRIPT_CMD_ID ,3 > KillMsg;
    typedef cromulence::CmdMessage< Script , SCRIPT_CMD_ID ,4 > LoadMsg;
    typedef cromulence::CmdMessage< User , SCRIPT_CMD_ID ,5 > SetUserMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< CurrentScript , SCRIPT_STATUS_TLM_ID  > StatusMsg;
}