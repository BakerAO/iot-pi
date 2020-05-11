import sys
message = str(sys.argv[1]) + '-' + str(sys.argv[2])

import serial
port = serial.Serial('/dev/ttyACM0', 115200)
port.write(message)
