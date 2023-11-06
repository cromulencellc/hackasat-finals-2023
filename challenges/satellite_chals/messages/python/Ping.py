
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




PING_SIZE = 256
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6276)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6276)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class SmolPing(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6276)
        self.fcn =  2
        self.count += 1
        self.payload_format = None
class Ping(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6276)
        self.fcn =  3
        self.count += 1
        self.payload_format = Struct(
        'data' / PaddedString(PING_SIZE,'utf-8'),
        )
class SpamReq(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6276)
        self.fcn =  4
        self.count += 1
        self.payload_format = Struct(
        'nToSpam' / Int16ul,
        )

class Pong(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3168)
        self.count +=1
        self.payload_format = Struct(
        'time' / PaddedString(PING_SIZE,'utf-8'),
        'data' / PaddedString(256,'utf-8'),
        )
    def name( self ):
        return "Ping.Pong"
class SmolPong(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3169)
        self.count +=1
        self.payload_format = Struct(
        'time' / PaddedString(32,'utf-8'),
        )
    def name( self ):
        return "Ping.SmolPong"
class Spam(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3170)
        self.count +=1
        self.payload_format = Struct(
        'kb' / Int8ul[1012],
        )
    def name( self ):
        return "Ping.Spam"