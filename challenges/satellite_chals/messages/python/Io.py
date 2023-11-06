
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




TIME_LEN = 32
MAX_REQUESTS = 4
MAX_DL_BUFFERS = 10
MAX_IMG_NAME_SIZE = 32
MAX_GPS_RESPONSE_SIZE = 1024
MAX_GPS_CMD_SIZE = 256
CAMERA_POWER = 4
GPS_POWER = 5
DEFAULT_EXPOSURE_US = 250
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class FileRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  2
        self.count += 1
        self.payload_format = Struct(
        'path' / PaddedString(64,'utf-8'),
        'start' / Int32ul,
        'nBytes' / Int32ul,
        )
class FilterModify(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  3
        self.count += 1
        self.payload_format = Struct(
        'msgid' / Int16ul,
        'allowed' / Int8ul,
        )
class FilterRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  4
        self.count += 1
        self.payload_format = Struct(
        'msgid' / Int16ul,
        )
class FilterDefaults(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  5
        self.count += 1
        self.payload_format = None
class HouseKeepingRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  6
        self.count += 1
        self.payload_format = None
class DevicePower(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  7
        self.count += 1
        self.payload_format = Struct(
        'device' / Int8ul,
        'power' / Int8ul,
        )
class AcbPoint(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  8
        self.count += 1
        self.payload_format = Struct(
        'qAxis' / Float32l[3],
        'qScalar' / Float32l,
        )
class AcbGetTlm(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  9
        self.count += 1
        self.payload_format = None
class NovatelCmd(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  11
        self.count += 1
        self.payload_format = Struct(
        'id' / Int16sl,
        'type' / Int8ul,
        'command' / PaddedString(MAX_GPS_CMD_SIZE,'utf-8'),
        )
class ImageSnap(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  13
        self.count += 1
        self.payload_format = Struct(
        'img' / PaddedString(MAX_IMG_NAME_SIZE,'utf-8'),
        )
class ImageList(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  14
        self.count += 1
        self.payload_format = None
class ImageGet(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  15
        self.count += 1
        self.payload_format = Struct(
        'img' / PaddedString(MAX_IMG_NAME_SIZE,'utf-8'),
        'download_directory' / PaddedString(MAX_IMG_NAME_SIZE,'utf-8'),
        )
class ImageDelete(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  16
        self.count += 1
        self.payload_format = Struct(
        'img' / PaddedString(MAX_IMG_NAME_SIZE,'utf-8'),
        )
class UpdateIoConfig(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  17
        self.count += 1
        self.payload_format = Struct(
        'chunkSize' / Int16ul,
        'waitNackHighDelay' / Int16ul,
        'waitNackHighFlag' / Int8ul,
        'muxDelay' / Int16ul,
        )
class SyncTime(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  18
        self.count += 1
        self.payload_format = None
class TimeQuery(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  19
        self.count += 1
        self.payload_format = None
class SetTime(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  20
        self.count += 1
        self.payload_format = Struct(
        'seconds' / Int32ul,
        'microseconds' / Int32ul,
        )
class SetExposure(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6272)
        self.fcn =  21
        self.count += 1
        self.payload_format = Struct(
        'microsecs' / Int32ul,
        )

class CmdAck(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3152)
        self.count +=1
        self.payload_format = Struct(
        'msgid' / Int16ul,
        'size' / Int16ul,
        'status' / Int8ul,
        'accepted' / Int32ul,
        'rejected' / Int32ul,
        'power_state' / Int8ul,
        'tlm' / Int32ul[MAX_DL_BUFFERS],
        )
    def name( self ):
        return "Io.CmdAck"
class Housekeeping(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3153)
        self.count +=1
        self.payload_format = Struct(
        'upPktAccepted' / Int32ul,
        'upPktRejected' / Int32ul,
        'upBytes' / Int64ul,
        'downBytes' / Int64ul,
        'downlinkChunkSize' / Int16ul,
        'waitNackHighDelay' / Int16ul,
        'waitNackHighState' / Int8ul,
        )
    def name( self ):
        return "Io.Housekeeping"
class DownlinkFileData(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3154)
        self.count +=1
        self.payload_format = Struct(
        'filename' / PaddedString(32,'utf-8'),
        'start' / Int32ul,
        'nBytes' / Int32ul,
        'filesize' / Int32ul,
        'crc32' / Int32ul,
        )
    def name( self ):
        return "Io.DownlinkFileData"
class Filter(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3155)
        self.count +=1
        self.payload_format = Struct(
        'msgid' / Int16ul,
        'allowed' / Int8ul,
        )
    def name( self ):
        return "Io.Filter"
class AcbState(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3156)
        self.count +=1
        self.payload_format = Struct(
        'utc' / Struct( "yr" / Int8ul,
                "mo" / Int8ul,
                "day" / Int8ul,
                "hr" / Int8ul,
                "min" / Int8ul,
                "sec" / Int8ul,
                "csec" / Int8ul,
                ),
        'mode' / Int8ul,
        'position' / Float32l[3],
        'velocity' / Float32l[3],
        'qAxis' / Float32l[3],
        'qScalar' / Float32l,
        'w' / Float32l[3],
        'star_tacker_delay' / Float32l,
        )
    def name( self ):
        return "Io.AcbState"
class AcbRawState(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3157)
        self.count +=1
        self.payload_format = Struct(
        'utc' / Struct( "yr" / Int8ul,
                "mo" / Int8ul,
                "day" / Int8ul,
                "hr" / Int8ul,
                "min" / Int8ul,
                "sec" / Int8ul,
                "csec" / Int8ul,
                ),
        'mode' / Int8ul,
        'submode' / Int8ul,
        'qAxis' / Float32l[3],
        'qScalar' / Float32l,
        'w' / Float32l[3],
        'starTrackerDelayMs' / Int16ul,
        'rwSpeedRps' / Int16ul[3],
        'mtbState' / Int8sl[3],
        'positionEci' / Float32l[3],
        'velocityEci' / Float32l[3],
        )
    def name( self ):
        return "Io.AcbRawState"
class NovatelOut(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3158)
        self.count +=1
        self.payload_format = Struct(
        'id' / Int16sl,
        'response' / PaddedString(MAX_GPS_RESPONSE_SIZE,'utf-8'),
        'crc' / PaddedString(8,'utf-8'),
        )
    def name( self ):
        return "Io.NovatelOut"
class CameraFiles(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3159)
        self.count +=1
        self.payload_format = Struct(
        'item' / Int16ul,
        'max_items' / Int16ul,
        'searchpath' / PaddedString(50,'utf-8'),
        'path' / PaddedString(50,'utf-8'),
        )
    def name( self ):
        return "Io.CameraFiles"
class CameraFtpDlStatus(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3160)
        self.count +=1
        self.payload_format = Struct(
        'img' / PaddedString(50,'utf-8'),
        'state' / Int8ul,
        )
    def name( self ):
        return "Io.CameraFtpDlStatus"
class PollingConfig(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3161)
        self.count +=1
        self.payload_format = Struct(
        'allowed' / Int8ul,
        )
    def name( self ):
        return "Io.PollingConfig"
class StrTime(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3162)
        self.count +=1
        self.payload_format = Struct(
        'time' / PaddedString(TIME_LEN,'utf-8'),
        )
    def name( self ):
        return "Io.StrTime"