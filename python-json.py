import serial
import io
import json
import time

ser = serial.Serial('/dev/ttyACM0', 9600)
sendStr = json.dumps({'led': True, 'beep': True}, separators=(',',':'))
time.sleep(3)
ser.write(sendStr + "\n")
ser.write(sendStr + "\n")
ser.flushInput()
sendStr = json.dumps({'red': True, 'beep': True}, separators=(',',':'))
ser.write(sendStr + "\n")
time.sleep(2)
sendStr = json.dumps({'green': True, 'beep': True}, separators=(',',':'))
ser.write(sendStr + "\n")
while True:
    readStr = ser.readline()
    print(readStr)
    readJson = json.loads(readStr)
    print(readJson)
    if readJson['range'] < 25:
        sendStr = json.dumps({'led': True, 'beep': True}, separators=(',',':'))
        ser.write(sendStr + "\n")
    else:
        try:
		angle = int(readJson['range']) % 180
		sendStr = json.dumps({'angle': angle}, separators=(',',':'))
		ser.write(sendStr + "\n")
        except:
                pass
