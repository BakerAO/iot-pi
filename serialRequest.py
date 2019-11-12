import serial
# import requests

port = serial.Serial(
  # '/dev/cu.SLAB_USBtoUART',
  '/dev/ttyUSB0',
  115200, 8, 'N', 1, 1
)

while True:
  rcv = port.readline()
  if len(rcv) > 1:
    # url = "https://api.github.com/events"
    # response = requests.get(url)
    # print(response.status_code)
    # print(response.text)
    # print(response.json())
    print(rcv)
