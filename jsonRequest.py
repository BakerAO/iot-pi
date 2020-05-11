import json
import requests
import serial

port = serial.Serial(
  # '/dev/cu.SLAB_USBtoUART',
  '/dev/ttyS0',
  115200, 8, 'N', 1, 1
)

while True:
  rcv = port.readline()
  if len(rcv) > 1:
    rcv = str(rcv)
    rcvI = rcv.index('RCV=')
    comma = rcv.index(',')
    transmitterId = rcv[rcvI + 4 : comma]
    rcv = rcv[comma + 1 : ]

    comma = rcv.index(',')
    dataLength = rcv[ : comma]
    rcv = rcv[comma + 1 : ]

    comma = rcv.index(',')
    temperature = rcv[rcv.index('t-') + 2 : comma]
    rcv = rcv[comma + 1 : ]

    comma = rcv.index(',')
    humidity = rcv[rcv.index('h-') + 2 : comma]
    rcv = rcv[comma + 1 : ]

    comma = rcv.index(',')
    RSSI = rcv[ : comma]
    rcv = rcv[comma + 1 : ]

    SNR = rcv[ : -5]

    data = {
      'deviceId': transmitterId,
      'dataLength': dataLength,
      'temperature': temperature,
      'humidity': humidity,
      'RSSI': RSSI,
      'SNR': SNR
    }

    url = "https://api.innov8.host/devices/thermometers"
    # url = 'http://192.168.1.13/devices/thermometers'
    headers = {'content-type': 'application/json'}
    try:
      response = requests.post(url, headers=headers, data=json.dumps(data))
      print(response)
    except:
      print('Bad Connection')
