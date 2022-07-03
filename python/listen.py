from dotenv import dotenv_values, find_dotenv # pip3 install python-dotenv
import paho.mqtt.client as mqtt # pip3 install paho-mqtt
import serial # pip3 install pyserial
import requests
# import argparse

env = dotenv_values(find_dotenv())
clientId = env['CLIENT_ID']
broker = env['MQTT_BROKER']
mqttPort = env['MQTT_PORT']
user = env['MQTT_USER']
password = env['MQTT_PASSWORD']
api = 'http://api.tidoba.com/devices'

serialPort = serial.Serial('/dev/ttyACM0', 115200)

# argParser = argparse.ArgumentParser()
# argParser.add_argument('-dev', default=False, action='store_true')
# args = argParser.parse_args()
# if args.dev:
#   api = 'http://10.0.0.5:8081/devices'
#   broker = '10.0.0.5'
print(api)
print(broker)

# CONNACK received from the server.
def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe("1/water_flow")

# PUBLISH message is received from the server.
def on_message(client, userdata, msg):
  payload = msg.payload.decode('UTF-8')
  print(msg.topic)
  print(payload)
  serialPort.write(payload.encode())

client = mqtt.Client()
# client.username_pw_set(user, password)
client.on_connect = on_connect
client.on_message = on_message
client.connect(broker, mqttPort)
client.loop_start()

while True:
  rcv = serialPort.readline()
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
