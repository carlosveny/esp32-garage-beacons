# esp32-garage-beacons
Detector de presencia mediante beacons usando un esp32 programado en Arduino IDE, que reporta la información a un bus KNX.

## Información importante
### Hardware
Elementos necesarios para ejecutar la app:
- ESP32 (en mi caso, ESP32-WROOM)
- iBeacon (en mi caso, HolyIOT)

Si se quiere realizar la integración con KNX se require una BCU (en mi caso, Siemens 5WG1117-2AB12).

### Dependencias
Es necesario instalar la librería [esp32-tpuart](https://github.com/carlosveny/esp32-tpuart) para la comunicación con el bus KNX. Esta librería es un fork de [arduino-tpuart](https://github.com/Domos-Snips/arduino-tpuart) para que funcione sobre un ESP32.

### Instalación Arduino IDE para ESP32
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
    - Buscar **esp32** e instalar la de **Espressif** (testeado con la v3.0.1)
5. Conectar el esp32 al ordendador mediante el cable USB-C (comprobar que sea un cable de datos y no solo de carga).
6. Seleccionar nuestra placa en Arduino IDE de la siguiente forma:
    - Herramientas -> Placa -> esp32 -> ESP32-WROOM-DA Module. *(cambiar la placa según la que se tenga)*.
7. Seleccionar el puerto donde está conectado el esp32 de la siguiente forma:
    - Herramientas -> Puerto -> *seleccionar donde está el esp32*
8. Lanzar el programa al esp32.

## Funcionamiento
La app se encarga de realizar escaneos, dados unos parámetros de configuración, con la finalidad de encontrar determinados beacons. Cada beacon se encuentra dentro de un coche, por lo que si la app detecta un beacon, significa que el coche asociado se encuentra en el garaje.

Los escaneos no se realizan de manera regular, sino únicamente cuando se recibe una señal conocida del bus KNX (puerta garaje abierta o cerrada, por ejemplo). De esta manera evitamos realizar escaneos innecesarios. También se puede forzar un escaneo manual con un pulsador conectado al PIN determinado en la configuración. Cabe destacar que también se puede conectar un LED a otro PIN determinado en la configuración, el cual se enciende cuando se está escaneando, a modo de feedback.

Cuando se detecta un beacon, se envía un `1` a la dirección KNX asociada según la configuración. En caso de no detectar algún beacon se envía un `0` a su dirección asociada.

### Falsos negativos
Para prevenir los falsos negativos (no detectar un beacon cuando realmente sí está presente), cada escaneo se compone de varios sub-escaneos (según la configuración) mediante los cuales la app se asegura de que realmente un beacon NO está presente.

De esta manera, cuando se detecta un beacon, enseguida se envía un `1` a su dirección asociada; pero cuando no se detecta, se espera a terminar la ronda de sub-escaneos para enviar un `0`. Esto es porque si se detecta un beacon, es 100% seguro que está presente (los falsos positivos no existen); pero si no se detecta, puede ser que sí esté presente y el esp32 no lo haya detectado.

En caso de que en un sub-escaneo ya se detecten todos los beacons, no se realizan el resto de sub-escaneos, ya que los falsos positivos no existen.

## Parámetros de configuración
Esta app tiene varios parámetros de configuración para modificar su funcionamiento.

Cabe recordar que la plantilla de configuración se encuentra en el archivo `Config-example.h`, pero se debe modificar el nombre del archivo a `Config.h` para que la app cargue la configuración.

| Parámetro              | Explicación |
| ---------------------- | ----------- |
| TOTAL_SECONDS_SCANNING | Total de segundos durante los cuales se van a estar realizando escaneos |
| NUMBER_OF_SCANS | Número de sub-escaneos que componen 1 escaneo completo (para evitar falsos negativos) |
| SCAN_TIME | Total de segundos que dura un sub-escaneo |
| START_SCANNING | Si es `true` la app empieza realizando un escaneo inicial |
| KNX_IP_ADDRESS | Dirección IP para identificar al ESP32 en el bus KNX
| BLE_ADDRESSES | Array de direcciones MAC de cada uno de los beacons a detectar |
| KNX_ADDRESSES | Array de direcciones KNX asociadas a cada uno de los beacons. Debe tener la misma longitud que *BLE_ADDRESSES*, y el orden determina la asociación con el mismo |
| KNX_READ_ADDRESSES | Array de direcciones KNX que se escuchan para iniciar los escaneos |
| PIN_LED | PIN del ESP32 donde va conectado el LED que se enciende al escanear |
| PIN_BUTTON | Pin del ESP32 donde va conectado el pulsador para forzar un escaneo manual |

### Ejemplo de ejecución
```ino
const int TOTAL_SECONDS_SCANNING = 60;
const int NUMBER_OF_SCANS = 3;
const int SCAN_TIME = 5; 
```
En este caso se van a realizar **3 sub-escaneos** de **5 segundos** cada uno, por lo que 1 escaneo completo realmente durará 3*5 = 15 segundos. Como hemos indicado que se tiene que estar **60 segundos** escaneando, se van a realizar 60/15 = 4 escaneos completos.
