import requests
import datetime as dt
time_format = "%Y-%m-%d %H:%M:%S.%f"

def time_str( dto ):
    return dto.strftime( time_format )[:-4]

def solve( url , apikey, timestr , qAxis , qS, id):
    data_header = {'accept': 'application/json', 
               "x-api-key": apikey
               }

    start = dt.datetime.strptime( timestr , time_format )
    
    camOnTime = start + dt.timedelta( seconds=-15 )
    pointTime = start + dt.timedelta( seconds=-10 )
    snapTime  = start + dt.timedelta( seconds=0 )
    ftpTime   = start + dt.timedelta( seconds=10)
    camOffTime = start + dt.timedelta( seconds=15 )

    camOn = requests.post(f'{url}/tasking/camera_power', 
                 headers=data_header,
                 json={ 'time': time_str( camOnTime),
                        'on': True   } )
    print( f"Camera On: {camOn}")
    # Point mr sat
    p = {
         "time":  time_str(pointTime),
         "qx": qAxis[0],
         "qy": qAxis[1],
         "qz": qAxis[2],
         "qS": qS }
    point = requests.post(f'{url}/tasking/pointing',
                          headers=data_header,
                          json=p)
    print( f"Pointing: {point}")
    
    # Take a picture
    pic = requests.post(f'{url}/tasking/image' ,
                    headers=data_header,
                    json={
                        "time": time_str( snapTime),
                        "imageId": 0
                        })
    print( f"Snap: {pic}")
# Get a picture
    getPic = requests.post(f'{url}/tasking/request_image' ,
                    headers=data_header,
                    json={
                        "time":   time_str( ftpTime),
                        "imageId": 0
                        })
    print( f"ftp download: {getPic}")

    camOff = requests.post(f'{url}/tasking/camera_power', 
                 headers=data_header,
                 json={ 'time':  time_str(camOffTime),
                        'on': False   } )
    print( f"camera off {camOff}")