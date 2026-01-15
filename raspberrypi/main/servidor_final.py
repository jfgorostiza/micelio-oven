import threading, time, os
from flask import Flask, jsonify
from controler import Controler

app = Flask(__name__)

# --- AUTO-DETECCION ARDUINO ---
arduino = None
# Intentamos conectar en puertos comunes
for port in ['/dev/ttyUSB0', '/dev/ttyACM0', '/dev/ttyUSB1']:
    try:
        print(f"Probando conexion en {port}...")
        arduino = Controler(port=port)
        print(f"EXITO: Arduino conectado en {port}")
        break
    except:
        pass

# --- HILO DE LECTURA DE FONDO ---
def loop_lectura():
    while True:
        if arduino:
            try:
                arduino.get_data()
            except:
                pass
        time.sleep(5)

# Protocolo Técnico de Despliegue - Micelio Admin (Comentado para evitar error de sintaxis)
# Iniciamos el hilo (daemon)
threading.Thread(target=loop_lectura, daemon=True).start()

# --- API ENDPOINTS ---
@app.route('/api/ping')
def ping(): return jsonify({"status": "pong"})

@app.route('/api/sensors/summary')
def summary():
    # MODO FALLBACK: Si falla el cable, enviamos datos simulados para que la App no crashee
    if not arduino or not arduino.reals:
        return jsonify({
            "temperature": 24, "humidity": 65, "co2": 410,
            "actuators": {"heat": True, "humidifier": False, "fan": True},
            "status": "simulated_fallback"
        })
    
    # DATOS REALES
    data = arduino.reals
    return jsonify({
        "temperature": int(data[0]),
        "humidity": int(data[3]),
        "CO2": int(data[13]),
        "actuators": {
            "heat": data[6] > 0, "humidifier": data[7] > 0, "fan": data[8] > 0
        }
    })

if __name__ == '__main__':
    # Importante: 0.0.0.0 permite acceso desde Tailscale
    app.run(host='0.0.0.0', port=5000)
