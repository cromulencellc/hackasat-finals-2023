from moonlighter import CfeTimeHk,Staging,CfeTime,Heartbeat,sbn,Version,CfeSbHk,Filez,Script,ScriptUdp,Gps,Tlm,CfeHk,CfeEvsHk,Io,Attitude,Ping,CfeEvs,Tva,crackmewd,Geofence
# Tlm for all apps
sha = "fe3dbe7f829033804b96d7408deecccf5bc4adad77f1ea2f8cdebb6ce464be3b"
tlm  = {
    # CfeTimeHk App
    # Staging App
    3216: Staging.QueueStatus,
    3217: Staging.QueueList,
    # CfeTime App
    # Heartbeat App
    3136: Heartbeat.Pps,
    # sbn App
    # Version App
    3264: Version.VersionInfo,
    3265: Version.AppInfo,
    # CfeSbHk App
    # Filez App
    3088: Filez.Response,
    3089: Filez.ResponseShort,
    3090: Filez.FileList,
    # Script App
    3200: Script.Status,
    # ScriptUdp App
    3184: ScriptUdp.HouseKeeping,
    3185: ScriptUdp.Kiddie,
    # Gps App
    3120: Gps.Ecef,
    3121: Gps.Geodetic,
    # Tlm App
    3232: Tlm.FilterInfo,
    3233: Tlm.BufferDetails,
    # CfeHk App
    # CfeEvsHk App
    # Io App
    3152: Io.CmdAck,
    3153: Io.Housekeeping,
    3154: Io.DownlinkFileData,
    3155: Io.Filter,
    3156: Io.AcbState,
    3157: Io.AcbRawState,
    3158: Io.NovatelOut,
    3159: Io.CameraFiles,
    3160: Io.CameraFtpDlStatus,
    3161: Io.PollingConfig,
    3162: Io.StrTime,
    # Attitude App
    3072: Attitude.Housekeeping,
    3073: Attitude.AttitudeCommand,
    # Ping App
    3168: Ping.Pong,
    3169: Ping.SmolPong,
    3170: Ping.Spam,
    # CfeEvs App
    2056: CfeEvs.Event,
    # Tva App
    3248: Tva.Status,
    3249: Tva.Emitted,
    # crackmewd App
    3280: crackmewd.HouseKeeping,
    3281: crackmewd.CrackMeWdStatus,
    # Geofence App
    3104: Geofence.GeofenceZone,
    3105: Geofence.Rejected,
    3106: Geofence.SnapMeta,
}