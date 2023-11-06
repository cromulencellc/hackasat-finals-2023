
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




EVT_ID = 201
DATUM_LEN = 16
SOLN_LEN = 32
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6278)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6278)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class Config(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6278)
        self.fcn =  2
        self.count += 1
        self.payload_format = Struct(
        'housekeeping_rate' / Int8ul,
        'polling_rate' / Int8ul,
        'polling_enabled' / Int8ul,
        )

class Ecef(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3120)
        self.count +=1
        self.payload_format = Struct(
        'position' / Float64l[3],
        )
    def name( self ):
        return "Gps.Ecef"
class Geodetic(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3121)
        self.count +=1
        self.payload_format = Struct(
        'longitude' / Float32l,
        'latitude' / Float32l,
        'altitude' / Float32l,
        'dev_lat' / Float32l,
        'dev_lon' / Float32l,
        'dev_alt' / Float32l,
        'soln_age' / Float32l,
        'sats_tracked' / Int8ul,
        'sats_soln' / Int8ul,
        'solution_info' / PaddedString(SOLN_LEN,'utf-8'),
        'datum' / PaddedString(DATUM_LEN,'utf-8'),
        )
    def name( self ):
        return "Gps.Geodetic"