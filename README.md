# Descripción del Proyecto
Este repositorio contiene la implementación de un sistema distribuido basado en una arquitectura cliente-servidor para el procesamiento y la interpolación de imágenes táctiles provenientes de un gripper robótico. El proyecto simula un entorno industrial real donde los datos de baja resolución capturados por un sensor físico son escalados y visualizados de forma remota.
El flujo de trabajo consiste en un cliente desarrollado en C++ que extrae las capturas de un archivo JSON, verifica su integridad geométrica y aplica un algoritmo de interpolación bilineal manual para aumentar la resolución espacial de 16x16 a 128x128 píxeles. Posteriormente, estos datos se empaquetan y se transmiten mediante peticiones HTTP POST a un servidor en Python. Este último actúa como un procesador visual encargado de reconstruir la matriz numérica y generar de manera automática mapas de calor (heatmaps) para el análisis de la presión táctil.
# Estructura de Archivos
## main.cpp: 
Código principal del cliente en C++. Actúa como el centro de control que orquesta el bucle de ejecución: lectura de datos, validación geométrica, llamada a la función de interpolación y gestión del envío de las tramas de red.
SensorTactil.h / SensorTactil.cpp: Archivos que definen la programación modular del cliente. Contienen las estructuras de datos y la lógica matemática para la lectura del JSON, la interpolación bilineal manual y la comunicación de red utilizando la librería libcurl.
## ServidorPython.py: 
Servidor desarrollado con Flask (Python). Recibe los datos JSON mediante HTTP, los transforma utilizando NumPy y genera los mapas de presión con Matplotlib, gestionando también la creación y limpieza automática del directorio de resultados.
## ejecutar.sh: 
Script de automatización en Bash diseñado para compilar el código fuente en C++, desplegar el servidor Python en segundo plano de manera sincronizada, ejecutar el cliente y liberar los recursos del sistema al finalizar.
tactile_captures_50.json: Archivo base de datos que contiene las 50 capturas de presión iniciales generadas por el sensor del gripper robótico.
## imagenes_tactiles/:
Directorio generado dinámicamente por el servidor Python durante la ejecución, destinado a almacenar los archivos de imagen .png resultantes.
# Requisitos Previos (Prerrequisitos)
Para compilar y ejecutar este proyecto correctamente en un entorno UNIX (macOS/Linux), asegúrate de contar con los siguientes componentes instalados:
Compilador g++ (compatible con C++11 o superior).
Librería de desarrollo libcurl instalada en el sistema.
Librería nlohmann/json para C++ (incluida localmente en el directorio del proyecto).
Intérprete de Python 3.
Librerías de Python requeridas para el servidor (instalables mediante el comando: pip3 install Flask numpy matplotlib).
Instrucciones de Ejecución
## Opción 1: Ejecución Automatizada (Recomendada)
Para evitar problemas de sincronización entre el cliente y el servidor, así como para facilitar la gestión de procesos, se proporciona un script en Bash que automatiza todo el flujo de trabajo.
Abre una terminal y navega hasta el directorio del proyecto.
Otorga permisos de ejecución al script:
chmod +x ejecutar.sh
Ejecuta el script:
./ejecutar.sh
El script se encargará automáticamente de compilar el cliente, iniciar el servidor Python en segundo plano, esperar la apertura del puerto de red, ejecutar el binario C++ y finalmente enviar una señal de terminación al servidor para liberar el puerto 5000.
## Opción 2: Ejecución Manual
Si deseas observar el flujo de datos paso a paso, puedes ejecutar los componentes de manera independiente siguiendo este orden estricto:
Compila el cliente C++ enlazando la librería de red:
g++ main.cpp SensorTactil.cpp -o cliente_tactil -lcurl
En una terminal, inicia el servidor de visualización:
python3 ServidorPython.py
Abre una nueva terminal o pestaña y ejecuta el binario cliente:
./cliente_tactil
Una vez finalizado el procesamiento de las 50 capturas, regrese a la terminal del servidor Python y finalice el proceso manualmente utilizando la combinación de teclas Ctrl + C.
## Salida Esperada
Al ejecutar el proyecto de forma exitosa, se observará el siguiente comportamiento:
La terminal mostrará trazas de registro estructuradas ([INFO], [SUCCESS]) que confirman la lectura del archivo JSON, la validación geométrica (16x16), la correcta interpolación matemática y el envío HTTP de cada paquete.
El servidor Python creará (o vaciará, si ya existía) un directorio llamado imagenes_tactiles/ en la raíz del proyecto.
Al concluir la ejecución, dicha carpeta contendrá 50 archivos de imagen (desde capture_0.png hasta capture_49.png) que representan los mapas de calor en alta resolución (128x128) con la escala de colores correspondiente a la presión del sensor.