# Clase principal que itnegra camara sensores y logs
import os
import time
import logging
import serial
import threading
from datetime import datetime
from camHandler import CamHandler
from time import sleep   # Imports sleep (aka wait or pause) into the program

class Controler():
    def __init__( self ) :
        self.cam = CamHandler()
#Configurar aqui cada cuanto se realizan las fotos
        self.cam.set_period_hours(0.167) 
        self.cam.start() # thread comienza el loop de tomar fotos
        self.log_data = logging.getLogger("data")
        self.log_data.setLevel( logging.INFO )

        formater = logging.Formatter('%(asctime)s,%(message)s')
        dir = os.path.dirname(__file__)
        self.filename = os.path.join(dir, r'../../logs')
        fn = datetime.now().strftime("/%Y_%m_%d_%I_%M_%S.log")
        self.filename += fn
        data_fh = logging.FileHandler(self.filename)
        data_fh.setFormatter(formater)
        self.log_data.addHandler( data_fh )

        self.ser = serial.Serial('/dev/ttyACM0', 115200, timeout = 1)
        self.start = time.time()
        self.tau = 5 # time between logs in secs
 
    def loop( self ):
        msg = "dht1_T, dht2_T, dht3_T, dht1_H, dht2_H, dht3_H, hm, tm, CO2, eCO2, TVOC, t_min, t_max, h_min, h_max, heat_rele, hum_rele, fan_rele"
        self.log_data.info(msg)
        while True:
            if (time.time() - self.start) > self.tau:
                self.ser.write(b'GET_DATA\n')
                time.sleep(1)
                data = self.ser.readline().decode('utf-8').strip()
                if data:
                    print(f"Datos recibidos: {data}")
                    self.log_data.info(data)
#                    try:
#                        values = [int(i) for i in data.split(',')]
#                        if len(values) == 18:  # Verificar que se recibieron 18 valores
#                            print("Datos correctamente formateados y completos.")
#                            print("Valores:", values)
#                        else:
#                            print(f"Error: Se esperaban 18 valores, pero se recibieron {len(values)}.")
#                    except ValueError:
#                        print("Error: Los datos recibidos no son válidos para convertir a enteros.")
                else:
                    print("No se recibió ninguna respuesta del Arduino.")
                self.start = time.time()

    def end( self ):
#        except KeyboardInterrupt:
#            print("Interrumpido por el usuario")

        # Clean up everything
        pan.stop()                 # At the end of the program, stop the PWM
        GPIO.cleanup()           # Resets the GPIO pins back to defaults

        
        