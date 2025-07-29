import os
import threading
import numpy as np
#import cv2 as cv # de pronto solamente coge imagenes en negro
import math
import matplotlib.pyplot as plt
import time
#import RPi.GPIO as GPIO
from picamera2 import Picamera2 #sudo apt install python3-picamera2 libcamera-apps libcamera-dev
from datetime import datetime
from gpiozero import LED
from gpiozero import Servo
from gpiozero.pins.rpigpio import RPiGPIOFactory

from time import sleep

class CamHandler():
    file_name = datetime.now().strftime("%Y_%m_%d_%I_%M_%S") + ".jpg"
    def __init__( self ):
        # Configura el pin GPIO 17 para el LED
        self.led = LED(27)
        self.led.on()
        self.cap = Picamera2()
        self.cap.preview_configuration.main.size = (1280, 720)
        self.cap.preview_configuration.main.format = "RGB888"
        self.cap.configure("preview")
        self.cap.start()
#        if not self.cap.isOpened():
#            print("Error: No se pudo abrir la camara")
        self.start_time = time.time()
        self.tau = 5

        pan_pin = 17
        tilt_pin = 4
        myCorrection=0.45
        maxPW=(2.0+myCorrection)/1000
        minPW=(1.0-myCorrection)/1000 
        self.panServo = Servo(pan_pin, min_pulse_width=minPW, max_pulse_width=maxPW)
        self.tiltServo = Servo(tilt_pin)
        self.tiltServo.min()
        self.panServo.value = 0
        self.panLast = 0
        self.tiltLast = 0
#        self.tiltServo.value = 0
#        self.servoPositions = [[-0.5, 0.5],[0,0],[0.5,0.5]]
        self.servoPositions = [[-1, -0.5],[0,-1],[1, -0.5]]
        dir = os.path.dirname(__file__)
        self.img_dir = os.path.join(dir, r'../../../img')

        self.stop_thread = False
        self.control_thread = threading.Thread(target = self.controlLoop, args=(1,))

    def start( self ):
        self.control_thread.start()

    def stop( self ):
        self.stop_thread = True

    def controlLoop(self, name):
        while True:
            if (time.time() - self.start_time) > self.tau:
                for i, p in enumerate(self.servoPositions):
                    print("taking a picture: " + str(p))
                    self.moveServo("pan", value = p[0])
                    self.moveServo("tilt", value = p[1])
#                    sleep(1)
                    self.takePic( i )
                    self.start_time = time.time()

            if self.stop_thread == True:
                print("CamHandler::stop thread")
                self.end()
                break

    def moveServo( self, servoID, value, alpha = 0.95 ):
        valueSmoothed = 0
        valueOld = 0
        print("Moving servo " + servoID)
        if servoID == "pan" :
            servo = self.panServo
            valueSmoothed = value * (1-alpha) + self.panLast * alpha
        else:
            servo = self.tiltServo
            valueSmoothed = value * (1-alpha) + self.tiltLast * alpha
        valueOld = valueSmoothed
        print(valueSmoothed)
        servo.value = valueSmoothed
        sleep(0.01)
        while(abs(valueSmoothed - value) > (1-alpha) ):
            valueSmoothed = value * (1-alpha) + valueOld * alpha
            valueOld = valueSmoothed
            print(valueSmoothed)
            if valueSmoothed < -1: 
                print("<-1")
                valueSmoothed = -1
            if valueSmoothed > 1: 
                print(">1")
                valueSmoothed = 1
            servo.value = valueSmoothed
            sleep(0.01)
        if servoID == "pan" :
            self.panLast= valueSmoothed
        else:
            self.tiltLast= valueSmoothed
        print("last servo value = " + str(servo.value))
        print("last valueSmoothed value = " + str(valueSmoothed))
        servo.value = None


    def takePic(self, id):
        self.led.off()  # Enciende el LED
        time.sleep(1)
        file_name = self.img_dir
        file_name += r"/" + datetime.now().strftime("%Y_%m_%d_%I_%M_%S_") + str(id) + ".jpg"
        self.cap.capture_file(file_name)
        print("writing file: " + file_name)
        time.sleep(0.5)
        self.led.on()  # Apaga
    
    def end(self):
        self.cap.close()
        self.panServo.stop()                 # At the end of the program, stop the PWM
        self.tiltServo.stop()                 # At the end of the program, stop the PWM
   #     GPIO.cleanup()           # Resets the GPIO pins back to defaults