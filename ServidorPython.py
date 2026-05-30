from flask import Flask, request, jsonify
import numpy as np
import matplotlib
matplotlib.use('Agg') # Clave para Mac
import matplotlib.pyplot as plt
import os
import glob # Nueva librería para buscar archivos viejos

app = Flask(__name__)

# 1. Definimos el nombre de la carpeta
CARPETA_RESULTADOS = "imagenes_tactiles"

# 2. Lógica de inicio: Crear carpeta o borrar imágenes viejas
if not os.path.exists(CARPETA_RESULTADOS):
    os.makedirs(CARPETA_RESULTADOS)
    print(f"Carpeta '{CARPETA_RESULTADOS}' creada.")
else:
    print(f"Limpiando imágenes antiguas en '{CARPETA_RESULTADOS}'...")
    archivos_viejos = glob.glob(os.path.join(CARPETA_RESULTADOS, "*.png"))
    for archivo in archivos_viejos:
        os.remove(archivo)
    print("✨ Carpeta limpia y lista.")

@app.route('/procesar_matriz', methods=['POST'])
def procesar_matriz():
    try:
        # Recibir los datos JSON
        datos = request.get_json()
        
        capture_id = datos.get("capture_id")
        data_matrix = datos.get("data")
        
        if capture_id is None or data_matrix is None:
            return jsonify({"error": "Faltan datos en el JSON"}), 400
            
        print(f"Recibiendo captura ID: {capture_id}...")

        # Reconstruir la matriz con NumPy
        matriz_np = np.array(data_matrix)

        # Generar la imagen táctil
        plt.figure(figsize=(6, 6))
        plt.imshow(matriz_np, cmap="inferno", interpolation='nearest') 
        plt.colorbar(label="Presion")
        plt.title(f"Mapa tactil - Captura {capture_id}")
        
        # 3. Guardar la imagen DENTRO de la carpeta
        nombre_archivo = f"capture_{capture_id}.png"
        ruta_completa = os.path.join(CARPETA_RESULTADOS, nombre_archivo)
        
        plt.savefig(ruta_completa)
        plt.close() # Cierra la figura
        
        print(f"Imagen {nombre_archivo} guardada con éxito.")
        
        return jsonify({"status": "ok", "message": f"Captura {capture_id} procesada"}), 200

    except Exception as e:
        print(f"Error procesando la solicitud: {e}")
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    print("=== Servidor Python Iniciado ===")
    print("Esperando matrices en el puerto 5000...")
    app.run(host='127.0.0.1', port=5000)