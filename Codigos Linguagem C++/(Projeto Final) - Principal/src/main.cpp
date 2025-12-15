#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MCP23X17.h>
#include "Adafruit_VL53L0X.h"
#include "seguidorlinha.h"
#include <senhas.h>
#include <certificados.h>
#include <ArduinoJson.h>
#include <Adafruit_PN532.h>
#include <img.h> // fornece array katchau
#include <TFT_eSPI.h>
#include "leds.h"
#include <time.h>
#include <Adafruit_NeoPixel.h>

// ======================== CONFIGURAÇÕES DE MQTT ==========================
const int mqtt_port = 8883;
const char *mqtt_client_id = "Carrinho_Mesa01";
const char *mqtt_topic_sub = "senai134/comandos"; // RECEBE comandos
const char *mqtt_topic_pub = "senai134/comandos"; // ENVIA telemetria/status

WiFiClientSecure espClient;
PubSubClient client(espClient);

// ======================== Conf. (Led RGB) ==========================

#define PINO_LEDS_RGB 45
#define QTD_LEDS 40

Adafruit_NeoPixel fitaRGB(QTD_LEDS, PINO_LEDS_RGB, NEO_GRB + NEO_KHZ800);

// ======================== PINOS (motores e MCP mantidos) ==========================
#define pinM0dir 14
#define pinM0Esq 13
#define pinM1dir 10
#define pinM1Esq 46
#define pinM2dir 3
#define pinM2Esq 18
#define pinM3dir 12
#define pinM3Esq 11

#define frequenciaPWM 20000
#define resolucaoPWM 8

static int PINO_BOTAO_ENCODER = 19;

// =========================== VARIÁVEIS GLOBAIS ==========================
int contA = 0, contB = 0, contC = 0, contD = 0, contE = 0, contF = 0, contK = 0;
int ultimoExecA = 0, ultimoExecB = 0, ultimoExecC = 0, ultimoExecD = 0, ultimoExecE = 0, ultimoExecF = 0, ultimoExecK = 0;
int ultimoX = 0, ultimoY = 0; // mantidos para telemetria (se quiser)

bool botaoAPressionado = 0, botaoASolto = 0,
     botaoBPressionado = 0, botaoBSolto = 0,
     botaoCPressionado = 0, botaoCSolto = 0,
     botaoDPressionado = 0, botaoDSolto = 0,
     botaoEPressionado = 0, botaoESolto = 0,
     botaoFPressionado = 0, botaoFSolto = 0,
     botaoKPressionado = 0, botaoKSolto = 0;

int selecionarCaixa = 0;
uint8_t sucess = false;

Adafruit_MCP23X17 mcp;
Adafruit_ADS1115 ads;
Adafruit_VL53L0X lox;
Carrinho carrinho(mcp);
TFT_eSPI tft;
Leds leds(mcp);
Adafruit_PN532 nfc(-1, -1);

// ----------------- RGB ----------------

unsigned long ultimoMovimento = 0;
int intervaloChase = 5; // velocidade da animação (ms)
int posicaoChase = 0;

// Controle de pisca individual
bool estadoPiscaDir = false;
bool estadoPiscaEsq = false;

unsigned long ultimoPiscaDir = 0;
unsigned long ultimoPiscaEsq = 0;

static unsigned long ultimaLeitura = 0;
const unsigned long intervalo = 1000; // 1s sem bloquear código

const unsigned long intervaloPisca = 300; // piscar padrão de carro

bool estadoPiscaAlerta = false;
unsigned long ultimoPiscaAlerta = 0;

// Variáveis de controle vindas do MQTT
bool farolDianteiro = false, farolTraseiro = false, piscaDireita = false, piscaEsquerda = false;
bool estadoPisca = false;
unsigned long previousMillis = 0;
const unsigned long interval = 500; // intervalo piscas
static uint32_t ATRASO_PARTIDA_MS = 300;
static uint32_t instanteInicioAtraso = 0;
static bool emCorrida = false;
static bool emAtrasoPartida = false;

const uint8_t pinMotor[4][2] = {
    {pinM0Esq, pinM0dir},
    {pinM1Esq, pinM1dir},
    {pinM2Esq, pinM2dir},
    {pinM3Esq, pinM3dir}};
const uint8_t chMotor[4][2] = {
    {0, 1},
    {2, 3},
    {4, 5},
    {6, 7}};
const uint8_t velocidadeMax = 200;
const uint8_t incremento = 5;
const int tempoRampa = 20;
const int tempoMovimento = 2000;

// Sensores timing (não usados no loop reduzido)
unsigned long ultimaLeituraTemp = 0;
const unsigned long intervaloLeituraTemp = 10000;
unsigned long ultimaLeituraDist = 0;
const unsigned long intervaloLeituraDist = 1000;
#define CALIBRACAO false

// Ajuste o fuso horário (exemplo: -3h para Brasil)
const long gmtOffset_sec = -3 * 3600;
const int daylightOffset_sec = 0;

// RFID
uint8_t uid[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0};

int modoLeituraRFID = 0;
bool flagModoLeitura = false;
bool modoChase = false;

// ======================== PROTÓTIPOS ==========================
void sincronizarTempo();
void conectarWiFi();
void reconnectMQTT();
void callback(char *topic, byte *message, unsigned int length);
void publicarTelemetria(float temp, int dist);
void aplicarDirecao(int dir[4]);
void frente();
void tras();
void lateralEsquerda();
void lateralDireita();
void diagonalFrenteDireita();
void diagonalFrenteEsquerda();
void diagonalTrasDireita();
void diagonalTrasEsquerda();
void girarProprioEixo();
void stop();
void parar();
void pararCarrinho();
void imagem();
void limparTela();
void enviarJSONMotores(int dir[4], int vel);
void enviarPacoteCompleto();
void temperaturaMotores();
void Leituraproximidade();
void acenderRGBvermelho();
void acenderRGBbranco();
void acenderRGBamarelo();
void apagarRGB();
void efeitoChase();
bool lerCaixas();
bool compararCartao(const uint8_t (&a)[8], const uint8_t (&b)[8]);
void atualizabotoes();
void atualizaRGB();
void atualizaLeituraCaixas();

// ======================== SETUP ==========================
void setup()
{
  // MÍNIMO necessário: Serial, I2C para MCP (faróis/piscas), carrinho e PWM dos motores,
  // WiFi/MQTT (certificados e callback) e tentativa de conexão ao broker.
  Serial.begin(115200);

  // leds.begin();
  tft.init();
  tft.setRotation(1); // ajustado

  limparTela();

  // I2C (necessário para MCP23X17 que usamos para faróis/piscas)
  Wire.begin();
  ads.begin();
  lox.begin();

  // leitr de caixas
  nfc.begin();

  // Inicializa MCP23X17 (necessário para controlar faróis/piscas via MQTT)
  if (!mcp.begin_I2C())
  {
    Serial.println(" Erro ao inicializar MCP23X17!");
    while (1)
      ;
  }
  // configura pinos 10..15 como saída (usados para LEDs/faróis/piscas)
  for (int p = 10; p <= 15; p++)
  {
    mcp.pinMode(p, OUTPUT);
    mcp.digitalWrite(p, LOW);
  }

  // Inicializa o carrinho (biblioteca) - mantém para garantir motors control funcionando
  carrinho.begin();
  carrinho.setPID(6.0f, 0.5f, 0.5f);
  carrinho.setVelocidades(25.0f, 0.0f);

  // Configura PWM dos motores (ledc)
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      ledcSetup(chMotor[i][j], frequenciaPWM, resolucaoPWM);
      ledcAttachPin(pinMotor[i][j], chMotor[i][j]);
    }
  }

  // === MQTT (seguro) ===
  espClient.setCACert(AWS_ROOT_CA);
  espClient.setCertificate(AWS_CERT);
  espClient.setPrivateKey(AWS_KEY);

  client.setServer(AWS_BROKER, mqtt_port);
  client.setCallback(callback);

  // configura pinos 10..15 como saída (usados para LEDs/faróis/piscas)
  for (int p = 10; p <= 15; p++)
  {
    mcp.pinMode(p, OUTPUT);
    mcp.digitalWrite(p, LOW);
  }

  fitaRGB.begin(); // configuracao dos leds rgb
  fitaRGB.show();

  // Conectar WiFi e MQTT (mantive as chamadas; conectarWiFi usa SSID/SENHA definidos em senhas.h)
  conectarWiFi();
  sincronizarTempo();
  reconnectMQTT();
  client.setBufferSize(2048);
}

// ======================== LOOP ==========================
void loop()
{
  leds.atualizar();
  atualizabotoes();
  atualizaRGB();
  atualizaLeituraCaixas();

  // Mantém conexão MQTT
  if (!client.connected())
    reconnectMQTT();
  client.loop();

  if (botaoAPressionado)
  {
    frente();
    leds.ligarFarois();
    acenderRGBbranco();
  }

  else if (botaoASolto)
  {
    parar();
    leds.desligarFarois();
    apagarRGB();
  }

  if (botaoBPressionado)
  {
    lateralDireita();
    leds.piscarSetaDireita();
    acenderRGBamarelo();
  }
  else if (botaoBSolto)
  {
    parar();
    leds.desligarSetaDireita();
    apagarRGB();
  }

  if (botaoDPressionado)
  {
    lateralEsquerda();
    leds.piscarSetaEsquerda();
    acenderRGBamarelo();
  }
  else if (botaoDSolto)
  {
    parar();
    leds.desligarSetaEsquerda();
    apagarRGB();
  }

  if (botaoCPressionado)
  {
    tras();
    leds.ligarLanternas();
    acenderRGBvermelho();
  }
  else if (botaoCSolto)
  {
    parar();
    leds.desligarLanternas();
    apagarRGB();
  }

  if (botaoKPressionado)
  {
    //
    leds.piscarFaroisJuntos();
    leds.piscarLanternasJuntas();
    leds.piscarSetaDireita();
    leds.piscarSetaEsquerda();
    modoChase = true;
    imagem();
  }
  else if (botaoKSolto)
  {
    leds.desligarTodos();
    limparTela();
    modoChase = false;
  }




  if (botaoFPressionado)
  {
   if (!flagModoLeitura)
      modoLeituraRFID++;

      if(modoLeituraRFID>1) modoLeituraRFID = 0;

    flagModoLeitura = true;
  }


  else if (botaoFSolto)
  {
    flagModoLeitura = false;
  }



}

// ======================== FUNÇÕES ==========================

// === Wi-Fi ===
void conectarWiFi()
{
  Serial.print("Conectando ao Wi-Fi ");
  Serial.println(SSID);
  WiFi.begin(SSID, SENHA);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(400);
    Serial.print(".");
  }
  Serial.println("\n Wi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}
void sincronizarTempo()
{
  Serial.println("⏳ Sincronizando horário NTP...");

  configTime(gmtOffset_sec, daylightOffset_sec,
             "pool.ntp.org", "time.nist.gov");

  time_t now = time(nullptr);

  // aguarda sincronizar (retorna > 100000 quando deu certo)
  while (now < 100000)
  {
    delay(200);
    Serial.print(".");
    now = time(nullptr);
  }

  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  Serial.println("\n✔ Horário NTP sincronizado!");
  Serial.printf("⏰ Agora: %02d/%02d/%04d %02d:%02d:%02d\n",
                timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}
void reconnectMQTT()
{
  while (!client.connected())
  {
    Serial.print("Conectando ao MQTT...");
    if (client.connect(mqtt_client_id))
    {
      Serial.println(" Conectado!");
      client.subscribe(mqtt_topic_sub);
      Serial.print(" Inscrito em: ");
      Serial.println(mqtt_topic_sub);
    }
    else
    {
      Serial.printf("Falhou, rc=%d. Tentando em 5s...\n", client.state());
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("\n Mensagem recebida no tópico: ");
  Serial.println(topic);

  // Converte o payload em string
  String msg;
  for (unsigned int i = 0; i < length; i++)
    msg += (char)payload[i];

  Serial.print(" JSON recebido: ");
  Serial.println(msg);

  // Decodifica o JSON (tamanho estático razoável)
  StaticJsonDocument<1024> doc;
  DeserializationError erro = deserializeJson(doc, msg);
  if (erro)
  {
    Serial.print(" Erro JSON: ");
    Serial.println(erro.c_str());
    return;
  }

  // ---------------------------
  //  COMANDOS DE MOTORES (mantido)
  // ---------------------------
  if (doc.containsKey("tipo") && String((const char *)doc["tipo"].as<const char *>()) == "motores")
  {
    // Ignora mensagens enviadas pelo próprio carrinho
    if (doc.containsKey("source") && String((const char *)doc["source"].as<const char *>()) == "carrinho")
    {
      Serial.println("Ignorando mensagem de motores enviada por mim mesmo.");
      return;
    }

    Serial.println(" Comando de motores recebido!");
    for (int i = 0; i < 4; i++)
    {
      String motor = "M" + String(i);
      int direcao = doc[motor]["direcao"] | 0; // -1 = ré, 0 = parar, 1 = frente
      int velocidade = doc[motor]["velocidade"] | 0;

      if (velocidade > velocidadeMax)
        velocidade = velocidadeMax;

      // Usa ledcWrite para enviar PWM aos canais correspondentes
      if (direcao == 0)
      {
        ledcWrite(chMotor[i][0], 0);
        ledcWrite(chMotor[i][1], 0);
      }
      else if (direcao > 0)
      {
        // frente -> canal[1] recebe PWM; canal[0] 0
        ledcWrite(chMotor[i][0], 0);
        ledcWrite(chMotor[i][1], velocidade);
      }
      else
      {
        // ré -> canal[0] recebe PWM; canal[1] 0
        ledcWrite(chMotor[i][0], velocidade);
        ledcWrite(chMotor[i][1], 0);
      }

      Serial.printf(" M%d -> dir: %d | vel: %d\n", i, direcao, velocidade);
    }

    Serial.println(" Motores atualizados via MQTT.");
    return;
  }

  // ===========================================================
  // ⚡ COMANDOS DO APP INVENTOR (direcao / select / start)
  // ===========================================================

  // 1) Modo manual
  if (doc.containsKey("select"))
  {
    int v = doc["select"];
    if (v == 0)
    {
      Serial.println(">> MODO MANUAL ATIVADO");
      // coloque aqui se quiser apagar LEDs ou zerar algo
    }
  }

  // 2) Modo automático
  if (doc.containsKey("start"))
  {
    int v = doc["start"];
    if (v == 10)
    {
      Serial.println(">> MODO AUTOMÁTICO ATIVADO");
      // coloque aqui sua função de automático se existir
    }
  }

  // 3) Movimentação
  if (doc.containsKey("direcao"))
  {

    int d = doc["direcao"];
    Serial.print(">> DIRECAO = ");
    Serial.println(d);

    switch (d)
    {
    case 1:
      frente();
      break;
    case 2:
      tras();
      break;
    case 3:
      lateralDireita();
      break; // direita
    case 4:
      lateralEsquerda();
      break; // esquerda

    case 5:
      diagonalFrenteEsquerda();
      break;
    case 6:
      diagonalTrasEsquerda();
      break;
    case 7:
      diagonalFrenteDireita();
      break;
    case 8:
      diagonalTrasDireita();
      break;
    case 9:
      girarProprioEixo();
      break;
    case 10:
      stop();
      break;

    default:
      parar();
      break;
    }
  }

  // ---------------------------
  //  COMANDOS DE BOTÕES REMOTOS (movement control)
  // ---------------------------
  // atualiza contA..contD como antes
  if (doc.containsKey("botaoA"))
    contA = doc["botaoA"] | 0;
  if (doc.containsKey("botaoB"))
    contB = doc["botaoB"] | 0;
  if (doc.containsKey("botaoC"))
    contC = doc["botaoC"] | 0;
  if (doc.containsKey("botaoD"))
    contD = doc["botaoD"] | 0;
  if (doc.containsKey("botaoE"))
    contE = doc["botaoE"] | 0;
  if (doc.containsKey("botaoF"))
    contF = doc["botaoF"] | 0;
  if (doc.containsKey("botaoK"))
    contK = doc["botaoK"] | 0;
}

// ======= Publicação =======
void publicarTelemetria(float temp, int dist)
{
  StaticJsonDocument<256> doc;
  if (temp >= 0)
    doc["temperatura"] = temp;
  if (dist >= 0)
    doc["distancia"] = dist;
  doc["uptime_ms"] = millis();

  char buffer[256];
  size_t n = serializeJson(doc, buffer);
  bool ok = client.publish(mqtt_topic_pub, buffer, n);
  Serial.print(" Telemetria publicada: ");
  Serial.println(buffer);
  if (!ok)
    Serial.println(" Falha ao publicar telemetria.");
}

// ======= PWM / movimento =======
void aplicarDirecao(int dir[4])
{
  for (int i = 0; i < 4; i++)
  {
    if (dir[i] == 1)
    {
      ledcWrite(chMotor[i][0], 0);
      ledcWrite(chMotor[i][1], velocidadeMax);
    }
    else if (dir[i] == -1)
    {
      ledcWrite(chMotor[i][0], velocidadeMax);
      ledcWrite(chMotor[i][1], 0);
    }
    else
    {
      ledcWrite(chMotor[i][0], 0);
      ledcWrite(chMotor[i][1], 0);
    }
  }
}

// MOVIMENTOS NÃO-BLOQUEANTES: aplicam direção imediatamente e publicam estado.
void frente()
{
  int dir[4] = {-1, 1, -1, 1}; //-1, 1, -1, 1
  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  mcp.digitalWrite(11, HIGH);
  mcp.digitalWrite(12, HIGH);
  leds.desligarSetaDireita();
  leds.desligarSetaEsquerda();
  leds.desligarLanternas();
}
void tras()
{
  int dir[4] = {1, -1, 1, -1}; // 1, -1, 1, -1
  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  mcp.digitalWrite(14, HIGH);
  mcp.digitalWrite(15, HIGH);
  leds.desligarSetaDireita();
  leds.desligarSetaEsquerda();
  leds.desligarFarois();
}
void lateralDireita()
{
  // Movimento lateral (strafing) verdadeiro para OMNIDIRECIONAIS
  int dir[4] = {
      1,  // M0
      -1, // M1
      -1, // M2
      1   // M3
  };

  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  leds.piscarSetaDireita();
  leds.desligarSetaEsquerda();
  leds.desligarLanternas();
  leds.desligarFarois();
  Serial.println("Movendo lateralmente para a DIREITA (OMNI).");
}
void lateralEsquerda()
{
  int dir[4] = {
      -1, // M0
      1,  // M1
      1,  // M2
      -1  // M3
  };

  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  leds.piscarSetaEsquerda();
  leds.desligarLanternas();
  leds.desligarFarois();
  leds.desligarSetaDireita();
  Serial.println("Movendo lateralmente para a ESQUERDA (OMNI).");
}
void diagonalFrenteDireita()
{
  int dir[4] = {
      1,  // M0
      -1, // M1
      0,  // M2 (reduz para puxar diagonal)
      0   // M3
  };
  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  leds.piscarSetaDireita();
  leds.desligarSetaEsquerda();
  leds.desligarLanternas();
  leds.desligarFarois();
  Serial.println("Movimento diagonal frente-direita");
}
void diagonalFrenteEsquerda()
{
  int dir[4] = {
      0, // M0
      0, // M1
      1, // M2
      -1 // M3
  };
  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  leds.piscarSetaEsquerda();
  leds.desligarLanternas();
  leds.desligarFarois();
  leds.desligarSetaDireita();
  Serial.println("Movimento diagonal frente-esquerda");
}
void diagonalTrasDireita()
{
  int dir[4] = {
      0,
      1,
      0,
      -1};
  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  leds.piscarSetaDireita();
  leds.desligarSetaEsquerda();
  leds.desligarLanternas();
  leds.desligarFarois();
  Serial.println("Movimento diagonal trás-direita");
}
void diagonalTrasEsquerda()
{
  int dir[4] = {
      -1,
      0,
      1,
      0};
  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  leds.piscarSetaEsquerda();
  leds.desligarLanternas();
  leds.desligarFarois();
  leds.desligarSetaDireita();
  Serial.println("Movimento diagonal trás-esquerda");
}
void girarProprioEixo()
{
  int dir[4] = {
      1,  // Motor 0 para frente
      -1, // Motor 1 para trás
      1,  // Motor 2 para frente
      -1  // Motor 3 para trás
  };

  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  leds.piscarSetaDireita();
  leds.piscarSetaEsquerda();
  leds.desligarLanternas();
  leds.desligarFarois();
  Serial.println("Girando no próprio eixo para a direita");
}
void stop()
{
  int dir[4] = {
      0, // Motor 0 para frente
      0, // Motor 1 para frente
      0, // Motor 2 para trás
      0  // Motor 3 para trás
  };

  aplicarDirecao(dir);
  enviarJSONMotores(dir, velocidadeMax);
  leds.desligarSetaEsquerda();
  leds.desligarLanternas();
  leds.desligarFarois();
  leds.desligarSetaDireita();
  Serial.println("Girando no próprio eixo para a direita");
}
void parar()
{
  int dir[4] = {0, 0, 0, 0};
  aplicarDirecao(dir);
  enviarJSONMotores(dir, 0);
  leds.desligarSetaEsquerda();
  leds.desligarLanternas();
  leds.desligarFarois();
  leds.desligarSetaDireita();
  Serial.println(" Carrinho parado.");
}

void imagem()
{
  tft.fillScreen(TFT_BLACK);              // Fundo preto
  tft.pushImage(0, 0, 240, 240, katchau); // Mostra a imagem KATCHAU (se presente em img.h)
}

void limparTela()
{
  tft.fillScreen(TFT_BLACK);
}
void pararCarrinho()
{
  emCorrida = false;
  emAtrasoPartida = false;
  carrinho.controlarRodas(0.0f, 0.0f, 0.0f);
  Serial.println(">> PAROU (função)");
}
void enviarJSONMotores(int dir[4], int vel)
{
  if (!client.connected())
    reconnectMQTT();

  StaticJsonDocument<512> doc;

  doc["tipo"] = "status_completo";

  // ---------- MOTORES ----------
  JsonObject motores = doc.createNestedObject("motores");

  for (int i = 0; i < 4; i++)
  {
    String key = String("M") + String(i);
    JsonObject motor = motores.createNestedObject(key.c_str());
    motor["direcao"] = dir[i];
    motor["velocidade"] = vel;
  }

  // ---------- TEMPERATURAS ----------
  JsonObject temp = doc.createNestedObject("temperaturas");

  for (int i = 0; i < 4; i++)
  {
    int16_t leitura = ads.readADC_SingleEnded(i);
    float mV = leitura * 0.1875; // ADS1115 → mV
    float tempC = mV / 10.0;     // LM35 → °C

    String key = String("M") + String(i);
    temp[key] = tempC;
  }

  // ---------- TIMESTAMP ----------
  time_t now = time(nullptr);
  doc["timestamp"] = now;

  doc["source"] = "carrinho";

  // ---------- ENVIO ----------
  char buffer[512];
  size_t n = serializeJsonPretty(doc, buffer);
  client.publish(mqtt_topic_pub, buffer, n);

  Serial.print("📡 JSON Status Completo publicado: ");
  Serial.println(buffer);
}

void enviarPacoteCompleto()
{
  StaticJsonDocument<256> doc;
  doc["tipo"] = "status";
  doc["contA"] = contA;
  doc["contB"] = contB;
  doc["contC"] = contC;
  doc["contD"] = contD;
  doc["joystickX"] = ultimoX;
  doc["joystickY"] = ultimoY;
  doc["piscaDireita"] = piscaDireita;
  doc["piscaEsquerda"] = piscaEsquerda;
  doc["farolDianteiro"] = mcp.digitalRead(11) || mcp.digitalRead(12);
  doc["farolTraseiro"] = mcp.digitalRead(14) || mcp.digitalRead(15);

  char buffer[256];
  size_t n = serializeJson(doc, buffer);
  client.publish(mqtt_topic_pub, buffer, n);
}

// void temperaturaMotores()
// {
//   if (!client.connected())
//     reconnectMQTT();

//   StaticJsonDocument<256> doc;
//   doc["tipo"] = "temperatura_motores";

//   // ADS1115: cada LSB ≈ 0.1875 mV
//   // LM35: 10 mV = 1°C

//   for (int i = 0; i < 4; i++)
//   {
//     int16_t leitura = ads.readADC_SingleEnded(i);
//     float mV = leitura * 0.1875; // converte para mV
//     float tempC = mV / 10.0;     // LM35 -> graus Celsius

//     char key[8];
//     sprintf(key, "M%d", i);
//     doc[key] = tempC;
//   }

//   // Timestamp real
//   // time_t now = time(nullptr);
//   // doc["timestamp"] = now;
//   doc["source"] = "carrinho";

//   char buffer[256];
//   size_t n = serializeJson(doc, buffer);
//   client.publish(mqtt_topic_pub, buffer, n);

//   Serial.print("🌡 JSON Temperaturas publicado: ");
//   Serial.println(buffer);
// }

// void Leituraproximidade()
// {
//   if (!client.connected())
//     reconnectMQTT();

//   const int DISTANCIA_MINIMA_MM = 150; // limite de segurança (15 cm)

//   VL53L0X_RangingMeasurementData_t medida;
//   lox.rangingTest(&medida, false);

//   int distancia = -1;

//   if (medida.RangeStatus == 0) // leitura válida
//   {
//     distancia = medida.RangeMilliMeter;
//   }

//   // =============================
//   //      LÓGICA DE SEGURANÇA
//   // =============================
//   bool perigo = false;

//   if (distancia > 0 && distancia < DISTANCIA_MINIMA_MM)
//   {
//     perigo = true;

//     // PARA O CARRINHO IMEDIATAMENTE
//     int dir[4] = {0, 0, 0, 0};
//     aplicarDirecao(dir);
//     enviarJSONMotores(dir, 0);

//     Serial.println("🚨 Obstáculo detectado! Carrinho parado automaticamente.");
//   }

//   // =============================
//   //  ENVIO MQTT - PADRÃO DO CÓDIGO
//   // =============================
//   StaticJsonDocument<256> doc;
//   doc["tipo"] = "proximidade";
//   doc["distancia_mm"] = distancia;
//   doc["perigo"] = perigo; // true se houve parada automática

//   // time_t now = time(nullptr);
//   // doc["timestamp"] = now;
//   doc["source"] = "carrinho";

//   char buffer[256];
//   size_t n = serializeJson(doc, buffer);
//   client.publish(mqtt_topic_pub, buffer, n);

//   Serial.print("📏 JSON Proximidade publicado: ");
//   Serial.println(buffer);
// }

// ----------------------- Led RBG ------------------------
void acenderRGBvermelho()
{
  for (int i = 0; i < QTD_LEDS; i++)
  {
    fitaRGB.setPixelColor(i, fitaRGB.Color(255, 0, 0));
  }
  fitaRGB.show();
}

void acenderRGBbranco()
{
  for (int i = 0; i < QTD_LEDS; i++)
  {
    fitaRGB.setPixelColor(i, fitaRGB.Color(255, 255, 255));
  }
  fitaRGB.show();
}

void acenderRGBamarelo()
{
  for (int i = 0; i < QTD_LEDS; i++)
  {
    fitaRGB.setPixelColor(i, fitaRGB.Color(255, 255, 0));
  }
  fitaRGB.show();
}

void apagarRGB()
{
  for (int i = 0; i < QTD_LEDS; i++)
  {
    fitaRGB.setPixelColor(i, fitaRGB.Color(0, 0, 0)); // desligar
  }
  fitaRGB.show();
}



bool lerCaixas()
{
  Serial.println("Lendo o RFID");
  bool newCard = nfc.inListPassiveTarget(); // verifica se ha um novo cartao a ser lido
  Serial.println(newCard);
  for (int i = 0; i < 8; i++)
    uid[i] = 0;

  if (newCard)
  {
    Serial.println("Cartao detectado!");

    // leitura do cartao
    u_int8_t uidLength; // tamanho do ID lido

    sucess = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (sucess)
    {
      Serial.print("UID do cartao: ");

      for (uint8_t i = 0; i < uidLength; i++)
      {
        Serial.print(uid[i], HEX);
        Serial.print(" ");
      }

      Serial.println();
    }
    else
    {
      Serial.println("Falha ao ler do cartao");
    }
  }
  return sucess;
}

bool compararCartao(const uint8_t (&a)[8], const uint8_t (&b)[8])
{
  for (int i = 0; i < 8; i++)
  {
    Serial.print(a[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  for (int i = 0; i < 8; i++)
  {
    Serial.print(b[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  for (int i = 0; i < 8; i++)
  {
    if (a[i] != b[i])
    {
      return false;
    }
  }
  return true;
}

void atualizabotoes()
{
  if (contA != ultimoExecA)
  {
    if (contA)
      botaoAPressionado = 1;
    else
      botaoASolto = 1;
  }

  else
  {
    botaoAPressionado = 0;
    botaoASolto = 0;
  }

  if (contB != ultimoExecB)
  {
    if (contB)
      botaoBPressionado = 1;
    else
      botaoBSolto = 1;
  }

  else
  {
    botaoBPressionado = 0;
    botaoBSolto = 0;
  }

  if (contC != ultimoExecC)
  {
    if (contC)
      botaoCPressionado = 1;
    else
      botaoCSolto = 1;
  }

  else
  {
    botaoCPressionado = 0;
    botaoCSolto = 0;
  }

  if (contD != ultimoExecD)
  {
    if (contD)
      botaoDPressionado = 1;
    else
      botaoDSolto = 1;
  }

  else
  {
    botaoDPressionado = 0;
    botaoDSolto = 0;
  }

  if (contE != ultimoExecE)
  {
    if (contE)
      botaoEPressionado = 1;
    else
      botaoESolto = 1;
  }

  else
  {
    botaoEPressionado = 0;
    botaoESolto = 0;
  }

  if (contF != ultimoExecF)
  {
    if (contF)
      botaoFPressionado = 1;
    else
      botaoFSolto = 1;
  }

  else
  {
    botaoFPressionado = 0;
    botaoFSolto = 0;
  }

  if (contK != ultimoExecK)
  {
    if (contK)
      botaoKPressionado = 1;
    else
      botaoKSolto = 1;
  }

  else
  {
    botaoKPressionado = 0;
    botaoKSolto = 0;
  }

  ultimoExecA = contA;
  ultimoExecB = contB;
  ultimoExecC = contC;
  ultimoExecD = contD;
  ultimoExecE = contE;
  ultimoExecF = contF;
  ultimoExecK = contK;
}

void atualizaRGB()
{

  if (modoChase)
  {
    unsigned long agora = millis();
    if (agora - ultimoMovimento >= (unsigned long)intervaloChase)
    {
      ultimoMovimento = agora;

      posicaoChase++;
      if (posicaoChase >= QTD_LEDS)
        posicaoChase = 0;

      int contadorAux = 0;
      if(posicaoChase < 8) contadorAux = QTD_LEDS;

      fitaRGB.setPixelColor(posicaoChase + (contadorAux - 9), 0, 0, 0); //apaga o ultimo da sequencia

      // Acende o LED atual (vermelho)
      

      fitaRGB.setPixelColor(posicaoChase, 255, 0, 0);
      fitaRGB.setPixelColor(posicaoChase + (contadorAux - 1), 255, 0, 0);
      fitaRGB.setPixelColor(posicaoChase + (contadorAux - 2), 255, 0, 0);
      fitaRGB.setPixelColor(posicaoChase + (contadorAux - 3), 255, 165, 0);
      fitaRGB.setPixelColor(posicaoChase + (contadorAux - 4), 255, 165, 0);
      fitaRGB.setPixelColor(posicaoChase + (contadorAux - 5), 255, 255, 0);
      fitaRGB.setPixelColor(posicaoChase + (contadorAux - 6), 255, 255, 0);
      fitaRGB.setPixelColor(posicaoChase + (contadorAux - 7), 255, 255, 0);
      fitaRGB.setPixelColor(posicaoChase + (contadorAux - 8), 255, 255, 0);

      fitaRGB.show();
    }
  }

  else{
    apagarRGB();
  }
}


void atualizaLeituraCaixas()
{
   if (modoLeituraRFID == 1)
    {
      Serial.printf("selecionarCaixa = %d\t", selecionarCaixa);
      Serial.printf("leituraCaixa : %d\n", lerCaixas());

      if (selecionarCaixa == 1)
      {
        u_int8_t caixaA[8] = {0xC3, 0x8A, 0x14, 0x0C, 0X00, 0X00, 0x00, 0x00};
        u_int8_t caixaB[8] = {0x23, 0x80, 0xD9, 0x12, 0X00, 0X00, 0x00, 0x00};
        u_int8_t caixaC[8] = {0xC3, 0xED, 0xA5, 0xEE, 0X00, 0X00, 0x00, 0x00};
        if (lerCaixas())
        {

          if (compararCartao(uid, caixaA))
          {
            Serial.println("CAIXA A");
            acenderRGBamarelo();
          }

          else if (compararCartao(uid, caixaB))
          {
            Serial.println("CAIXA B");
            acenderRGBbranco();
          }
          else if (compararCartao(uid, caixaC))
          {
            Serial.println("CAIXA C");
            acenderRGBvermelho();
            Serial.println("LEU");
          }
        }
      }
    }
    else
    {
      apagarRGB();
    }
}
