import serial
import time

# Configurar el puerto serial (ajustar el nombre del puerto según corresponda)
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

while True:

    # Enviar solicitud de datos
    ser.write(b'GET_DATA\n')  # El comando para solicitar los datos

    # Esperar para asegurar que Arduino tenga tiempo de procesar
    time.sleep(1)

    # Leer los datos enviados por el Arduino
    data = ser.readline().decode('utf-8').strip()

    # Comprobar si se ha recibido algo
    if data:
        print(f"Datos recibidos: {data}")
        # Intentar convertir los datos a una lista de enteros
        try:
            values = [int(i) for i in data.split(',')]
            if len(values) == 18:  # Verificar que se recibieron 18 valores
                print("Datos correctamente formateados y completos.")
                print("Valores:", values)

        # Grabar los datos en un archivo CSV
                with open('datos.csv', mode='a', newline='') as file:  # Abrir archivo en modo 'append'
                    # Crear un escritor de CSV
                    import csv
                    writer = csv.writer(file)

                    # Escribir los valores en una fila CSV
                    writer.writerow(values)
                    print("Datos grabados correctamente en 'datos.csv'.")

            else:
                print(f"Error: Se esperaban 18 valores, pero se recibieron {len(values)}.")
        except ValueError:
            print("Error: Los datos recibidos no son válidos para convertir a enteros.")
    else:
        print("No se recibió ninguna respuesta del Arduino.")

    # Cerrar el puerto serial
ser.close()

