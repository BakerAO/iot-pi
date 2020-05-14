from flask import Flask
from flask_socketio import SocketIO, send

app = Flask(__name__)
app.config['SECRET_KEY'] = 'mysecret'
socket = SocketIO(app)

@app.route('/')
def home():
  return 'Home Page'

@socket.on('message')
def handleMessage(msg):
  print(msg)
  send(msg)

if __name__ == '__main__':
  socket.run(app, port=8082)
