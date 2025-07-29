from picamera2 import PiCamera2
import os
from time import sleep

cam = PiCamera2()
cam.resolution = (1024, 768)
cam.start_preview()
sleep(2)
cam.capture("test.jpg")
cam.stop_preview()