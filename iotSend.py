import serial

port = serial.Serial('/dev/ttyACM0', 115200)
message = ''

def run():
  print 'Input Device ID and Command'
  message = raw_input()
  port.write(message)
  message = ''

while True:
  if message == '':
    run()
