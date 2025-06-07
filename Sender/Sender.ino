// Copyright (c) 2025 schoperena (Sebastian Choperena Solano)

// Este archivo es parte de estacion-monitoreo-calidad-aire.
// estacion-monitoreo-calidad-aire se distribuye bajo la licencia Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International.

// Deberías haber recibido una copia de la licencia junto con este programa.
// Si no, consulta <https://creativecommons.org/licenses/by-nc-sa/4.0/>.

#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"
// #include "SSD1306.h" // Pantalla no utilizada

// --- Pines y Tipos de Sensores ---
#define DHTPIN 17       // Pin del sensor DHT11/DHT22
#define DHTTYPE DHT11   // Tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);

#define MQ135_PIN 34    // Pin del sensor MQ135 (ADC)

// --- Configuración Deep Sleep ---
#define uS_TO_S_FACTOR 1000000ULL  // Factor de conversión de microsegundos a segundos
#define TIME_TO_SLEEP  60          // Tiempo que el ESP32 estará en deep sleep (segundos)

// SSD1306 display(0x3c, 4, 15);  // SDA = GPIO4, SCL = GPIO15 // Pantalla no utilizada

void setup() {
  Serial.begin(115200);
  // Pequeña espera para permitir que el monitor serie se conecte después de un reinicio/despertar
  delay(1000); 
  Serial.println("ESP32 despertó.");

  // --- Iniciar pantalla OLED (comentado ya que no se usa) ---
  // pinMode(16, OUTPUT);
  // digitalWrite(16, LOW);
  // delay(50);
  // digitalWrite(16, HIGH);
  // display.init();
  // display.setFont(ArialMT_Plain_10);

  // --- Iniciar sensores ---
  dht.begin();
  delay(1000); // Espera para estabilización del sensor DHT

  // --- Iniciar LoRa ---
  // Pines para TTGO LoRa32 V1: SCK=5, MISO=19, MOSI=27, NSS=18, RST=14, DIO0=26
  // SPI.begin(5, 19, 27, 18); // Opcional: LoRa.begin() usualmente maneja esto.
  LoRa.setPins(18, 14, 26); // Configura NSS, RST, DIO0
  if (!LoRa.begin(915E6)) { // Frecuencia para América (ajusta si es necesario)
    Serial.println("Fallo al iniciar LoRa. Intentando de nuevo después del deep sleep.");
    // Si LoRa falla, igual irá a dormir y reintentará al despertar.
  } else {
    Serial.println("LoRa Emisor listo.");
  }

  // --- Leer datos de los sensores ---
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int mq135Value = analogRead(MQ135_PIN);

  // Verificar si la lectura del DHT fue exitosa
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error al leer del sensor DHT!");
    // Aquí podrías decidir enviar valores de error o no enviar nada
    // Por ahora, se enviarán los valores NaN si hay error.
  }

  // --- Mostrar en OLED (comentado ya que no se usa) ---
  // display.clear();
  // display.drawString(0, 0, "Temp: " + String(temperature, 1) + " C");
  // display.drawString(0, 12, "Humedad: " + String(humidity, 1) + " %");
  // display.drawString(0, 24, "Calidad aire: " + String(mq135Value));
  // display.display();

  // --- Enviar datos por LoRa ---
  Serial.print("Enviando paquete: ");
  Serial.print(temperature); Serial.print("°C, ");
  Serial.print(humidity); Serial.print("%, MQ135: ");
  Serial.println(mq135Value);
  
  LoRa.beginPacket();
  LoRa.print(temperature); LoRa.print(",");
  LoRa.print(humidity); LoRa.print(",");
  LoRa.print(mq135Value);
  LoRa.endPacket();

  Serial.println("Datos enviados por LoRa.");
  
  // --- Configurar y entrar en Deep Sleep ---
  Serial.println("Entrando en modo deep sleep por " + String(TIME_TO_SLEEP) + " segundos.");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  
  // Es buena práctica detener LoRa antes de dormir para ahorrar más energía, aunque no siempre es estrictamente necesario.
  // LoRa.end(); // Desactiva el módulo LoRa
  // SPI.end();  // Desactiva el bus SPI

  Serial.flush(); // Asegura que todos los mensajes seriales se envíen antes de dormir
  esp_deep_sleep_start();

  // El código después de esp_deep_sleep_start() no se ejecutará hasta el próximo despertar.
}

void loop() {
  // Esta función no se alcanzará porque el ESP32 estará en deep sleep.
  // Se deja vacía intencionalmente.
}