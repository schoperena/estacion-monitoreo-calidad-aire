# 🌡️💧💨 Monitor Ambiental LoRa con ESP32 y Google Sheets

¡Bienvenido a este proyecto de monitoreo ambiental! Este sistema utiliza dos placas TTGO LoRa32 V1 (ESP32) para recolectar datos de temperatura, humedad y calidad del aire. Un nodo emisor envía los datos a través de LoRa, y un nodo receptor los capta, los muestra en una pantalla OLED y los sube a una hoja de cálculo de Google Sheets para su visualización y análisis.

---

## 📝 Descripción General

Este proyecto demuestra cómo construir un sistema de monitoreo de bajo costo y largo alcance utilizando:
* **Nodo Emisor (Sender):** Equipado con sensores DHT11 (temperatura y humedad) y MQ135 (calidad del aire). Lee los datos y los envía periódicamente vía LoRa, utilizando el modo de sueño profundo (deep sleep) para ahorrar energía.
* **Nodo Receptor (Receiver):** Recibe los datos LoRa, los muestra en una pantalla OLED integrada, se conecta a una red WiFi y envía la información a una hoja de Google Sheets mediante un script de Google Apps.

Es una solución ideal para monitorear condiciones ambientales en lugares remotos o de difícil acceso donde la conectividad WiFi directa para el sensor no es viable, pero sí para un gateway/receptor.

---

## ⚙️ Hardware Requerido

* 2 x [Placas de desarrollo TTGO LoRa32 V1](https://www.lilygo.cc/products/ttgo-lora32-v1-0) (o similar con ESP32 y LoRa SX1276/8)
    * Estas placas integran un microcontrolador ESP32 (con WiFi y Bluetooth), un chip LoRa y, usualmente, una pantalla OLED de 0.96 pulgadas.
* 1 x Sensor de temperatura y humedad [DHT11](https://www.adafruit.com/product/386) (o DHT22 para mayor precisión).
* 1 x Sensor de calidad del aire [MQ135](https://www.sparkfun.com/products/17045).
* Cables de conexión (Jumpers).
* Fuente de alimentación para ambos nodos (e.g., baterías LiPo para el emisor, USB para el receptor).

---

## 📦 Software y Tecnologías Utilizadas

* **Lenguaje de Programación:** C++ (Arduino Framework).
* **IDE:** [Arduino IDE](https://www.arduino.cc/en/software) o [PlatformIO](https://platformio.org/).
* **Protocolo de Comunicación Inalámbrica:** [LoRa](https://lora-alliance.org/) (Long Range) para la transmisión de datos entre los nodos. LoRa es una tecnología de comunicación inalámbrica de largo alcance y bajo consumo, ideal para aplicaciones IoT.
* **Conectividad:** WiFi para que el nodo receptor suba los datos a internet.
* **Servicios Web:**
    * [Google Sheets](https://www.google.com/sheets/about/): Para almacenar y visualizar los datos.
    * [Google Apps Script](https://developers.google.com/apps-script): Para crear un webhook que reciba los datos y los escriba en Google Sheets.
* **Bibliotecas Arduino Principales:**
    * `LoRa` (by Sandeep Mistry)
    * `DHT sensor library` (by Adafruit)
    * `Adafruit Unified Sensor` (dependencia para DHT)
    * `SSD1306` (para la pantalla OLED, ej. by ThingPulse o Adafruit)
    * `HTTPClient` (para las peticiones web en el ESP32)

---

## 📂 Estructura del Proyecto

```bash
📦estacion-monitoreo-calidad-aire
┣ 📂sender
┃ ┗ 📜sender.ino        # Código para el nodo emisor (ESP32 + Sensores + LoRa TX)
┣ 📂receiver
┃ ┗ 📜receiver.ino  # Código para el nodo receptor (ESP32 + LoRa RX + WiFi + OLED + HTTP)
┣ 📜google_apps_script.gs  # Código del script para Google Sheets (Webhook)
┣ 📜README.md              # Este archivo que estás leyendo :)
```

---

## 🚀 Cómo Funciona

### 📡 Nodo Emisor (`sender.ino`)

1.  **Despertar:** El ESP32 se despierta del modo de sueño profundo (deep sleep).
2.  **Lectura de Sensores:**
    * Lee la temperatura y humedad del sensor DHT11.
    * Lee el valor analógico del sensor MQ135 como una indicación de la calidad del aire.
3.  **Transmisión LoRa:** Empaqueta los datos de los sensores (temperatura, humedad, valor MQ135) en una cadena de texto separada por comas.
4.  **Envío:** Transmite el paquete de datos utilizando el módulo LoRa.
5.  **Dormir:** Vuelve al modo de sueño profundo durante 1 minuto para conservar energía. Este ciclo se repite indefinidamente.

### 🏠 Nodo Receptor (`receiver.ino`)

1.  **Inicialización:**
    * Se conecta a la red WiFi especificada. Muestra el estado de la conexión en la pantalla OLED.
    * Inicializa el módulo LoRa para recibir datos. Muestra el estado en la OLED.
2.  **Recepción de Datos LoRa:**
    * Escucha continuamente paquetes LoRa entrantes.
    * Cuando se recibe un paquete, lee la cadena de datos.
3.  **Procesamiento y Visualización:**
    * Decodifica la cadena recibida para extraer los valores de temperatura, humedad y calidad del aire.
    * Muestra estos valores en la pantalla OLED.
4.  **Envío a Google Sheets:**
    * Si la conexión WiFi está activa, formatea los datos y los envía mediante una petición HTTP POST al webhook de Google Apps Script.
    * Muestra el código de respuesta HTTP en el monitor serie.
    * Si la conexión WiFi se pierde, intenta reconectarse.

### 📊 Google Apps Script (`google_apps_script.gs`) y Google Sheets

1.  **Webhook:** El script de Google Apps actúa como un servicio web (webhook) que puede ser llamado mediante peticiones HTTP (GET o POST).
2.  **Recepción de Datos:** Cuando el nodo receptor envía los datos, el script los recibe como parámetros en la petición.
3.  **Procesamiento de Datos:**
    * Valida que los parámetros necesarios (temperatura, humedad, calidad) estén presentes.
    * Interpreta el valor numérico de la calidad del aire (MQ135) en una descripción textual (ej: "Aire limpio", "Calidad moderada", "Aire contaminado", "Aire muy contaminado").
    * Formatea los números de temperatura, humedad y calidad para usar comas como separador decimal (configurable según preferencias).
4.  **Escritura en Google Sheets:**
    * Abre la hoja de cálculo de Google Sheets especificada por su ID y el nombre de la hoja ("Datos").
    * Añade una nueva fila con los siguientes datos:
        * Fecha y hora actual (automáticamente generada por `new Date()`).
        * Temperatura.
        * Humedad.
        * Valor numérico de la calidad del aire.
        * Interpretación textual de la calidad del aire.
5.  **Respuesta:** Devuelve un mensaje de "OK" si la operación fue exitosa, o un mensaje de error.

---

## 🛠️ Configuración e Instalación

### 1. Conexiones de Hardware

* **Nodo Emisor:**
    * Conecta el sensor DHT11 al pin `GPIO17` del TTGO LoRa32 (o el pin definido en `#define DHTPIN 17`).
    * Conecta el pin de salida analógica (AO) del sensor MQ135 al pin `GPIO34` del TTGO LoRa32 (o el pin definido en `#define MQ135_PIN 34`).
    * Asegúrate de alimentar correctamente los sensores (VCC y GND).

![Emisor con una abteria 18650 y la pantalla activa](https://iili.io/FJKFDN4.jpg)

* **Nodo Receptor:**
    * Generalmente no requiere conexiones adicionales si solo usa la pantalla OLED y LoRa integrados en el TTGO.

![Receptor con carcasa impresa en 3D](https://iili.io/FJKfiyG.jpg)

### 2. Configuración del Entorno Arduino IDE

1.  **Instalar Soporte ESP32:** Si aún no lo has hecho, añade el soporte para placas ESP32 en tu Arduino IDE. Ve a `Archivo > Preferencias` e introduce la siguiente URL en "Gestor de URLs Adicionales de Tarjetas":
    ```
    https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
    ```
    Luego, ve a `Herramientas > Placa > Gestor de Tarjetas`, busca "esp32" e instala el paquete de Espressif Systems (en este caso se trabajó con la version 2.0.9).
2.  **Seleccionar Placa:** En `Herramientas > Placa`, selecciona "TTGO LoRa32-OLED V1" (o la placa ESP32 que estés usando).
3.  **Instalar Bibliotecas:** Ve a `Herramientas > Administrar Bibliotecas` e instala las siguientes:
    * `LoRa` por Sandeep Mistry
    * `DHT sensor library` por Adafruit
    * `Adafruit Unified Sensor` por Adafruit (es una dependencia para la biblioteca DHT)
    * `Adafruit SSD1306` o `ESP8266 and ESP32 OLED driver for SSD1306 display` por ThingPulse (dependiendo de la que uses, tu código indica `SSD1306.h` que es común en la de ThingPulse para TTGO).

### 3. Configuración de Google Sheets y Google Apps Script

1.  **Crear Hoja de Cálculo:** Ve a [Google Sheets](https://docs.google.com/spreadsheets/) y crea una nueva hoja de cálculo.
2.  **Nombrar la Hoja:** Renombra la primera hoja (sheet) a "**Datos**" (el script la busca con este nombre).
3.  **Obtener ID de la Hoja de Cálculo:** Copia el ID de tu hoja de cálculo de la URL. Por ejemplo, si la URL es `https://docs.google.com/spreadsheets/d/ESTE_ES_EL_ID_LARGO/edit#gid=0`, el ID es `ESTE_ES_EL_ID_LARGO`.
4.  **Abrir Editor de Scripts:** En tu hoja de cálculo, ve a `Extensiones > Apps Script`.
5.  **Pegar el Código del Script:** Borra cualquier código existente en el editor y pega el contenido de `google_apps_script.gs` que se proporciona en este proyecto.
6.  **Actualizar ID en el Script:** Dentro del código del script, localiza la línea `var sheetId = "1YG-QNsZTo1uTu1pK27krv6wenaQEufblueP9vAEQdKM";` y reemplaza el ID de ejemplo con el ID real de tu hoja de cálculo que copiaste en el paso 3.
7.  **Desplegar como Aplicación Web:**
    * Haz clic en el botón "**Implementar**" (o "Deploy") en la parte superior derecha.
    * Selecciona "**Nueva implementación**" (o "New deployment").
    * Haz clic en el icono de engranaje ("Seleccionar tipo") y elige "**Aplicación web**" (o "Web app").
    * En "Descripción", puedes poner algo como "Webhook Monitor LoRa".
    * En "Ejecutar como", selecciona "**Yo (tu.email@example.com)**".
    * En "Quién tiene acceso", selecciona "**Cualquier persona**" (o "Anyone"). Esto es importante para que el ESP32 pueda enviar datos sin autenticación compleja.
    * Haz clic en "**Implementar**".
8.  **Autorizar Script:** La primera vez, Google te pedirá que autorices el script para acceder a tus hojas de cálculo. Sigue los pasos y concede los permisos necesarios.
9.  **Copiar URL del Webhook:** Después de una implementación exitosa, se te proporcionará una **URL de la aplicación web**. ¡Esta es tu URL de webhook! Cópiala.

### 4. Configuración del Código ESP32

* **`sender.ino`:**
    * Verifica los pines de los sensores (`DHTPIN`, `MQ135_PIN`).
    * Asegúrate de que la frecuencia LoRa (`LoRa.begin(915E6)`) sea la correcta para tu región y compatible con el receptor (915MHz es común en América).
* **`receiver.ino`:**
    * Actualiza las credenciales WiFi:
        ```cpp
        const char* ssid = "TU_SSID_WIFI";
        const char* password = "TU_PASSWORD_WIFI";
        ```
    * Actualiza la URL del webhook con la que obtuviste de Google Apps Script:
        ```cpp
        const char* webhook = "URL_DE_TU_WEBHOOK_AQUI";
        ```
    * Verifica los pines LoRa (`LoRa.setPins`) y la configuración de la pantalla OLED si es diferente.
    * Asegúrate de que la frecuencia LoRa (`LoRa.begin(915E6)`) coincida con la del emisor.

---

## 💡 Uso

1.  **Cargar Código:**
    * Abre `sender.ino` en tu Arduino IDE, selecciona la placa y puerto correctos, y cárgalo a tu primer TTGO LoRa32.
    * Abre `receiver.ino` en otra instancia del Arduino IDE (o después), selecciona la placa y puerto correctos, y cárgalo a tu segundo TTGO LoRa32.
2.  **Encender Dispositivos:**
    * Alimenta el nodo emisor. Debería empezar a enviar datos LoRa cada minuto.
    * Alimenta el nodo receptor. Debería conectarse a WiFi y luego esperar datos LoRa.
3.  **Monitorear:**
    * Observa el monitor serie del Arduino IDE para ambos nodos para ver mensajes de estado y depuración.
    * El nodo receptor mostrará los datos recibidos en su pantalla OLED.
    * Abre tu hoja de Google Sheets. Deberías ver nuevos datos aparecer cada vez que el receptor envía información. ¡🎉!

---

## 🌟 Características Destacadas

* **Bajo Consumo:** El nodo emisor utiliza deep sleep para una larga duración de batería.
* **Largo Alcance:** Comunicación LoRa para cubrir distancias considerables.
* **Visualización en Tiempo Real:** Datos mostrados en la OLED del receptor.
* **Almacenamiento en la Nube:** Integración con Google Sheets para fácil acceso y análisis de datos históricos.
* **Interpretación de Datos:** El script de Google Apps proporciona una interpretación básica de la calidad del aire.
* **Adaptable:** Fácil de modificar para añadir más sensores o cambiar la lógica.

---

## 🔮 Posibles Mejoras Futuras

* **Calibración del MQ135:** Implementar una rutina de calibración para el sensor MQ135 para obtener lecturas de PPM más precisas.
* **Impelementar un sensor PM2.5 y PM10:** Implementar un sensor más preciso que mida la polución en el aire de particulas PM2.5 y PM10.
* **Cifrado de Datos LoRa:** Añadir una capa de cifrado a los mensajes LoRa para mayor seguridad.
* **Gestión de Errores Mejorada:** Implementar reintentos más robustos para el envío HTTP y un mejor feedback de errores.
* **Alertas:** Configurar el script de Google Apps para enviar alertas por correo electrónico si los valores de los sensores cruzan ciertos umbrales.

---

¡Esperamos que sea util este proyecto! Si tienes alguna pregunta o sugerencia, no dudes en abrir un "Issue" en el repositorio. Happy Hacking! 🚀