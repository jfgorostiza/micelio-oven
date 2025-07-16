# Set up libraries and overall settings
import RPi.GPIO as GPIO  # Imports the standard Raspberry Pi GPIO library
from time import sleep   # Imports sleep (aka wait or pause) into the program

pan_pin = 7
tilt_pin = 11

GPIO.setmode(GPIO.BOARD) # Sets the pin numbering system to use the physical layout

GPIO.setup(pan_pin,GPIO.OUT)  # Sets up pin 11 to an output (instead of an input)
GPIO.setup(tilt_pin,GPIO.OUT)  # Sets up pin 11 to an output (instead of an input)
pan = GPIO.PWM(pan_pin, 50)     # Sets up pin 11 as a PWM pin
tilt = GPIO.PWM(tilt_pin, 50)     # Sets up pin 11 as a PWM pin
pan.start(0)               # Starts running PWM on the pin and sets it to 0
tilt.start(0)               # Starts running PWM on the pin and sets it to 0

# Move the servo back and forth
# Mover el servo a varias posiciones
def move_pan():
        pan.ChangeDutyCycle(6)     # Changes the pulse width to 3 (so moves the servo)
        sleep(3)                 # Wait 1 second
        pan.ChangeDutyCycle(10)     # Changes the pulse width to 3 (so moves the servo)
        sleep(3)                 # Wait 1 second
        pan.ChangeDutyCycle(12)    # Changes the pulse width to 12 (so moves the servo)
        sleep(3)

def move_tilt():
        tilt.ChangeDutyCycle(3)     # Changes the pulse width to 3 (so moves the servo)
        sleep(3)                 # Wait 1 second
        tilt.ChangeDutyCycle(6)     # Changes the pulse width to 3 (so moves the servo)
        sleep(3)                 # Wait 1 second

def move_both():
        pan.ChangeDutyCycle(0)     # Changes the pulse width to 3 (so moves the servo)
        tilt.ChangeDutyCycle(6)     # Changes the pulse width to 3 (so moves the servo)
        sleep(3)                 # Wait 1 second
        pan.ChangeDutyCycle(10)     # Changes the pulse width to 3 (so moves the servo)
        tilt.ChangeDutyCycle(3)     # Changes the pulse width to 3 (so moves the servo)
        sleep(3)                
        pan.ChangeDutyCycle(12)    # Changes the pulse width to 12 (so moves the servo)
        tilt.ChangeDutyCycle(6)     # Changes the pulse width to 3 (so moves the servo)
        sleep(3)
      
try:
    while True:
          move_both()

except KeyboardInterrupt:
    print("Interrumpido por el usuario")

    # Clean up everything
    pan.stop()                 # At the end of the program, stop the PWM
    GPIO.cleanup()           # Resets the GPIO pins back to defaults