#pragma once
extern "C" {
   #include "cfe.h"
}
// schema sha
static constexpr const char* SCHEMA_SHA = "fe3dbe7f829033804b96d7408deecccf5bc4adad77f1ea2f8cdebb6ce464be3b";
// Command message ids
static const CFE_SB_MsgId_t CFETIMEHK_CMD_ID = 0x180d ;
static const CFE_SB_MsgId_t STAGING_CMD_ID = 0x1888 ;
static const CFE_SB_MsgId_t CFETIME_CMD_ID = 0x1805 ;
static const CFE_SB_MsgId_t HEARTBEAT_CMD_ID = 0x1892 ;
static const CFE_SB_MsgId_t SBN_CMD_ID = 0x1891 ;
static const CFE_SB_MsgId_t VERSION_CMD_ID = 0x1883 ;
static const CFE_SB_MsgId_t CFESBHK_CMD_ID = 0x180b ;
static const CFE_SB_MsgId_t FILEZ_CMD_ID = 0x1890 ;
static const CFE_SB_MsgId_t SCRIPT_CMD_ID = 0x1887 ;
static const CFE_SB_MsgId_t SCRIPTUDP_CMD_ID = 0x1893 ;
static const CFE_SB_MsgId_t GPS_CMD_ID = 0x1886 ;
static const CFE_SB_MsgId_t TLM_CMD_ID = 0x1881 ;
static const CFE_SB_MsgId_t CFEHK_CMD_ID = 0x1808 ;
static const CFE_SB_MsgId_t CFEEVSHK_CMD_ID = 0x1809 ;
static const CFE_SB_MsgId_t IO_CMD_ID = 0x1880 ;
static const CFE_SB_MsgId_t ATTITUDE_CMD_ID = 0x1894 ;
static const CFE_SB_MsgId_t PING_CMD_ID = 0x1884 ;
static const CFE_SB_MsgId_t CFEEVS_CMD_ID = 0x1801 ;
static const CFE_SB_MsgId_t TVA_CMD_ID = 0x1889 ;
static const CFE_SB_MsgId_t CRACKMEWD_CMD_ID = 0x1895 ;
static const CFE_SB_MsgId_t GEOFENCE_CMD_ID = 0x1885 ;
// Telemetry message ids
    // TLM for app CfeTimeHk
    // TLM for app Staging
static const CFE_SB_MsgId_t STAGING_QUEUESTATUS_TLM_ID = 0x0c90;
static const CFE_SB_MsgId_t STAGING_QUEUELIST_TLM_ID = 0x0c91;
    // TLM for app CfeTime
    // TLM for app Heartbeat
static const CFE_SB_MsgId_t HEARTBEAT_PPS_TLM_ID = 0x0c40;
    // TLM for app sbn
    // TLM for app Version
static const CFE_SB_MsgId_t VERSION_VERSIONINFO_TLM_ID = 0x0cc0;
static const CFE_SB_MsgId_t VERSION_APPINFO_TLM_ID = 0x0cc1;
    // TLM for app CfeSbHk
    // TLM for app Filez
static const CFE_SB_MsgId_t FILEZ_RESPONSE_TLM_ID = 0x0c10;
static const CFE_SB_MsgId_t FILEZ_RESPONSESHORT_TLM_ID = 0x0c11;
static const CFE_SB_MsgId_t FILEZ_FILELIST_TLM_ID = 0x0c12;
    // TLM for app Script
static const CFE_SB_MsgId_t SCRIPT_STATUS_TLM_ID = 0x0c80;
    // TLM for app ScriptUdp
static const CFE_SB_MsgId_t SCRIPTUDP_HOUSEKEEPING_TLM_ID = 0x0c70;
static const CFE_SB_MsgId_t SCRIPTUDP_KIDDIE_TLM_ID = 0x0c71;
    // TLM for app Gps
static const CFE_SB_MsgId_t GPS_ECEF_TLM_ID = 0x0c30;
static const CFE_SB_MsgId_t GPS_GEODETIC_TLM_ID = 0x0c31;
    // TLM for app Tlm
static const CFE_SB_MsgId_t TLM_FILTERINFO_TLM_ID = 0x0ca0;
static const CFE_SB_MsgId_t TLM_BUFFERDETAILS_TLM_ID = 0x0ca1;
    // TLM for app CfeHk
    // TLM for app CfeEvsHk
    // TLM for app Io
static const CFE_SB_MsgId_t IO_CMDACK_TLM_ID = 0x0c50;
static const CFE_SB_MsgId_t IO_HOUSEKEEPING_TLM_ID = 0x0c51;
static const CFE_SB_MsgId_t IO_DOWNLINKFILEDATA_TLM_ID = 0x0c52;
static const CFE_SB_MsgId_t IO_FILTER_TLM_ID = 0x0c53;
static const CFE_SB_MsgId_t IO_ACBSTATE_TLM_ID = 0x0c54;
static const CFE_SB_MsgId_t IO_ACBRAWSTATE_TLM_ID = 0x0c55;
static const CFE_SB_MsgId_t IO_NOVATELOUT_TLM_ID = 0x0c56;
static const CFE_SB_MsgId_t IO_CAMERAFILES_TLM_ID = 0x0c57;
static const CFE_SB_MsgId_t IO_CAMERAFTPDLSTATUS_TLM_ID = 0x0c58;
static const CFE_SB_MsgId_t IO_POLLINGCONFIG_TLM_ID = 0x0c59;
static const CFE_SB_MsgId_t IO_STRTIME_TLM_ID = 0x0c5a;
    // TLM for app Attitude
static const CFE_SB_MsgId_t ATTITUDE_HOUSEKEEPING_TLM_ID = 0x0c00;
static const CFE_SB_MsgId_t ATTITUDE_ATTITUDECOMMAND_TLM_ID = 0x0c01;
    // TLM for app Ping
static const CFE_SB_MsgId_t PING_PONG_TLM_ID = 0x0c60;
static const CFE_SB_MsgId_t PING_SMOLPONG_TLM_ID = 0x0c61;
static const CFE_SB_MsgId_t PING_SPAM_TLM_ID = 0x0c62;
    // TLM for app CfeEvs
static const CFE_SB_MsgId_t CFEEVS_EVENT_TLM_ID = 0x0808;
    // TLM for app Tva
static const CFE_SB_MsgId_t TVA_STATUS_TLM_ID = 0x0cb0;
static const CFE_SB_MsgId_t TVA_EMITTED_TLM_ID = 0x0cb1;
    // TLM for app crackmewd
static const CFE_SB_MsgId_t CRACKMEWD_HOUSEKEEPING_TLM_ID = 0x0cd0;
static const CFE_SB_MsgId_t CRACKMEWD_CRACKMEWDSTATUS_TLM_ID = 0x0cd1;
    // TLM for app Geofence
static const CFE_SB_MsgId_t GEOFENCE_GEOFENCEZONE_TLM_ID = 0x0c20;
static const CFE_SB_MsgId_t GEOFENCE_REJECTED_TLM_ID = 0x0c21;
static const CFE_SB_MsgId_t GEOFENCE_SNAPMETA_TLM_ID = 0x0c22;