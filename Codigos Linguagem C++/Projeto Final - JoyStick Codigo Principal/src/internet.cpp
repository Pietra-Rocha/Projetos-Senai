#include <Arduino.h>
#include "Internet.h"
#include <WiFi.h> 
#include "senhas.h"

//* --------- CONFIGURACAO DO WIFI ------- */

const unsigned long tempoEsperaConexao = 15000;  // Tempo máximo (em milissegundos) para tentar conectar ao WiFi
const unsigned long tempoEsperaReconexao = 5000; 



void conecxaoWiFi()
{
  Serial.printf("Conectando ao Wifi: %s", SSID); // Imprime na serial o SSID da rede sendo conectada

  WiFi.begin(SSID, SENHA); // Inicia a conexão com a rede WiFi

  unsigned long tempoInicialWiFi = millis(); // Guarda o tempo em que a tentativa de conexão começou

  // Espera a conexão completar ou até o tempo limite ser atingido
  while (WiFi.status() != WL_CONNECTED && millis() - tempoInicialWiFi < tempoEsperaConexao)
  {
    Serial.print("."); // Imprime um ponto a cada meio segundo para indicar progresso
    delay(500);        // Aguarda 500 milissegundos antes de checar novamente
  }

  // Verifica se a conexão foi bem-sucedida
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi Conectado com sucesso! "); // Mensagem de sucesso
    Serial.println("Endereço IP: ");                  // Mostra o IP atribuído ao dispositivo
    Serial.println(WiFi.localIP());
  }
  else
  {
    // Caso não tenha conseguido conectar, mostra mensagem de erro
    Serial.println("\n Falha ao conectar ao WiFi. Verifique o nome da rede e a senha ");
  }
}

// Função para checar se o WiFi ainda está conectado
void checkWiFi()
{
  unsigned long tempoAtual = millis();                         // Obtém o tempo atual desde que o dispositivo foi ligado (em milissegundos)
  static unsigned long tempoUltimaConecxao = 0;                // Guarda o momento da última verificação da conexão WiFi

  if (tempoAtual - tempoUltimaConecxao > tempoEsperaReconexao) // Verifica se já se passou o tempo necessário desde a última verificação
  {
    if (WiFi.status() != WL_CONNECTED) // Se o WiFi foi desconectado
    {
      Serial.println("\nConexão Perdida! Tentando reconectar..."); // Informa que vai tentar reconectar
      conecxaoWiFi();                                              // Tenta reconectar
    }
    tempoUltimaConecxao = tempoAtual; // Atualiza o tempo da última verificação
  }
}
