#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "sbnPayloads.hpp"
namespace cromulence::sbn
{
    // Command messages
    typedef cromulence::CmdMessage< empty , SBN_CMD_ID ,0 > NoOpMsg;
    // Telemetry messages
}