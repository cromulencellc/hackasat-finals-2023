
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




FILE_PATH_MAX_SIZE = 32
SYNC_WORD = 431942429
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6281)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6281)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class TimeMark(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6281)
        self.fcn =  1
        self.count += 1
        self.payload_format = Struct(
        'yr' / Int8ul,
        'mo' / Int8ul,
        'day' / Int8ul,
        'hr' / Int8ul,
        'min' / Int8ul,
        'sec' / Int8ul,
        )
class Discard(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6281)
        self.fcn =  2
        self.count += 1
        self.payload_format = None
class LoadFile(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6281)
        self.fcn =  3
        self.count += 1
        self.payload_format = Struct(
        'name' / PaddedString(FILE_PATH_MAX_SIZE,'utf-8'),
        )
class StatusRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6281)
        self.fcn =  4
        self.count += 1
        self.payload_format = None

class Status(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3248)
        self.count +=1
        self.payload_format = Struct(
        'num_loaded' / Int32ul,
        'current_position' / Int32ul,
        'load_status' / Int8ul,
        'filename' / PaddedString(FILE_PATH_MAX_SIZE,'utf-8'),
        )
    def name( self ):
        return "Tva.Status"
class Emitted(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3249)
        self.count +=1
        self.payload_format = Struct(
        'utc_sec' / Float64l,
        'command_id' / Int16ul,
        'function' / Int8ul,
        'tGoMilliSec' / Int64sl,
        )
    def name( self ):
        return "Tva.Emitted"