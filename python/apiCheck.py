from dotenv import dotenv_values, find_dotenv # pip3 install python-dotenv
import requests
from time import sleep

env = dotenv_values(find_dotenv())
api = 'http://' + env['API']

try:
    print(api)
    response = requests.get(api)
    print(response)
except:
    print('Error')
