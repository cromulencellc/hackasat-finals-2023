#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "StagingPayloads.hpp"
namespace cromulence::staging
{
    // Command messages
    typedef cromulence::CmdMessage< empty , STAGING_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , STAGING_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< RecoverFile , STAGING_CMD_ID ,2 > RecoverMsg;
    typedef cromulence::CmdMessage< QueueName , STAGING_CMD_ID ,3 > StatusRequestMsg;
    typedef cromulence::CmdMessage< QueueSize , STAGING_CMD_ID ,4 > QueueSizeMsg;
    typedef cromulence::CmdMessage< QueueId , STAGING_CMD_ID ,5 > QueueEmptyMsg;
    typedef cromulence::CmdMessage< StagingSize , STAGING_CMD_ID ,6 > StagingSizeMsg;
    typedef cromulence::CmdMessage< Image , STAGING_CMD_ID ,7 > ConfirmDownloadMsg;
    typedef cromulence::CmdMessage< TeamImage , STAGING_CMD_ID ,8 > AddImageMsg;
    typedef cromulence::CmdMessage< empty , STAGING_CMD_ID ,9 > StageImagesMsg;
    typedef cromulence::CmdMessage< TeamImage , STAGING_CMD_ID ,10 > RemoveMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< QueueStatus , STAGING_QUEUESTATUS_TLM_ID  > QueueStatusMsg;
    typedef cromulence::TlmMessage< QueueListing , STAGING_QUEUELIST_TLM_ID  > QueueListMsg;
}