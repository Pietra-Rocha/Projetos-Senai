🚙 Carrinho Omnidirecional com MQTT, RFID e LEDs RGB (ESP32-S3)
📖 Visão Geral

Projeto completo de um carrinho omnidirecional, controlado via MQTT, com:

Movimentos omnidirecionais

LEDs RGB (NeoPixel)

Leitor RFID (PN532)

Display TFT

Telemetria em JSON (motores, temperatura, timestamp)

Ideal para integração com apps móveis, Node-RED ou dashboards IoT.

🧩 Hardware Utilizado

ESP32-S3

Motores omnidirecionais

MCP23X17 (faróis e setas)

ADS1115 + LM35

VL53L0X

PN532 (RFID)

Fita LED RGB (NeoPixel)

Display TFT

📚 Bibliotecas Necessárias

ArduinoJson

PubSubClient

Adafruit NeoPixel

Adafruit PN532

Adafruit ADS1X15

Adafruit MCP23X17

TFT_eSPI

Biblioteca seguidorlinha

🔐 Configurações Obrigatórias

Antes de compilar, configure:

senhas.h → SSID e senha do Wi-Fi

certificados.h → Certificados MQTT (AWS ou broker seguro)

⚙️ Como Compilar
🔹 Arduino IDE

Abra main.cpp.

Selecione ESP32-S3.

Verifique as bibliotecas instaladas.

Clique em ✔ Verificar.

🔹 PlatformIO
pio run

🔌 Como Gravar no ESP32
🔹 Arduino IDE

Clique em ➡ Upload.

🔹 PlatformIO
pio run --target upload

📡 Comunicação MQTT

Tópico: senai134/comandos

Recebe comandos de movimento

Publica status completo em JSON