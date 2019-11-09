import serial

port = serial.Serial(
  # '/dev/cu.SLAB_USBtoUART',
  '/dev/ttyUSB0',
  115200, 8, 'N', 1, 1
)

while True:
  rcv = port.readline()
  if len(rcv) > 1:
    print(rcv)
