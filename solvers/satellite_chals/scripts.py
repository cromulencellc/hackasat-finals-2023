import requests



def upload( url , apikey  , id , script ):
    data_header = {'accept': 'application/json', 
           "x-api-key": apikey
           }
    with open(script, 'rb') as f:
        files = {'file': ('script', open(script, 'rb'))}

        uploadReq = requests.post(f'{url}/tasking/upload_script?script_id={id}', 
                headers=data_header,
                files=files)
        print( f"load script: {uploadReq}")
def run( url , apikey , id, timestr ):
    data_header = {'accept': 'application/json', 
               "x-api-key": apikey,
               "Content-Type": "application/json"}
    runScript = requests.post(f'{url}/tasking/run_script', 
                 headers=data_header,
                 json={ 'time': timestr,
                        'id': id   } )
    print( f"run script: {runScript}")