import serial
import time

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)
ser.flush()
time.sleep(2)

def getData() :
    msg = "GET_DATA\0"
#    msg = "G\0"
#    ser.write(b'GET_DATA\n')
    ser.write(msg.encode(encoding = 'ascii'))
    rta = ser.readline().decode('utf-8').rstrip()
    return rta

def sendData( data ):
    cmd = f'SEND_DATA:{data}\0'
    ser.write( cmd.encode(encoding = 'ascii') )

def doAction( action ):
  cmd = f'DO_ACTION:{action}\0'
  ser.write( cmd.encode(encoding = 'ascii') )
  rta = ser.readline().decode('utf-8').rstrip()
  return rta

def close():
   ser.close()

if __name__ == "__main__":
    print("solicitar datos")
    data = getData()
    print(f"Data: {data}")

    time.sleep(1)

    print("Send data to Parse")
    sendData("4")

    time.sleep(10)
    print("Do action")
    rta = doAction("PRINT")
    print(f"rta: {rta}")

    close()