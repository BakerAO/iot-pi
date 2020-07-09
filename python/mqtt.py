import paho.mqtt.client as mqtt
import serial

broker = 'broker.innov8.host'
port = serial.Serial('/dev/ttyACM1', 115200)

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
client.loop_forever()
