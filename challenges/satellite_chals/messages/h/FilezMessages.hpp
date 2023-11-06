#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "FilezPayloads.hpp"
namespace cromulence::filez
{
    // Command messages
    typedef cromulence::CmdMessage< empty , FILEZ_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , FILEZ_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< Snippet , FILEZ_CMD_ID ,2 > SnippetMsg;
    typedef cromulence::CmdMessage< Delete , FILEZ_CMD_ID ,3 > DeleteMsg;
    typedef cromulence::CmdMessage< DirRequest , FILEZ_CMD_ID ,4 > DirRequestMsg;
    typedef cromulence::CmdMessage< Copy , FILEZ_CMD_ID ,5 > CopyMsg;
    typedef cromulence::CmdMessage< UnPack , FILEZ_CMD_ID ,6 > UnpackMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< Response , FILEZ_RESPONSE_TLM_ID  > ResponseMsg;
    typedef cromulence::TlmMessage< ResponseShort , FILEZ_RESPONSESHORT_TLM_ID  > ResponseShortMsg;
    typedef cromulence::TlmMessage< FileList , FILEZ_FILELIST_TLM_ID  > FileListMsg;
}