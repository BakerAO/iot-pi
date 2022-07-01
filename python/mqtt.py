from dotenv import dotenv_values, find_dotenv # pip3 install python-dotenv
import paho.mqtt.client as mqtt # pip3 install paho-mqtt

env = dotenv_values(find_dotenv())
broker = env['MQTT_BROKER']
port = int(env['MQTT_PORT'])
user = env['MQTT_USER']
password = env['MQTT_PASSWORD']

# CONNACK received from the server.
def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe("topic1")
  client.publish("topic1", "my message")

# PUBLISH message is received from the server.
def on_message(client, userdata, msg):
  payload = msg.payload.decode('UTF-8')
  print(msg.topic)
  print(payload)

mqttClient = mqtt.Client()
# mqttClient.username_pw_set(user, password)
mqttClient.on_connect = on_connect
mqttClient.on_message = on_message
mqttClient.connect(broker, port)
mqttClient.loop_forever()
