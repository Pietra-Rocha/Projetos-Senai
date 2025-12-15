#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <senhas.h>
#include <internet.h>
#include <certificados.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>

const int mqttPort = 8883;
const char *mqtt_Topic_pub = "senai134/comandos";

WiFiClientSecure net;
PubSubClient client(net);
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define pinA 32
#define pinB 35
#define pinC 25
#define pinD 33
#define pinE 27
#define pinF 26
#define pinK 14

#define pinX 12
#define pinY 13

int ultimoA = 0, ultimoB = 0, ultimoC = 0, ultimoD = 0;
int ultimoE = 0, ultimoF = 0, ultimoK = 0;
int ultimoX = 0, ultimoY = 0;

unsigned long tempoWiFi = 0;
unsigned long tempoMQTT = 0;
unsigned long tempoLCD = 0;

String ultimaDirecao = "";
int ultimoLCD_X = -1;
int ultimoLCD_Y = -1;
String ultimaLinhaBotoes = "";

bool modoKatchau = false;
bool estadoRaio = false;
unsigned long tempoKatchau = 0;

void mostrarKatchau()
{
  if (millis() - tempoKatchau > 1500)
  {
    tempoKatchau = millis();
    estadoRaio = !estadoRaio;
  }

  lcd.clear();
  lcd.setCursor(3, 1);

  if (estadoRaio)
    lcd.print(" KATCHAU ");
  else
    lcd.print("  KATCHAU  ");
}

void conectarWiFi()
{
  Serial.print("Conectando ao Wi-Fi ");
  Serial.println(SSID);

  WiFi.begin(SSID, SENHA);

  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - tempoWiFi >= 500)
    {
      tempoWiFi = millis();
      Serial.print(".");
    }
  }

  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void conectarMQTT()
{
  while (!client.connected())
  {
    Serial.print("Conectando ao broker MQTT AWS...");

    if (client.connect("ESP32S3_Client"))
    {
      Serial.println("Conectado ao MQTT!");
    }
    else
    {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 3 segundos...");

      tempoMQTT = millis();
      while (millis() - tempoMQTT < 3000)
      {
      }
    }
  }
}

void enviarJSON(int A, int B, int C, int D, int E, int F, int K, int X, int Y)
{
  JsonDocument doc;

  doc["botaoA"] = A;
  doc["botaoB"] = B;
  doc["botaoC"] = C;
  doc["botaoD"] = D;
  doc["botaoE"] = E;
  doc["botaoF"] = F;
  doc["botaoK"] = K;

  JsonObject joystick = doc["joystick"].to<JsonObject>();
  joystick["X"] = X;
  joystick["Y"] = Y;

  String jsonString;
  serializeJson(doc, jsonString);

  client.publish(mqtt_Topic_pub, jsonString.c_str());
}

void setup()
{
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Direcao:");

  Serial.begin(9600);
  Serial.println("Iniciando sistema...");

  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(pinC, INPUT_PULLUP);
  pinMode(pinD, INPUT_PULLUP);
  pinMode(pinE, INPUT_PULLUP);
  pinMode(pinF, INPUT_PULLUP);
  pinMode(pinK, INPUT_PULLUP);

  conectarWiFi();

  net.setCACert(AWS_ROOT_CA);
  net.setCertificate(AWS_CERT);
  net.setPrivateKey(AWS_KEY);

  client.setServer(AWS_BROKER, mqttPort);
  client.setBufferSize(2048);

  conectarMQTT();

  Serial.println("Sistema pronto!");
}

void loop()
{
  if (!client.connected())
    conectarMQTT();
  client.loop();

  int A = !digitalRead(pinA);
  int B = !digitalRead(pinB);
  int C = !digitalRead(pinC);
  int D = !digitalRead(pinD);
  int E = !digitalRead(pinE);
  int F = !digitalRead(pinF);
  int K = !digitalRead(pinK);

  int X = analogRead(pinX);
  int Y = analogRead(pinY);

  if (abs(X - ultimoX) < 5)
    X = ultimoX;
  if (abs(Y - ultimoY) < 5)
    Y = ultimoY;

  if (A != ultimoA || B != ultimoB || C != ultimoC || D != ultimoD ||
      E != ultimoE || F != ultimoF || K != ultimoK ||
      abs(X - ultimoX) > 50 || abs(Y - ultimoY) > 50)
  {
    enviarJSON(A, B, C, D, E, F, K, X, Y);

    ultimoA = A;
    ultimoB = B;
    ultimoC = C;
    ultimoD = D;
    ultimoE = E;
    ultimoF = F;
    ultimoK = K;
    ultimoX = X;
    ultimoY = Y;
  }

  if (modoKatchau)
  {
    mostrarKatchau();

    if (A || B || C || D || E || F || abs(X - ultimoX) > 50 || abs(Y - ultimoY) > 50)
    {
      modoKatchau = false;
      lcd.clear();
    }
    return;
  }

  if (millis() - tempoLCD >= 200)
  {
    tempoLCD = millis();

    static bool estavaF = false; // detecta transição soltar/apertar

    if (F)
    {
      // Se acabou de apertar F, limpa a tela uma vez
      if (!estavaF)
      {
        lcd.clear();
        estavaF = true;
      }

      // Mostra ESPERANDO CARGA no centro
      lcd.setCursor(2, 0);
      lcd.print("ESPERANDO");
      lcd.setCursor(4, 1);
      lcd.print("CARGA");

      return; // Não deixa mostrar mais nada enquanto F está apertado
    }
    else
    {
      // Quando soltar o F → limpa a tela UMA VEZ e volta ao normal
      if (estavaF)
      {
        lcd.clear();
        estavaF = false;
      }
    }

    String direcao = "Centro";

    if (A)
      direcao = "Frente";
    else if (B)
      direcao = "Direita";
    else if (C)
      direcao = "Tras";
    else if (D)
      direcao = "Esquerda";
    else if (K)
    {
      direcao = "Botao K";
      modoKatchau = true;
      lcd.clear();
      return;
    }

    if (direcao != ultimaDirecao)
    {
      lcd.setCursor(0, 0);
      lcd.print("Direcao:         ");
      lcd.setCursor(9, 0);
      lcd.print(direcao + "   ");
      ultimaDirecao = direcao;
    }

    /*if (X != ultimoLCD_X)
    {
      lcd.setCursor(0, 1);
      lcd.print("X:    ");
      lcd.setCursor(3, 1);
      lcd.print(X);
      ultimoLCD_X = X;
    }
*/
    /*if (Y != ultimoLCD_Y)
    {
      lcd.setCursor(0, 2);
      lcd.print("Y:    ");
      lcd.setCursor(3, 2);
      lcd.print(Y);
      ultimoLCD_Y = Y;
    }
*/
    String linhaBotoes = "";
    if (A)
      linhaBotoes += "X ";
    if (B)
      linhaBotoes += "A ";
    if (C)
      linhaBotoes += "B ";
    if (D)
      linhaBotoes += "Y ";
    if (F)
      linhaBotoes += "F ";
    if (K)
      linhaBotoes += "K ";

    if (linhaBotoes != ultimaLinhaBotoes)
    {
      lcd.setCursor(0, 3);
      lcd.print("Botoes:         ");
      lcd.setCursor(8, 3);
      lcd.print(linhaBotoes);
      ultimaLinhaBotoes = linhaBotoes;
    }
  }
}