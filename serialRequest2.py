import serial
# import requests
# import json

port = serial.Serial('/dev/ttyACM0', 115200)

while True:
  rcv = port.readline()
  if len(rcv) > 1:
    rcv = str(rcv)
    print(rcv)
