
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




EVT_ID = 64
DEFAULT_BLIND = 600
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6292)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6292)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class Config(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6292)
        self.fcn =  2
        self.count += 1
        self.payload_format = Struct(
        'max_blind' / Float64l,
        'frame_count' / Int16ul,
        'enabled' / Int8ul,
        )
class Request(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6292)
        self.fcn =  3
        self.count += 1
        self.payload_format = Struct(
        'qAxis' / Float64l[3],
        'qScalar' / Float64l,
        )

class Housekeeping(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3072)
        self.count +=1
        self.payload_format = Struct(
        'accepted' / Int32ul,
        'rejected' / Int32ul,
        'blind_sec' / Int32ul,
        )
    def name( self ):
        return "Attitude.Housekeeping"
class AttitudeCommand(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3073)
        self.count +=1
        self.payload_format = Struct(
        'qAxis' / Float64l[3],
        'qScalar' / Float64l,
        )
    def name( self ):
        return "Attitude.AttitudeCommand"