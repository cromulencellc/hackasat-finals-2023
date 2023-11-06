#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "TvaPayloads.hpp"
namespace cromulence::tva
{
    // Command messages
    typedef cromulence::CmdMessage< empty , TVA_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , TVA_CMD_ID ,0 > ResetMsg;
    typedef cromulence::CmdMessage< TimeMark , TVA_CMD_ID ,1 > TimeMarkMsg;
    typedef cromulence::CmdMessage< empty , TVA_CMD_ID ,2 > DiscardMsg;
    typedef cromulence::CmdMessage< File , TVA_CMD_ID ,3 > LoadFileMsg;
    typedef cromulence::CmdMessage< empty , TVA_CMD_ID ,4 > StatusRequestMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< Status , TVA_STATUS_TLM_ID  > StatusMsg;
    typedef cromulence::TlmMessage< CommandMeta , TVA_EMITTED_TLM_ID  > EmittedMsg;
}