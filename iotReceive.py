import serial
import requests

port = serial.Serial('/dev/ttyACM0', 115200)
# url = "https://api.innov8.host/devices"
url = "http://10.0.0.138:8081/devices"

while True:
  rcv = port.readline()
  if len(rcv) > 1:
    rcvString = str(rcv)
    url = url
    headers = {'content-type': 'application/json'}

    try:
      print(rcvString)
      response = requests.post(url, headers=headers, data=rcvString)
      print(response)
    except:
      print('Bad Connection')
