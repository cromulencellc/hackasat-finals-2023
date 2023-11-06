
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




MAX_PORT = 9000
TEAM_SIZE = 16
MESSAGE_SIZE = 64
PATH_SIZE = 256
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6291)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6291)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class Configure(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6291)
        self.fcn =  2
        self.count += 1
        self.payload_format = Struct(
        'server_port' / Int16ul,
        )
class Team(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6291)
        self.fcn =  3
        self.count += 1
        self.payload_format = Struct(
        'name' / PaddedString(TEAM_SIZE,'utf-8'),
        )

class HouseKeeping(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3184)
        self.count +=1
        self.payload_format = Struct(
        'gps_count' / Int32ul,
        'image_count' / Int32ul,
        'stage_count' / Int32ul,
        )
    def name( self ):
        return "ScriptUdp.HouseKeeping"
class Kiddie(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3185)
        self.count +=1
        self.payload_format = Struct(
        'team' / PaddedString(TEAM_SIZE,'utf-8'),
        'message' / PaddedString(MESSAGE_SIZE,'utf-8'),
        )
    def name( self ):
        return "ScriptUdp.Kiddie"