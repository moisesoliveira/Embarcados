import serial
ser = serial.Serial('/dev/ttyACM0',timeout = 0.01)
UARTcontrol = ' '

while True:
    print ser.read(23)
    ser.flush()
