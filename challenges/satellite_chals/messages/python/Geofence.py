
from spacepackets.ccsds.spacepacket import SpacePacketHeader, PacketType
from construct import *
from construct import Int8ul,Int16ul,Int32ul, Int64ul,Int8sl,Int16sl,Int32sl, Int64sl, Float32l, Float64l, PaddedString
import struct
from moonlighter import base_class




PATH_SIZE = 64
MAX_NAME_SIZE = 64
ZONE_NAME_SIZE = 32
TEAM_NAME_SIZE = 16
MAX_POLYGON_POINTS = 10
class NoOp(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6277)
        self.fcn =  0
        self.count += 1
        self.payload_format = None
class Reset(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6277)
        self.fcn =  1
        self.count += 1
        self.payload_format = None
class Get(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6277)
        self.fcn =  2
        self.count += 1
        self.payload_format = None
class TimeOffsets(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6277)
        self.fcn =  3
        self.count += 1
        self.payload_format = Struct(
        'leapseconds' / Float64l,
        'deltaUT1' / Float64l,
        )
class LoadGeofence(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6277)
        self.fcn =  4
        self.count += 1
        self.payload_format = Struct(
        'pathname' / PaddedString(PATH_SIZE,'utf-8'),
        )
class SnapRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6277)
        self.fcn =  5
        self.count += 1
        self.payload_format = Struct(
        'image_id' / Int8ul,
        )
class TeamChange(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6277)
        self.fcn =  6
        self.count += 1
        self.payload_format = Struct(
        'name' / PaddedString(TEAM_NAME_SIZE,'utf-8'),
        )
class DownloadRequest(base_class.Cmd):
    count = 0
    def __init__(self ):
        super( ).__init__(6277)
        self.fcn =  7
        self.count += 1
        self.payload_format = Struct(
        'image_id' / Int8ul,
        )

class GeofenceZone(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3104)
        self.count +=1
        self.payload_format = Struct(
        'name' / PaddedString(ZONE_NAME_SIZE,'utf-8'),
        'nZones' / Int8ul,
        'polygon' / Struct( "lat" / Float64l,
                "lon" / Float64l,
                ),
        )
    def name( self ):
        return "Geofence.GeofenceZone"
class Rejected(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3105)
        self.count +=1
        self.payload_format = Struct(
        'tai' / Float64l,
        'name' / PaddedString(ZONE_NAME_SIZE,'utf-8'),
        )
    def name( self ):
        return "Geofence.Rejected"
class SnapMeta(base_class.Tlm):
    count = 0
    def __init__(self):
        super( ).__init__(3106)
        self.count +=1
        self.payload_format = Struct(
        'timeJd' / Float64l,
        'gpsPosition' / Float32l[3],
        'qAxis' / Float32l[3],
        'qScalar' / Float32l,
        'team' / PaddedString(TEAM_NAME_SIZE,'utf-8'),
        )
    def name( self ):
        return "Geofence.SnapMeta"