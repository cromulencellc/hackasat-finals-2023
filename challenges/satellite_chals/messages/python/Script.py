
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




MAX_USER_SIZE = 16
MAX_NAME_SIZE = 256
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6279)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6279)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class Run(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6279)
        self.fcn =  2
        self.count += 1
        self.payload_format = Struct(
        'script' / Int8ul,
        )
class Kill(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6279)
        self.fcn =  3
        self.count += 1
        self.payload_format = None
class Load(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6279)
        self.fcn =  4
        self.count += 1
        self.payload_format = Struct(
        'script' / Int8ul,
        )
class SetUser(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6279)
        self.fcn =  5
        self.count += 1
        self.payload_format = Struct(
        'user' / PaddedString(MAX_USER_SIZE,'utf-8'),
        )

class Status(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3200)
        self.count +=1
        self.payload_format = Struct(
        'script' / PaddedString(MAX_NAME_SIZE,'utf-8'),
        'alive' / Int8ul,
        'status' / Int8ul,
        )
    def name( self ):
        return "Script.Status"