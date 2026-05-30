#ifndef SENSORTACTIL_H
#define SENSORTACTIL_H

#include <vector>
#include <string>

using namespace std;

// 1. Estructura para almacenar la información del sensor
struct Captura {
    int id;
    vector<vector<float>> matrizOriginal;    // Guardará la matriz inicial de 16x16
    vector<vector<float>> matrizInterpolada; // Guardará el resultado final de 128x128
};

// 2. Firmas de las funciones obligatorias para la práctica

// Función para leer el archivo JSON y extraer las 50 capturas
vector<Captura> leerJSON(const string& rutaArchivo);

// Función para validar que las dimensiones sean exactamente 16x16
bool validarMatriz(const vector<vector<float>>& matriz);

// Función para realizar la interpolación bilineal manual (de 16x16 a 128x128)
vector<vector<float>> interpolarBilineal(const vector<vector<float>>& matrizOrigen);

// Función para convertir los datos a un formato JSON válido para la red
string prepararJSONParaEnvio(const Captura& captura);

// Función para enviar las matrices interpoladas al servidor Python usando HTTP POST
bool enviarHTTP(const string& datosJSON, const string& urlDestino);

#endif