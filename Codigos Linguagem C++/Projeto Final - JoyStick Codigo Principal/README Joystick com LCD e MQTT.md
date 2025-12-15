🎮 Joystick com LCD e MQTT

📖 Visão Geral

Este projeto transforma um ESP32 em um controle remoto, utilizando:

Botões físicos

Joystick analógico

Display LCD I2C 20x4

Comunicação MQTT segura

Os comandos são enviados em JSON para o carrinho.


🧩 Hardware Utilizado

ESP32

LCD I2C 20x4

Botões (A, B, C, D, E, F, K)

Joystick analógico


📚 Bibliotecas Necessárias

ArduinoJson

LiquidCrystal_I2C

PubSubClient

WiFiClientSecure


🔐 Configuração Inicial

Edite os arquivos:

senhas.h → Wi-Fi

certificados.h → Certificados MQTT

internet.h → Broker MQTT


⚙️ Como Compilar
🔹 Arduino IDE

Abra main.cpp.

Selecione a placa ESP32 correta.

Clique em ✔ Verificar.


🔌 Como Gravar no ESP32
🔹 Arduino IDE

Clique em ➡ Upload.



📤 Dados Enviados (JSON)
{
  "botaoA": 1,
  "botaoB": 0,
  "botaoC": 0,
  "botaoD": 0,
  "botaoE": 0,
  "botaoF": 0,
  "botaoK": 0,
  "joystick": {
    "X": 2048,
    "Y": 1980
  }
}