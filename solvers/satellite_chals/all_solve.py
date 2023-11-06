import image.solve as IS
import script_kiddie.solve as SK
import geofence.solve as GF 
import ironbank.solve as IB
import santa.solve as XMAS
from datetime import datetime, timedelta

BASE_TIME_STR = "2023-08-10 05:38:05.00"

BASE_TIME_FORMAT = "%Y-%m-%d %H:%M:%S.%f"
BASE_TIME = datetime.strptime(BASE_TIME_STR, BASE_TIME_FORMAT)
date_to_str = lambda date : date.strftime("%Y-%m-%d %H:%M:%S") + "{:.2f}".format(date.microsecond/10**6)[1:]

url = "https://userapi.REDACTED.satellitesabove.com"
apikey = "redacted-key"

snapTime = date_to_str(BASE_TIME)
snapAxis =[0,0,0]
snapScalar = 1.0

pwnFence = False
pwnTime = date_to_str(BASE_TIME + timedelta(seconds=10))
rejectedTime = date_to_str(BASE_TIME + timedelta(seconds=20))
rejectedAxis=[0,0,0]
rejectedScalar=0

kiddieTime =  date_to_str(BASE_TIME + timedelta(seconds=60))
ironbankTime =  date_to_str(BASE_TIME + timedelta(seconds=60))
santaTime = date_to_str( BASE_TIME + timedelta( seconds=60) ) 
crackmeTime =  date_to_str(BASE_TIME + timedelta(seconds=50))

# A good snap
IS.solve( url=url , apikey=apikey, timestr=snapTime , qAxis=snapAxis, qS=snapScalar, id=1 )

# A rejected snap 
if pwnFence:
    GF.solve( url=url, apikey=apikey , id=0 , timestr=pwnTime)
IS.solve( url=url , apikey=apikey, timestr=rejectedTime , qAxis=rejectedAxis, qS=rejectedScalar, id=2 )

#  Script kidz
SK.solve( url=url, apikey=apikey , id=1 , timestr=kiddieTime )

# Ironbank
IB.solve( url=url, apikey=apikey , id=2 , timestr=ironbankTime)

# Santa 
XMAS.solve( url=url, apikey=apikey , id=0 , timestr=santaTime)
