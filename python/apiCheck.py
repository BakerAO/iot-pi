from dotenv import dotenv_values, find_dotenv # pip3 install python-dotenv
import requests
from time import sleep

env = dotenv_values(find_dotenv())
api = 'http://' + env['API'] + '/devices'

while True:
    sleep(3)
    try:
        headers = {'content-type': 'application/json'}
        response = requests.get(api)
        print(response)
    except:
      print('Error')
