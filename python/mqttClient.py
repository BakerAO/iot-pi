import paho.mqtt.client as mqtt

# client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("1/water_flow/10004")

# PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(1)
    print(msg.topic)
    print(2)
    print(msg.payload)
    print(3)
    print(str(msg.payload))
    print(4)
    print(msg.payload.decode('UTF-8'))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("broker.innov8.host", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
