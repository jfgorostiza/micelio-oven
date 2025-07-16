import os
import threading
import numpy as np
import cv2 as cv
import math
import matplotlib.pyplot as plt
import time
#import RPi.GPIO as GPIO
from datetime import datetime
from gpiozero import LED
from gpiozero import Servo
from gpiozero.pins.rpigpio import RPiGPIOFactory

from time import sleep

class CamHandler():
    tau = 5
    file_name = datetime.now().strftime("%Y_%m_%d_%I_%M_%S") + ".jpg"
    def __init__( self ):
        # Configura el pin GPIO 17 para el LED
        self.led = LED(27)
        self.led.on()
#        self.cap = cv.VideoCapture(0)
#        if not self.cap.isOpened():
#            print("Error: No se pudo abrir la camara")
        self.start = time.time()
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
        dir = os.path.direname(__file__)
        self.img_dir = os.path.join(dir, r'../../img')

        self.stop_thread = False
        self.control_thread = threading.Thread(target = self.controlLoop, args=(1,))
        self.control_thread.start()

    def controlLoop(self, name):
        while True:
            if (time.time() - self.start) > self.tau:
                for i, p in enumerate(self.servoPositions):
                    print("taking a picture: " + str(p))
                    self.moveServo("pan", value = p[0])
                    self.moveServo("tilt", value = p[1])
#                    sleep(1)
                    self.takePic( i )
                    self.start = time.time()

            if self.stop_thread == True:
                print("CamHandler::stop thread")
                self.end()
                break

    def moveServo( self, servoID, value, alpha = 0.95 ):
        valueSmoothed = 0
        valueOld = 0
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
    #        value = value * 100
            valueSmoothed = value * (1-alpha) + valueOld * alpha
            valueOld = valueSmoothed
    #        print(valueSmoothed)
            servo.value = valueSmoothed
            sleep(0.01)
        if servoID == "pan" :
            self.panLast= servo.value
        else:
            self.tiltLast= servo.value
        servo.value = None


    def takePic(self, id):
        self.led.off()  # Enciende el LED
        time.sleep(0.5)
        cap = cv.VideoCapture(0)
        if not cap.isOpened():
            print("Error: No se pudo abrir la camara")

        ret,frame = cap.read()
        if not ret:
            print("Error: no se pudo abrir el frame")
#        cv.destroyAllWindows()
#        plt.imshow(frame)
#        plt.show()
#        cv.imshow('cam', frame) # doesnt work, not show any image
        file_name = self.img_dir
        file_name += r"/" + datetime.now().strftime("%Y_%m_%d_%I_%M_%S_") + str(id) + ".jpg"
        cv.imwrite(file_name,frame)
        print("writing file: " + file_name)
        cap.release()
        time.sleep(0.5)
        self.led.on()  # Apaga
#        cv.waitKey(1)
    
    def end(self):
        self.cap.release()
        cv.destroyAllWindows()
        self.pan.stop()                 # At the end of the program, stop the PWM
        self.tilt.stop()                 # At the end of the program, stop the PWM
   #     GPIO.cleanup()           # Resets the GPIO pins back to defaults