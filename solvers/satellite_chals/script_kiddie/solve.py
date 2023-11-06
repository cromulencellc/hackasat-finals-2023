
import scripts

def solve( url , apikey , id , timestr  ):
    scripts.upload( url=url, apikey=apikey,script="script_kiddie/solve.lua", id=id )
    scripts.run( url=url, apikey=apikey, id=id, timestr=timestr )