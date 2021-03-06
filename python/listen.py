import paho.mqtt.client as mqtt
import serial
import requests
import argparse

argParser = argparse.ArgumentParser()
argParser.add_argument('-dev', default=False, action='store_true')
args = argParser.parse_args()
broker = 'broker.innov8.host'
port = serial.Serial('/dev/ttyACM0', 115200)
api = 'https://api.innov8.host/devices'
if args.dev:
  api = 'http://10.0.0.142:8081/devices'
print(api)

# CONNACK received from the server.
def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe("1/water_flow")

# PUBLISH message is received from the server.
def on_message(client, userdata, msg):
  payload = msg.payload.decode('UTF-8')
  print(msg.topic)
  print(payload)
  port.write(payload.encode())

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker, 1883, 60)
client.loop_start()

while True:
  rcv = port.readline()
  if len(rcv) > 1:
    try:
      rcvString = rcv.decode('UTF-8')
      if rcvString.find('{') == 0:
        headers = {'content-type': 'application/json'}
        response = requests.post(api, headers=headers, data=rcvString)
        print(rcvString)
        print(response)
    except:
      print('Error')
