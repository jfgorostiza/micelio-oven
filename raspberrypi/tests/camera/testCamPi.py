import time
from picamera2 import Picamera2

# Inicializar la cámara
camera = Picamera2()

# Configurar una resolución de imagen (opcional)
camera.preview_configuration.main.size = (1280, 720)
#camera.preview_configuration.main.format = "RGB888"
camera.configure("preview")
camera.start()
# Hacer una pausa para que la cámara se ajuste a la luz
time.sleep(2)

# Tomar una foto y guardarla como 'test_image.jpg'
camera.capture_file('test_image.jpg')

# Cerrar la cámara al finalizar
camera.close()

print("Imagen capturada!")
