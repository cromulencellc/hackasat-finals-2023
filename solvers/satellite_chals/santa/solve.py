import scripts

def solve( url , apikey , id , timestr  ):
    scripts.upload( url=url, apikey=apikey,script="santa/solver.lua", id=id )
    scripts.run( url=url, apikey=apikey, id=id, timestr=timestr )