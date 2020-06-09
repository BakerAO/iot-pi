import paho.mqtt.client as mqtt
import serial
import requests

port = serial.Serial('/dev/ttyACM0', 115200)
# url = "https://api.innov8.host/devices"
url = "http://10.0.0.142:8081/devices"

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
    port.write('10004-OFF')

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("broker.innov8.host", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
# client.loop_forever()

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
  client.loop(0.5)
