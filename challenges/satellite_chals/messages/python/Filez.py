
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




PATH_SIZE = 64
MAX_FILENAME_SIZE = 32
MAX_FILE_LIST = 10
SHA_SIZE = 32
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6288)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6288)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class Snippet(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6288)
        self.fcn =  2
        self.count += 1
        self.payload_format = Struct(
        'filename' / PaddedString(MAX_FILENAME_SIZE,'utf-8'),
        'totalFileSize' / Int32ul,
        'startSeek' / Int32ul,
        'packetSize' / Int32ul,
        'packet' / Int8ul[1024],
        'packetStatus' / Int8ul,
        )
class Delete(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6288)
        self.fcn =  3
        self.count += 1
        self.payload_format = Struct(
        'path' / PaddedString(PATH_SIZE,'utf-8'),
        )
class DirRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6288)
        self.fcn =  4
        self.count += 1
        self.payload_format = Struct(
        'path' / PaddedString(PATH_SIZE,'utf-8'),
        'pattern' / PaddedString(32,'utf-8'),
        )
class Copy(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6288)
        self.fcn =  5
        self.count += 1
        self.payload_format = Struct(
        'src' / PaddedString(PATH_SIZE,'utf-8'),
        'dst' / PaddedString(PATH_SIZE,'utf-8'),
        'overwrite' / Int8ul,
        )
class Unpack(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6288)
        self.fcn =  6
        self.count += 1
        self.payload_format = Struct(
        'path' / PaddedString(PATH_SIZE,'utf-8'),
        'file' / PaddedString(MAX_FILENAME_SIZE,'utf-8'),
        )

class Response(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3088)
        self.count +=1
        self.payload_format = Struct(
        'path' / PaddedString(PATH_SIZE,'utf-8'),
        'totalFileSize' / Int32ul,
        'currentSha' / PaddedString(SHA_SIZE,'utf-8'),
        )
    def name( self ):
        return "Filez.Response"
class ResponseShort(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3089)
        self.count +=1
        self.payload_format = Struct(
        'path' / PaddedString(PATH_SIZE,'utf-8'),
        'totalFileSize' / Int32ul,
        'crc32' / Int32ul,
        )
    def name( self ):
        return "Filez.ResponseShort"
class FileList(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3090)
        self.count +=1
        self.payload_format = Struct(
        'ls' / Struct( "path" / PaddedString(64,'utf-8'),
                "size" / Int32ul,
                ),
        )
    def name( self ):
        return "Filez.FileList"