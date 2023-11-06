import scripts

def solve( url , apikey , id , timestr  ):
    scripts.upload( url=url, apikey=apikey,script="geofence/basic_pwn.lua", id=id )
    scripts.run( url=url, apikey=apikey, id=id, timestr=timestr )