#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "TlmPayloads.hpp"
namespace cromulence::tlm
{
    // Command messages
    typedef cromulence::CmdMessage< empty , TLM_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , TLM_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< empty , TLM_CMD_ID ,2 > ClearMsg;
    typedef cromulence::CmdMessage< empty , TLM_CMD_ID ,3 > DefaultFiltersMsg;
    typedef cromulence::CmdMessage< FilterSettings , TLM_CMD_ID ,4 > ModifyFilterMsg;
    typedef cromulence::CmdMessage< FilterRequest , TLM_CMD_ID ,5 > FilterRequestMsg;
    typedef cromulence::CmdMessage< BufferRequest , TLM_CMD_ID ,6 > SaveBufferMsg;
    typedef cromulence::CmdMessage< BufferRequest , TLM_CMD_ID ,7 > RestoreBufferMsg;
    typedef cromulence::CmdMessage< BufferRequest , TLM_CMD_ID ,8 > BufferRequestMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< FilterStatus , TLM_FILTERINFO_TLM_ID  > FilterInfoMsg;
    typedef cromulence::TlmMessage< BufferDetails , TLM_BUFFERDETAILS_TLM_ID  > BufferDetailsMsg;
}