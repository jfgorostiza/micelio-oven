
import serial
import time
import cv2
import logging
import matplotlib.pyplot as plt
from controler import Controler


# Leer datos de Arduino cada cierto tiempo (pueto serie)
# Tomar N fotos del interior cada cierto tiempo también 
# guardarlo todo en una carpeta de logs
controler = Controler()
controler.loop()