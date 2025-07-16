# Clase principal que itnegra camara sensores y logs
import os
import time
import logging
import serial
from datetime import datetime
from camHandler import CamHandler

class Controler():
    def __init__( self ) :
        self.cam = CamHandler() # trhead comienza el loop
        self.log_data = logging.getLogger("data")
        self.log_data.setLevel( logging.INFO )
        formater = logging.Formatter('%(message)s')
        dir = os.path.direname(__file__)
        self.filename = os.path.join(dir, r'../../logs')
        data_fh = logging.FileHandler(self.filename)
        data_fh.setFormatter(formater)
        self.log_data.addHandler( data_fh )

        self.ser = serial.Serial('/dev/ttyACM0', 115200, timeout = 1)

        self.tau = 5 # time between logs in secs
        self.start = time.time()


    def loop( self ):
        msg = "dht1_T, dht2_T, dht3_T, dht1_H, dht2_H, dht3_H, hm, tm, CO2, eCO2, TVOC, t_min, t_max, h_min, h_max, heat_rele, hum_rele, fan_rele"
        while True:
            if (time.time() - self.start) > self.tau:
                self.ser.write(b'GET_DATA\n')
                time.sleep(1)
                data = self.ser.readline().decode('utf-8').strip()
                if data:
                    print(f"Datos recibidos: {data}")
                    try:
                        values = [int(i) for i in data.split(',')]
                        if len(values) == 18:  # Verificar que se recibieron 18 valores
                            print("Datos correctamente formateados y completos.")
                            print("Valores:", values)
                            self.log_data.info(values)
                        else:
                            print(f"Error: Se esperaban 18 valores, pero se recibieron {len(values)}.")
                    except ValueError:
                        print("Error: Los datos recibidos no son válidos para convertir a enteros.")
                else:
                    print("No se recibió ninguna respuesta del Arduino.")
                self.start = time.time()



        
        