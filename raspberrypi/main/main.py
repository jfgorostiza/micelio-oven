
import serial
import time
import cv2
import matplotlib.pyplot as plt
from camHandler import camHandler

from gpiozero import Servo
from time import sleep
from gpiozero.pins.rpigpio import RPiGPIOFactory

# Leer datos de Arduino cada cierto tiempo (pueto serie)
# Tomar N fotos del interior cada cierto tiempo también 
# guardarlo todo en una carpeta de logs
cam = CamHandler()
