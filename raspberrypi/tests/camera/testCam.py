import cv2
import matplotlib.pyplot as plt

# Abrir la cámara (0 es el índice por defecto, cambia si tienes más cámaras conectadas)
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Error: No se pudo abrir la cámara")
else:
    while True:
        # Leer un frame de la cámara
        ret, frame = cap.read()

        if not ret:
            print("Error: No se pudo recibir el frame.")
            break

        # Mostrar el frame en una ventana
        cv2.imshow('Cámara USB', frame)
#        plt.imshow(frame)
#        plt.show()

        # Si presionas la tecla 'q', saldrá del bucle
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    # Liberar la cámara y cerrar las ventanas
    cap.release()
    cv2.destroyAllWindows()
