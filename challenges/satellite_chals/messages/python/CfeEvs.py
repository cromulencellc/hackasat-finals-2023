
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class






class Event(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(2056)
        self.count +=1
        self.payload_format = Struct(
        'app_name' / PaddedString(20,'utf-8'),
        'event_id' / Int16ul,
        'event_type' / Int16ul,
        'spacecraft_id' / Int32ul,
        'processor_id' / Int32ul,
        'message' / PaddedString(976,'utf-8'),
        'spare' / Int16ul,
        )
    def name( self ):
        return "CfeEvs.Event"