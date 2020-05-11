import sys
message = sys.argv[1] + '-' + sys.argv[2]

import serial
port = serial.Serial('/dev/ttyACM0', 115200)
port.write(message)
