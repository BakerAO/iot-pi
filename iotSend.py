import serial

port = serial.Serial('/dev/ttyACM0', 115200)
deviceId = 0
command = ''

def run():
  print 'Input Device ID'
  deviceId = raw_input()
  print 'Input Command'
  command = raw_input()
  port.write(deviceId + '-' + command)
  deviceId = 0

while True:
  if deviceId == 0:
    run()
