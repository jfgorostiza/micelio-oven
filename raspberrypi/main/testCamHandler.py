import time

from camHandler import CamHandler

cam = CamHandler()
cam.start()
input("Pulse una tecla para finalizar...")
cam.stop()
