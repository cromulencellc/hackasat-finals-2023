#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "VersionPayloads.hpp"
namespace cromulence::version
{
    // Command messages
    typedef cromulence::CmdMessage< empty , VERSION_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , VERSION_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< empty , VERSION_CMD_ID ,2 > AppRequestMsg;
    typedef cromulence::CmdMessage< empty , VERSION_CMD_ID ,3 > InfoRequestMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< VersionInfo , VERSION_VERSIONINFO_TLM_ID  > VersionInfoMsg;
    typedef cromulence::TlmMessage< AppInfo , VERSION_APPINFO_TLM_ID  > AppInfoMsg;
}