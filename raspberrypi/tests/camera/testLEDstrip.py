from gpiozero import LED
from time import sleep

# Configura el pin GPIO 17 para el LED
led = LED(27)

# Parpadeo del LED
while True:
    led.on()  # Enciende el LED
    print("LED encendido")
    sleep(1)  # Mantiene el LED encendido durante 1 segundo
    led.off()  # Apaga el LED
    print("LED apagado")
    sleep(1)  # Mantiene el LED apagado durante 1 segundo
