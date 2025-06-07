// Copyright (c) 2025 schoperena (Sebastian Choperena Solano)

// Este archivo es parte de estacion-monitoreo-calidad-aire.
// estacion-monitoreo-calidad-aire se distribuye bajo la licencia Creative Commons
// Attribution-NonCommercial-ShareAlike 4.0 International.

// Deberías haber recibido una copia de la licencia junto con este programa.
// Si no, consulta <https://creativecommons.org/licenses/by-nc-sa/4.0/>.

#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "SSD1306.h"
#include "screens_sch.h"

const char* ssid = "TU_SSID_WIFI";
const char* password = "TU_PASSWORD_WIFI";
const char* webhook = "URL_DE_TU_WEBHOOK_AQUI";

SSD1306 display(0x3c, 4, 15); // SDA, SCL

void setup() {
  Serial.begin(115200);

  // Iniciar pantalla OLED
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  delay(50);
  digitalWrite(16, HIGH);
  display.init();
  display.flipScreenVertically();
  boot_logo();
  qr_git();
  display.setFont(ArialMT_Plain_10);

  // Conectar WiFi
  WiFi.begin(ssid, password);
  display.drawString(0, 0, "Conectando WiFi...");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  display.clear();
  display.drawString(0, 0, "WiFi conectado");
  display.display();
  delay(1000);

  // Iniciar LoRa
  LoRa.setPins(18, 14, 26); // NSS, RST, DIO0
  if (!LoRa.begin(915E6)) {
    Serial.println("Fallo al iniciar LoRa");
    while (1);
  }
  Serial.println("LoRa Receptor listo");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    Serial.println("Recibido: " + received);

    int firstComma = received.indexOf(',');
    int secondComma = received.indexOf(',', firstComma + 1);

    if (firstComma != -1 && secondComma != -1) {
      float temperature = received.substring(0, firstComma).toFloat();
      float humidity = received.substring(firstComma + 1, secondComma).toFloat();
      int calidad = received.substring(secondComma + 1).toInt();

      // Mostrar en pantalla
      display.clear();
      display.drawString(0, 0, "Temp: " + String(temperature, 1) + " C");
      display.drawString(0, 12, "Humedad: " + String(humidity, 1) + " %");
      display.drawString(0, 24, "Calidad aire: " + String(calidad));
      display.display();

      // Enviar a Google Sheets con POST
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(webhook);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        String postData = "temperatura=" + String(temperature) +
                          "&humedad=" + String(humidity) +
                          "&calidad=" + String(calidad);

        int httpResponseCode = http.POST(postData);
        Serial.println("HTTP: " + String(httpResponseCode));
        http.end();

        delay(1000); // pequeño delay para no saturar peticiones
      } else {
        Serial.println("WiFi desconectado, reintentando...");
        WiFi.disconnect();
        WiFi.begin(ssid, password);
        unsigned long startAttemptTime = millis();

        // Esperar conexión hasta 10 segundos
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
          delay(500);
          Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("\nWiFi reconectado");
        } else {
          Serial.println("\nNo se pudo reconectar WiFi");
        }
      }
    }
  }
}
