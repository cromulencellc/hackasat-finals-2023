
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




MAX_BUFFERS = 10
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6273)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6273)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class Clear(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6273)
        self.fcn =  2
        self.count += 1
        self.payload_format = None
class DefaultFilters(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6273)
        self.fcn =  3
        self.count += 1
        self.payload_format = None
class ModifyFilter(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6273)
        self.fcn =  4
        self.count += 1
        self.payload_format = Struct(
        'apid' / Int16ul,
        'depth' / Int16ul,
        'decimation' / Int8ul,
        'allowed' / Int8ul,
        'queue' / Int8ul,
        )
class FilterRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6273)
        self.fcn =  5
        self.count += 1
        self.payload_format = Struct(
        'apid' / Int16ul,
        )
class SaveBuffer(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6273)
        self.fcn =  6
        self.count += 1
        self.payload_format = Struct(
        'bufferId' / Int8ul,
        )
class RestoreBuffer(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6273)
        self.fcn =  7
        self.count += 1
        self.payload_format = Struct(
        'bufferId' / Int8ul,
        )
class BufferRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6273)
        self.fcn =  8
        self.count += 1
        self.payload_format = Struct(
        'bufferId' / Int8ul,
        )

class FilterInfo(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3232)
        self.count +=1
        self.payload_format = Struct(
        'apid' / Int16ul,
        'depth' / Int16ul,
        'decimation' / Int8ul,
        'allowed' / Int8ul,
        'queue' / Int8ul,
        'decimationCtr' / Int32ul,
        )
    def name( self ):
        return "Tlm.FilterInfo"
class BufferDetails(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3233)
        self.count +=1
        self.payload_format = Struct(
        'path' / PaddedString(32,'utf-8'),
        'lastStore' / PaddedString(32,'utf-8'),
        'bufferId' / Int8ul,
        'size' / Int32ul,
        )
    def name( self ):
        return "Tlm.BufferDetails"