import paho.mqtt.client as mqtt
import serial
import requests

broker = 'broker.innov8.host'
port = serial.Serial('/dev/ttyACM0', 115200)
api = 'https://api.innov8.host/devices'
# api = 'http://10.0.0.142:8081/devices'

# CONNACK received from the server.
def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe("1/water_flow/10004")

# PUBLISH message is received from the server.
def on_message(client, userdata, msg):
  payload = msg.payload.decode('UTF-8')
  print(msg.topic)
  print(payload)
  if payload == 'shut_off':
    serialMsg = '10004-SHUT_OFF'
    port.write(serialMsg.encode())
  if payload == 'open':
    serialMsg = '10004-OPEN'
    port.write(serialMsg.encode())

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker, 1883, 60)

while True:
  client.loop()
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
