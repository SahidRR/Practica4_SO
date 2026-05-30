#include <iostream>
#include <vector>
#include <string>
#include "SensorTactil.h"

using namespace std;

int main() {
    cout << "=== Sistema de Procesamiento Tactil Iniciado ===" << endl;

    // 1. Leer el archivo JSON original
    string ruta = "tactile_captures_50.json";
    vector<Captura> capturas = leerJSON(ruta);

    // Si hubo un error leyendo o el archivo está vacío, detenemos el programa
    if (capturas.empty()) {
        cout << "Error critico: No se encontraron capturas. Abortando..." << endl;
        return 1;
    }

    cout << "Se cargaron " << capturas.size() << " capturas con exito." << endl;
    cout << "------------------------------------------------" << endl;

    // 2. Procesar cada captura individualmente
    for (int i = 0; i < capturas.size(); i++) {
        cout << "Procesando captura ID: " << capturas[i].id << "..." << endl;

        // Paso A: Validar dimensiones (16x16)
        if (!validarMatriz(capturas[i].matrizOriginal)) {
            cout << "  [X] Error: Dimensiones invalidas. Saltando captura." << endl;
            continue; // Salta a la siguiente iteración del bucle
        }
        cout << "  [OK] Matriz 16x16 validada." << endl;

        // Paso B: Interpolar bilinealmente a 128x128
        capturas[i].matrizInterpolada = interpolarBilineal(capturas[i].matrizOriginal);
        cout << "  [OK] Interpolacion 128x128 completada." << endl;

        // Paso C: Convertir a texto JSON y enviar por HTTP
        string datosListos = prepararJSONParaEnvio(capturas[i]);
        
        // Asumimos que el servidor Python (Flask) correrá en tu propia máquina en el puerto 5000
        string urlServidor = "http://127.0.0.1:5000/procesar_matriz"; 
        
        bool exitoEnvio = enviarHTTP(datosListos, urlServidor);
        if (exitoEnvio) {
            cout << "  [OK] Enviado correctamente al servidor Python." << endl;
        } else {
            cout << "  [X] Error de conexion con el servidor." << endl;
        }
    }

    cout << "------------------------------------------------" << endl;
    cout << "=== Procesamiento finalizado ===" << endl;

    return 0;
}