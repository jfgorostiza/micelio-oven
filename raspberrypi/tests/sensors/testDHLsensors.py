# Complete Project Details: https://RandomNerdTutorials.com/raspberry-pi-temperature-humidity-data-logger/

import adafruit_dht
import board
import time
import os

# Resultados Treal = 25º H = 38%:
#DHT22: Temp=25.1ºC, Temp=77.2ºF, Humidity=60.7%
#DHT11: Temp=26.0ºC, Temp=78.8ºF, Humidity=38.0%

# Resultados Treal = 28.2º H = 33%:
#DHT22: Temp=28.1ºC, Humidity=56.9%
#DHT11L: Temp=29.0ºC, Humidity=35.0%
#DHT11R: Temp=27.0ºC, Humidity=30.0%

# Sensor data pin is connected to GPIO 4
dht22 = adafruit_dht.DHT22(board.D4)   # acierta en la temperatura. En la humedad mide un 23% de más
# Uncomment for DHT11
dht11L = adafruit_dht.DHT11(board.D17)    # mide 1.5º de más. Acierta en la humedad.
dht11R = adafruit_dht.DHT11(board.D27)    # mide 1.5º de más. Acierta en la humedad.

# create a variable to control the while loop
running = True

# Check if the file exists before opening it in 'a' mode (append mode)
fileName = time.strftime('%H:%M:%S_%d:%m:%Y') + '.dat'
file = open(fileName, 'a')

file.write('time_stamp, dht22_t, dht22_h, dht11L_t, dht11L_h, dht11R_t, dht11L_h\n')

# loop forever
while running:
    try:
        # read the humidity and temperature
        dht22_t = dht22.temperature
        dht22_h = dht22.humidity

        dht11L_t = dht11L.temperature
        dht11L_h = dht11L.humidity

        dht11R_t = dht11R.temperature
        dht11R_h = dht11R.humidity

        #print readings on the shell
        print("DHT22: Temp={0:0.1f}ºC, Humidity={1:0.1f}%".format(dht22_t, dht22_h))
        print("DHT11L: Temp={0:0.1f}ºC, Humidity={1:0.1f}%".format(dht11L_t, dht11L_h))
        print("DHT11R: Temp={0:0.1f}ºC, Humidity={1:0.1f}%".format(dht11R_t, dht11R_h))
        print("___")

        # save time, date, temperature, and humidity in .txt file
        file.write(time.strftime('%H:%M:%S %d/%m/%Y') + ', {:.2f}, {:.2f}, {:.2f}, {:.2f}, {:.2f}, {:.2f}\n'.format(dht22_t, dht22_h, dht11L_t, dht11L_h, dht11R_t, dht11L_h))

        # log new readings every 10 seconds
        time.sleep(0.5)

    except RuntimeError as error:
        # Errors happen fairly often, DHT's are hard to read, just keep going
        print(error.args[0])
        time.sleep(2.0)
        continue

    except KeyboardInterrupt:
        print('Program stopped')
        running = False
        file.close()

    except Exception as e:
        print('An unexpected error occurred:', str(e))
        running = False
        file.close()
        dht22.exit()
        dht11L.exit()
        dht11R.exit()