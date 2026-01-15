
import serial
import time
import cv2
import logging
import threading, time, os
from flask import Flask, jsonify
import matplotlib.pyplot as plt
from controler import Controler


# Leer datos de Arduino cada cierto tiempo (pueto serie)
# Tomar N fotos del interior cada cierto tiempo también 
# guardarlo todo en una carpeta de logs
# Publicarlo mediant Flask

app = Flask(__name__)

controler = Controler()

threading.Thread(target=controler.loop(), daemon=True).start()


# --- API ENDPOINTS ---
@app.route('/api/ping')
def ping(): return jsonify({"status": "pong"})

@app.route('/api/sensors/summary')
def summary():
    # MODO FALLBACK: Si falla el cable, enviamos datos simulados para que la App no crashee
    if not controler or not controler.reals:
        return jsonify({
            "temperature": 24, "humidity": 65, "co2": 410,
            "actuators": {"heat": True, "humidifier": False, "fan": True},
            "status": "simulated_fallback"
        })
    
    # DATOS REALES
    data = controler.reals
    return jsonify({
        "temperature": int(data[0]),
        "humidity": int(data[1]),
        "CO2": int(data[2]),
        "actuators": {
            "heat": data[3] > 0, "humidifier": data[4] > 0, "fan": data[5] > 0
        }
    })

if __name__ == '__main__':
    # Importante: 0.0.0.0 permite acceso desde Tailscale
    app.run(host='0.0.0.0', port=5000)

