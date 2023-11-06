#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "IoPayloads.hpp"
namespace moonlighter::io
{
    // Command messages
    typedef cromulence::CmdMessage< empty , IO_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , IO_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< FileRequest , IO_CMD_ID ,2 > FileRequestMsg;
    typedef cromulence::CmdMessage< FilterSettings , IO_CMD_ID ,3 > FilterModifyMsg;
    typedef cromulence::CmdMessage< FilterRequest , IO_CMD_ID ,4 > FilterRequestMsg;
    typedef cromulence::CmdMessage< empty , IO_CMD_ID ,5 > FilterDefaultsMsg;
    typedef cromulence::CmdMessage< empty , IO_CMD_ID ,6 > HouseKeepingRequestMsg;
    typedef cromulence::CmdMessage< Power , IO_CMD_ID ,7 > DevicePowerMsg;
    typedef cromulence::CmdMessage< AcbPoint , IO_CMD_ID ,8 > AcbPointMsg;
    typedef cromulence::CmdMessage< empty , IO_CMD_ID ,9 > AcbGetTlmMsg;
    typedef cromulence::CmdMessage< NovatelCommand , IO_CMD_ID ,11 > NovatelCmdMsg;
    typedef cromulence::CmdMessage< Picture , IO_CMD_ID ,13 > ImageSnapMsg;
    typedef cromulence::CmdMessage< empty , IO_CMD_ID ,14 > ImageListMsg;
    typedef cromulence::CmdMessage< PictureDownload , IO_CMD_ID ,15 > ImageGetMsg;
    typedef cromulence::CmdMessage< Picture , IO_CMD_ID ,16 > ImageDeleteMsg;
    typedef cromulence::CmdMessage< UpdateIoConfig , IO_CMD_ID ,17 > UpdateIoConfigMsg;
    typedef cromulence::CmdMessage< empty , IO_CMD_ID ,18 > SyncTimeMsg;
    typedef cromulence::CmdMessage< empty , IO_CMD_ID ,19 > TimeQueryMsg;
    typedef cromulence::CmdMessage< IntTime , IO_CMD_ID ,20 > SetTimeMsg;
    typedef cromulence::CmdMessage< Exposure , IO_CMD_ID ,21 > SetExposureMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< CmdAck , IO_CMDACK_TLM_ID  > CmdAckMsg;
    typedef cromulence::TlmMessage< HouseKeeping , IO_HOUSEKEEPING_TLM_ID  > HousekeepingMsg;
    typedef cromulence::TlmMessage< DownlinkFileData , IO_DOWNLINKFILEDATA_TLM_ID  > DownlinkFileDataMsg;
    typedef cromulence::TlmMessage< FilterSettings , IO_FILTER_TLM_ID  > FilterMsg;
    typedef cromulence::TlmMessage< AcbState , IO_ACBSTATE_TLM_ID  > AcbStateMsg;
    typedef cromulence::TlmMessage< AcbRaw , IO_ACBRAWSTATE_TLM_ID  > AcbRawStateMsg;
    typedef cromulence::TlmMessage< NovatelResponse , IO_NOVATELOUT_TLM_ID  > NovatelOutMsg;
    typedef cromulence::TlmMessage< CameraFile , IO_CAMERAFILES_TLM_ID  > CameraFilesMsg;
    typedef cromulence::TlmMessage< ImageState , IO_CAMERAFTPDLSTATUS_TLM_ID  > CameraFtpDlStatusMsg;
    typedef cromulence::TlmMessage< Polling , IO_POLLINGCONFIG_TLM_ID  > PollingConfigMsg;
    typedef cromulence::TlmMessage< StrTime , IO_STRTIME_TLM_ID  > StrTimeMsg;
}