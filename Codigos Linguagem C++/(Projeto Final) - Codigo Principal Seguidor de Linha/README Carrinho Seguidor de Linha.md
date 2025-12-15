🚗 Carrinho Seguidor de Linha
📖 Visão Geral

Este projeto implementa um carrinho seguidor de linha utilizando ESP32-S3, com controle de início/parada via botão do encoder.
O sistema utiliza PID para controle dos motores, sensores de linha, além de sensores de temperatura (ADS1115 + LM35) e distância (VL53L0X).

O firmware é voltado para operação autônoma, com lógica de largada segura e parada manual.

🧩 Hardware Utilizado

ESP32-S3

Sensores de linha

ADS1115 + LM35 (temperatura)

VL53L0X (distância)

MCP23X17 (expansão de pinos)

Motores DC com controle PWM

Botão (encoder)

📚 Bibliotecas Necessárias

Instale todas pelo Gerenciador de Bibliotecas da Arduino IDE:

ArduinoJson

Adafruit ADS1X15

Adafruit MCP23X17

Adafruit VL53L0X

PubSubClient

Biblioteca personalizada seguidorlinha

⚙️ Como Compilar
🔹 Arduino IDE

Abra o arquivo main.cpp.

Selecione a placa ESP32-S3.

Escolha a porta correta.

Clique em ✔ Verificar.

🔹 PlatformIO
pio run

🔌 Como Gravar no ESP32
🔹 Arduino IDE

Conecte o ESP32 via USB.

Clique em ➡ Upload.

🔹 PlatformIO
pio run --target upload

▶️ Funcionamento

Pressione o botão com o carrinho sobre a faixa central para iniciar.

Pressione novamente para parar o carrinho.

O seguimento de linha é feito automaticamente via PID.