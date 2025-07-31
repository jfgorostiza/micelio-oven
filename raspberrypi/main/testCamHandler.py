import time

from camHandler import CamHandler

cam = CamHandler()
cam.set_period_hours(0.25) 
cam.start()
input("Pulse una tecla para finalizar...")
cam.stop()
