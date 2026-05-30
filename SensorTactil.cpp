#include "SensorTactil.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <curl/curl.h>
#include "nlohmann/json.hpp" 

using namespace std;
using json = nlohmann::json;

// 1. Función para leer el JSON
vector<Captura> leerJSON(const string& rutaArchivo) {
    vector<Captura> listaCapturas;
    ifstream archivo(rutaArchivo);

    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo " << rutaArchivo << endl;
        return listaCapturas;
    }

    json datosJson;
    archivo >> datosJson; 

    for (const auto& item : datosJson["captures"]) {
        Captura nuevaCaptura;
        nuevaCaptura.id = item["id"];
        nuevaCaptura.matrizOriginal = item["matrix"].get<vector<vector<float>>>();
        listaCapturas.push_back(nuevaCaptura);
    }

    archivo.close();
    return listaCapturas;
}

// 2. Función para validar las dimensiones
bool validarMatriz(const vector<vector<float>>& matriz) {
    if (matriz.size() != 16) return false; 

    for (int i = 0; i < 16; i++) {
        if (matriz[i].size() != 16) return false;
    }
    return true; 
}

// 3. Función para realizar la interpolación bilineal manual (16x16 a 128x128)
vector<vector<float>> interpolarBilineal(const vector<vector<float>>& matrizOrigen) {
    int altoOrigen = 16, anchoOrigen = 16;
    int altoDestino = 128, anchoDestino = 128; 

    vector<vector<float>> matrizDestino(altoDestino, vector<float>(anchoDestino, 0.0f));

    float ratioX = (float)(anchoOrigen - 1) / (anchoDestino - 1);
    float ratioY = (float)(altoOrigen - 1) / (altoDestino - 1);

    for (int i = 0; i < altoDestino; i++) {
        for (int j = 0; j < anchoDestino; j++) {
            float x = j * ratioX;
            float y = i * ratioY;

            int x1 = floor(x);
            int y1 = floor(y);
            int x2 = min(x1 + 1, anchoOrigen - 1);
            int y2 = min(y1 + 1, altoOrigen - 1);

            float dx = x - x1;
            float dy = y - y1;

            float Q11 = matrizOrigen[y1][x1];
            float Q21 = matrizOrigen[y1][x2];
            float Q12 = matrizOrigen[y2][x1];
            float Q22 = matrizOrigen[y2][x2];

            float R1 = Q11 * (1.0f - dx) + Q21 * dx;
            float R2 = Q12 * (1.0f - dx) + Q22 * dx;

            matrizDestino[i][j] = R1 * (1.0f - dy) + R2 * dy;
        }
    }
    return matrizDestino;
}

// 4. Función para convertir los datos a un formato JSON válido para la red
string prepararJSONParaEnvio(const Captura& captura) {
    json j;
    j["capture_id"] = captura.id;
    j["width"] = 128;
    j["height"] = 128;
    j["data"] = captura.matrizInterpolada;
    
    return j.dump(); 
}

// 5. Función auxiliar para que libcurl no imprima basura en la terminal
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}

// 6. Función para enviar las matrices usando HTTP POST mediante libcurl
bool enviarHTTP(const string& datosJSON, const string& urlDestino) {
    CURL* curl;
    CURLcode res;
    bool exito = false;

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, urlDestino.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, datosJSON.c_str());
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        res = curl_easy_perform(curl);
        
        if (res == CURLE_OK) {
            exito = true;
        } else {
            cerr << "Error en curl: " << curl_easy_strerror(res) << endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return exito;
}