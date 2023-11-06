
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




class CfeTimeSendHk(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6157)
        self.fcn =  0
        self.count += 1
        self.payload_format = None

