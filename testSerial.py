import serial
import requests
import json

port = serial.Serial('/dev/ttyACM0', 115200)

while True:
  rcv = port.readline()
  if len(rcv) > 1:
    rcv = str(rcv)

    deviceIdIndex = rcv.index('deviceId-')
    comma = rcv.index(',')
    transmitterId = rcv[deviceIdIndex + 9 : comma]
    rcv = rcv[comma + 1 : ]

    temperatureIndex = rcv.index('temperature-')
    if temperatureIndex > -1:
      comma = rcv.index(',')
      temperature = rcv[rcv.index('temperature-') + 12 : comma]
      rcv = rcv[comma + 1 : ]

    comma = rcv.index(',')
    humidity = rcv[rcv.index('humidity-') + 9 : comma]
    rcv = rcv[comma + 1 : ]

    comma = rcv.index(',')
    RSSI = rcv[ : comma]
    rcv = rcv[comma + 1 : ]

    SNR = rcv[ : -5]

    data = {
      'deviceId': transmitterId,
      'temperature': temperature,
      'humidity': humidity,
    }

    url = "https://api.innov8.host/devices/thermometers"
    headers = {'content-type': 'application/json'}
    try:
      response = requests.post(url, headers=headers, data=json.dumps(data))
      print(response)
    except:
      print('Bad Connection')
