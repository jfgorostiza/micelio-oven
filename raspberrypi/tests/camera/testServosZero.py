from gpiozero import Servo
from time import sleep
from gpiozero.pins.rpigpio import RPiGPIOFactory

# Configurar el pin GPIO que utilizarás (en este caso GPIO 17)
#pin = 4 # tilt
pin = 17 # pan

# Crear un objeto Servo en el pin GPIO 17
servo = Servo(pin)
lastValue = 0

def testMinMax():
    # Mover el servo a varias posiciones
    try:
        while True:
            print("Mover a -90 grados")
            servo.min()  # Mueve el servo a la posición mínima (-90 grados)
            sleep(2)

            print("Mover a 0 grados")
            servo.value = 0  # Mueve el servo a la posición central (0 grados)
            sleep(2)

            print("Mover a 90 grados")
            servo.max()  # Mueve el servo a la posición máxima (90 grados)
            sleep(2)

    except KeyboardInterrupt:
        print("Interrumpido por el usuario")

def moveServo( value, alpha = 0.95 ):
    valueSmoothed = 0
    valueOld = 0
    global lastValue
    valueSmoothed = value * (1-alpha) + lastValue * alpha
    valueOld = valueSmoothed
    print(valueSmoothed)
    servo.value = valueSmoothed
    sleep(0.01)
    while(abs(valueSmoothed - value) > (1-alpha) ):
#        value = value * 100
        valueSmoothed = value * (1-alpha) + valueOld * alpha
        valueOld = valueSmoothed
        print(valueSmoothed)
        servo.value = valueSmoothed
        sleep(0.01)
    lastValue = servo.value
    servo.value = None

def moveSmooth():
    # Mover el servo a varias posiciones
    try:
        while True:
            print("Mover a -90 grados")
            moveServo(-1)  # Mueve el servo a la posición mínima (-90 grados)
            sleep(2)

            print("Mover a 0 grados")
            moveServo(0)  # Mueve el servo a la posición central (0 grados)
            sleep(2)

            print("Mover a 90 grados")
            moveServo(1)  # Mueve el servo a la posición máxima (90 grados)
            sleep(2)

    except KeyboardInterrupt:
        print("Interrumpido por el usuario")


moveSmooth()
