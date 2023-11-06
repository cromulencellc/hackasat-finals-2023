#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "PingPayloads.hpp"
namespace cromulence::ping
{
    // Command messages
    typedef cromulence::CmdMessage< empty , PING_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , PING_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< empty , PING_CMD_ID ,2 > SmolPingMsg;
    typedef cromulence::CmdMessage< Ping , PING_CMD_ID ,3 > PingMsg;
    typedef cromulence::CmdMessage< SpamReq , PING_CMD_ID ,4 > SpamReqMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< Pong , PING_PONG_TLM_ID  > PongMsg;
    typedef cromulence::TlmMessage< SmolPong , PING_SMOLPONG_TLM_ID  > SmolPongMsg;
    typedef cromulence::TlmMessage< Spam , PING_SPAM_TLM_ID  > SpamMsg;
}