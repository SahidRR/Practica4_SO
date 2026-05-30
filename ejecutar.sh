#!/bin/bash

# Definición de variables
CLIENTE_BIN="cliente_tactil"
SERVIDOR_SCRIPT="ServidorPython.py"

echo "========================================================="
echo " AUTOMATIZACIÓN DE PROCESOS - PRÁCTICA 4 (S. OPERATIVOS) "
echo "========================================================="

# 1. Compilación del binario C++
echo "[INFO] Iniciando compilación del cliente C++ (libcurl)..."
g++ main.cpp SensorTactil.cpp -o $CLIENTE_BIN -lcurl

# Verificación de estado de compilación
if [ $? -ne 0 ]; then
    echo "[ERROR] Fallo en la compilación. Abortando el script."
    exit 1
fi
echo "[SUCCESS] Compilación finalizada. Binario generado: ./$CLIENTE_BIN"
echo "---------------------------------------------------------"

# 2. Despliegue del servidor en segundo plano
echo "[INFO] Desplegando Servidor Python en segundo plano..."
python3 $SERVIDOR_SCRIPT &
PID_SERVER=$!

# Pausa para garantizar la apertura del puerto TCP
echo "[INFO] Esperando inicialización del socket (puerto 5000)..."
sleep 3
echo "---------------------------------------------------------"

# 3. Ejecución del cliente C++
echo "[INFO] Iniciando proceso cliente para transmisión de datos..."
./$CLIENTE_BIN
ESTADO_CLIENTE=$?
echo "---------------------------------------------------------"

# 4. Limpieza y liberación de recursos
echo "[INFO] Enviando señal de terminación (SIGTERM) al servidor (PID: $PID_SERVER)..."
kill $PID_SERVER 2>/dev/null

echo "========================================================="
if [ $ESTADO_CLIENTE -eq 0 ]; then
    echo "[SUCCESS] Ejecución del sistema finalizada correctamente."
    echo "[INFO] Resultados almacenados en el directorio local 'imagenes_tactiles/'."
else
    echo "[WARNING] Ejecución finalizada. El proceso cliente reportó un código de error."
fi
echo "========================================================="