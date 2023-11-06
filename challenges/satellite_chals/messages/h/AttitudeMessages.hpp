#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "AttitudePayloads.hpp"
namespace cromulence::attitude
{
    // Command messages
    typedef cromulence::CmdMessage< empty , ATTITUDE_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , ATTITUDE_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< Config , ATTITUDE_CMD_ID ,2 > ConfigMsg;
    typedef cromulence::CmdMessage< Attitude , ATTITUDE_CMD_ID ,3 > RequestMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< HouseKeeping , ATTITUDE_HOUSEKEEPING_TLM_ID  > HousekeepingMsg;
    typedef cromulence::TlmMessage< Attitude , ATTITUDE_ATTITUDECOMMAND_TLM_ID  > AttitudeCommandMsg;
}