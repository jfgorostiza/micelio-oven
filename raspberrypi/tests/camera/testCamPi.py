import time
import picamera2 

# Inicializar la cámara
camera = picamera2.PICamera()

# Configurar una resolución de imagen (opcional)
camera.resolution = (1024, 768)

# Hacer una pausa para que la cámara se ajuste a la luz
time.sleep(2)

# Tomar una foto y guardarla como 'test_image.jpg'
camera.capture('test_image.jpg')

# Cerrar la cámara al finalizar
camera.close()

print("Imagen capturada!")
