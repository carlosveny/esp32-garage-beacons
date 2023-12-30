# esp32-garage-beacons
Detector de presencia mediante beacons usando un esp32 programado en Arduino IDE.

## Instalación
1. Descargar [Arduino IDE](https://www.arduino.cc/en/software). En este caso se ha usado el nuevo IDE 2.0.
2. Abrir el IDE y dejar que se instalen todas las librerías base.
3. Añadir placas extra al IDE, de la siguiente forma:
   - Archivo -> Preferencias -> Gestor de placas adicionales.
   - Aquí añadir las siguientes URLs:
       - `https://dl.espressif.com/dl/package_esp32_index.json`
       - `https://resource.heltec.cn/download/package_heltec_esp32_index.json`
    - Confirmar la selección y volver al inicio del IDE.
4. Instalar el software de nuestra placa de la siguiente forma:
    - Herramientas -> Placas -> Gestor de placas.
    - Buscar **esp32** e instalar la de **Espressif**.
5. Conectar el esp32 al ordendador mediante el cable USB-C (comprobar que sea un cable de datos y no solo de carga).
6. Seleccionar nuestra placa en Arduino IDE de la siguiente forma:
    - Herramientas -> Placa -> esp32 -> ESP32-WROOM-DA Module. *(cambiar la placa según la que se tenga)*.
7. Seleccionar el puerto donde está conectado el esp32 de la siguiente forma:
    - Herramientas -> Puerto -> *seleccionar donde está el esp32*
8. Lanzar el programa al esp32.

## Funcionamiento
