
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




QUEUE_NAME_LENGTH = 32
FILE_NAME_LENGTH = 64
MAX_USERS = 5
MAX_ITEMS = 10
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class Recover(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  2
        self.count += 1
        self.payload_format = Struct(
        'name' / PaddedString(FILE_NAME_LENGTH,'utf-8'),
        )
class StatusRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  3
        self.count += 1
        self.payload_format = Struct(
        'name' / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
        )
class QueueSize(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  4
        self.count += 1
        self.payload_format = Struct(
        'queue' / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
        'max_entries' / Int8ul,
        )
class QueueEmpty(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  5
        self.count += 1
        self.payload_format = Struct(
        'queue' / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
        )
class StagingSize(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  6
        self.count += 1
        self.payload_format = Struct(
        'max_images' / Int8ul,
        )
class ConfirmDownload(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  7
        self.count += 1
        self.payload_format = Struct(
        'img' / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
        )
class AddImage(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  8
        self.count += 1
        self.payload_format = Struct(
        'team' / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
        'img' / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
        )
class StageImages(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  9
        self.count += 1
        self.payload_format = None
class Remove(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6280)
        self.fcn =  10
        self.count += 1
        self.payload_format = Struct(
        'team' / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
        'img' / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
        )

class QueueStatus(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3216)
        self.count +=1
        self.payload_format = Struct(
        'name' / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
        'item' / Struct( "img" / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
                "status" / Int8ul,
                ),
        )
    def name( self ):
        return "Staging.QueueStatus"
class QueueList(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3217)
        self.count +=1
        self.payload_format = Struct(
        'queues' / Struct( "name" / PaddedString(QUEUE_NAME_LENGTH,'utf-8'),
                )[QUEUE_NAME_LENGTH],
        )
    def name( self ):
        return "Staging.QueueList"