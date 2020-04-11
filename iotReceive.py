import serial
import requests

port = serial.Serial('/dev/ttyACM0', 115200)

while True:
  rcv = port.readline()
  if len(rcv) > 1:
    rcvString = str(rcv)
    url = "https://api.innov8.host/devices/thermometers"
    headers = {'content-type': 'application/json'}

    try:
      response = requests.post(url, headers=headers, data=rcvString)
      print(response)
    except:
      print('Bad Connection')
