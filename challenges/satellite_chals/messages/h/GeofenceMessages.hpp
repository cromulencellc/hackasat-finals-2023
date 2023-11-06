#pragma once
#include "Message.h"
#include "MessageIds.hpp"
#include "GeofencePayloads.hpp"
namespace cromulence::geofence
{
    // Command messages
    typedef cromulence::CmdMessage< empty , GEOFENCE_CMD_ID ,0 > NoOpMsg;
    typedef cromulence::CmdMessage< empty , GEOFENCE_CMD_ID ,1 > ResetMsg;
    typedef cromulence::CmdMessage< empty , GEOFENCE_CMD_ID ,2 > GetMsg;
    typedef cromulence::CmdMessage< TimeOffsets , GEOFENCE_CMD_ID ,3 > TimeOffsetsMsg;
    typedef cromulence::CmdMessage< LoadGeofence , GEOFENCE_CMD_ID ,4 > LoadGeofenceMsg;
    typedef cromulence::CmdMessage< SnapRequest , GEOFENCE_CMD_ID ,5 > SnapRequestMsg;
    typedef cromulence::CmdMessage< Team , GEOFENCE_CMD_ID ,6 > TeamChangeMsg;
    typedef cromulence::CmdMessage< SnapRequest , GEOFENCE_CMD_ID ,7 > DownloadRequestMsg;
    // Telemetry messages
    typedef cromulence::TlmMessage< Zone , GEOFENCE_GEOFENCEZONE_TLM_ID  > GeofenceZoneMsg;
    typedef cromulence::TlmMessage< GeofenceName , GEOFENCE_REJECTED_TLM_ID  > RejectedMsg;
    typedef cromulence::TlmMessage< SnapMeta , GEOFENCE_SNAPMETA_TLM_ID  > SnapMetaMsg;
}