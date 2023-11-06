
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




MAX_NAME_SIZE = 32
MAX_APPS = 10
SHA_SIZE = 32
FPGA_NAME_SIZE = 40
TIME_SIZE = 10
DATE_SIZE = 16
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6275)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6275)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class AppRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6275)
        self.fcn =  2
        self.count += 1
        self.payload_format = None
class InfoRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6275)
        self.fcn =  3
        self.count += 1
        self.payload_format = None

class VersionInfo(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3264)
        self.count +=1
        self.payload_format = Struct(
        'sw_build_date' / PaddedString(DATE_SIZE,'utf-8'),
        'sw_build_time' / PaddedString(TIME_SIZE,'utf-8'),
        'fpga_version' / PaddedString(FPGA_NAME_SIZE,'utf-8'),
        )
    def name( self ):
        return "Version.VersionInfo"
class AppInfo(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3265)
        self.count +=1
        self.payload_format = Struct(
        'app_names' / Struct( "app" / PaddedString(MAX_NAME_SIZE,'utf-8'),
                )[MAX_NAME_SIZE],
        'app_sha' / Struct( "sha" / PaddedString(SHA_SIZE,'utf-8'),
                )[SHA_SIZE],
        )
    def name( self ):
        return "Version.AppInfo"