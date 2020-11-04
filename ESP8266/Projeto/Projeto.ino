#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <SPI.h>

#define I2CADDR 0x20

WiFiManager wifiManager;
uint8_t numID = 1;
bool gravar = false; //TX  RX
SoftwareSerial mySerial(14, 12);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
static const char ntpServidorNome[] = "us.pool.ntp.org";
const int fusoHorario = -3;
WiFiUDP Udp;
unsigned int localPort = 8888;
time_t prevDisplay = 0;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
const String host = "***.***.***.***";
const uint16_t port = 8080;
double valor = 100;
int p = -1;
const byte LINHAS = 4;
const byte COLUNAS = 4;
char teclaSelecionada = ' ';
char tecla;
bool cadastrar = false;
bool selecaoLetra = false;
bool pisca = false;
String nome = "";
char teclas[LINHAS][COLUNAS] =
    {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}};
byte pinosLinhas[LINHAS] = {0, 1, 2, 3};
byte pinosColunas[COLUNAS] = {4, 5, 6, 7};

byte setaBaixo[8] = {
    B00100,
    B00100,
    B00100,
    B00100,
    B00100,
    B10101,
    B01010,
    B00100};

byte setaCima[8] = {
    B00100,
    B01010,
    B10101,
    B00100,
    B00100,
    B00100,
    B00100,
    B00100};

Keypad_I2C kpd(makeKeymap(teclas), pinosLinhas, pinosColunas, LINHAS, COLUNAS, I2CADDR, PCF8574);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
time_t getNtpTime()
{
  IPAddress ntpServerIP;
  while (Udp.parsePacket() > 0)
    ;
  WiFi.hostByName(ntpServidorNome, ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500)
  {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE)
    {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + fusoHorario * SECS_PER_HOUR;
    }
  }
  return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void sendNTPpacket(IPAddress &address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket("pool.ntp.br", 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void displayDataHora()
{
  lcd.clear();
  pisca = !pisca;
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print(hour());
  if (pisca == true)
  {
    lcd.print(":");
  }
  else
  {
    lcd.print(" ");
  }
  String minutos = "" + minute();
  if (minute() < 10)
  {
    lcd.print("0");
  }
  lcd.print(minute());
  /*lcd.print(":");
  lcd.print(second());*/
  lcd.print(" ");
  if (day() < 10)
  {
    lcd.print("0");
  }
  lcd.print(day());
  lcd.print("/");
  if (month() < 10)
  {
    lcd.print("0");
  }
  lcd.print(month());
  lcd.print("/");
  lcd.print(year());
  lcd.setCursor(0, 1);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int getFingerprintIDez()
{
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)
  {
    return -1;
  }
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
  {
    return -1;
  }
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
    return -1;
  }
  WiFiClient client;
  client.setTimeout(10000);
  if (!client.connect(host, 8080))
  {
    Serial.println(("Connection failed"));
    return -1;
  }
  Serial.println(("Connected!"));
  client.print(("GET /espget/"));
  client.print((finger.fingerID));
  client.println((" HTTP/1.0"));
  client.println(("Host: ***.***.***.***"));
  client.println(("Connection: close"));
  if (client.println() == 0)
  {
    Serial.println(("Failed to send request"));
    return -1;
  }
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(("Unexpected response: "));
    Serial.println(status);
    return -1;
  }
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(("Invalid response"));
    return -1;
  }
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + 133;
  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, client);
  if (error)
  {
    Serial.print(("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return -1;
  }
  Serial.println(("Response:"));
  Serial.println(doc[0]["nome"].as<char *>());
  /*Serial.println(doc["time"].as<long>());
  Serial.println(doc["data"][0].as<float>(), 6);
  Serial.println(doc["data"][1].as<float>(), 6);*/
  lcd.clear();
  lcd.setCursor(0, 0);
  String palavra = doc[0]["nome"].as<char *>();
  lcd.print(palavra);
  int idPessoa = doc[0]["id"].as<int>();
  delay(3000);
  client.stop();
  client.setTimeout(10000);
  if (!client.connect(host, 8080))
  {
    Serial.println(("Connection failed"));
    return -1;
  }
  Serial.println(("Connected!"));
  client.print(("GET /esppost/"));
  client.print((idPessoa));
  client.println((" HTTP/1.0"));
  client.println(("Host: ***.***.***.***"));
  client.println(("Connection: close"));
  if (client.println() == 0)
  {
    Serial.println(("Failed to send request"));
    return -1;
  }
  status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(("Unexpected response: "));
    Serial.println(status);
    return -1;
  }
  char aux[] = "\r\n\r\n";
  if (!client.find(aux))
  {
    Serial.println(("Invalid response"));
    return -1;
  }
  client.stop();
  delay(800);
  /*for (int positionCounter = 0; positionCounter < palavra.length() - 16; positionCounter++)
  {
    lcd.scrollDisplayLeft();
    delay(800);
  }*/
  Serial.print("ID encontrado #");
  Serial.print(finger.fingerID);
  Serial.print(" com confiança de ");
  Serial.println(finger.confidence);
  delay(1000);
  return finger.fingerID;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t modoGravacaoID(uint8_t IDgravar)
{
  lcd.clear();
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("Esperando digital");
  lcd.print(IDgravar);
  p = -1;
  Serial.print("Esperando uma leitura válida para gravar #");
  Serial.println(IDgravar);
  delay(2000);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Leitura concluída");
      //delay(500);
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println(".");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Esperando digital");
      //delay(500);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro comunicação");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro comunicação");
      //delay(500);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de leitura");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro de leitura");
      //delay(500);
      break;
    default:
      Serial.println("Erro desconhecido");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro desconhecido");
      //delay(500);
      break;
    }
  }
  p = finger.image2Tz(1);
  switch (p)
  {
  case FINGERPRINT_OK:
    Serial.println("Leitura convertida");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Leitura convertida");
    delay(500);
    break;
  case FINGERPRINT_IMAGEMESS:
    Serial.println("Leitura suja");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Leitura suja");
    delay(500);
    return p;
  case FINGERPRINT_PACKETRECIEVEERR:
    Serial.println("Erro de comunicação");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro de comunicação");
    delay(500);
    return p;
  case FINGERPRINT_FEATUREFAIL:
    Serial.println("Não foi possível encontrar propriedade da digital");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Não foi possível encontrar propriedade da digital");
    delay(500);
    return p;
  case FINGERPRINT_INVALIDIMAGE:
    Serial.println("Não foi possível encontrar propriedade da digital");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Não foi possível encontrar propriedade da digital");
    delay(500);
    return p;
  default:
    Serial.println("Erro desconhecido");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro desconhecido");
    delay(500);
    return p;
  }
  Serial.println("Remova o dedo");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Remova o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER)
  {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(IDgravar);
  p = -1;
  Serial.println("Coloque o Mesmo dedo novamente");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Coloque o Mesmo dedo novamente");
  //delay(2000);
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Leitura concluída");
      delay(500);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print("Coloque o Mesmo dedo novamente");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Coloque o Mesmo dedo novamente");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro de comunicação");
      delay(500);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de Leitura");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro de Leitura");
      delay(500);
      break;
    default:
      Serial.println("Erro desconhecido");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro desconhecido");
      delay(500);
      break;
    }
  }
  p = finger.image2Tz(2);
  switch (p)
  {
  case FINGERPRINT_OK:
    Serial.println("Leitura convertida");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Leitura convertida");
    delay(500);
    break;
  case FINGERPRINT_IMAGEMESS:
    Serial.println("Leitura suja");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Leitura suja");
    delay(500);
    return p;
  case FINGERPRINT_PACKETRECIEVEERR:
    Serial.println("Erro de comunicação");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro de comunicação");
    delay(500);
    return p;
  case FINGERPRINT_FEATUREFAIL:
    Serial.println("Não foi possível encontrar as propriedades da digital");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Não foi possível encontrar as propriedades da digital");
    delay(500);
    return p;
  case FINGERPRINT_INVALIDIMAGE:
    Serial.println("Não foi possível encontrar as propriedades da digital");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Não foi possível encontrar as propriedades da digital");
    delay(500);
    return p;
  default:
    Serial.println("Erro desconhecido");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro desconhecido");
    delay(500);
    return p;
  }
  Serial.print("Criando modelo para #");
  Serial.println(IDgravar);
  p = finger.createModel();
  if (p == FINGERPRINT_OK)
  {
    Serial.println("As digitais batem!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("As digitais batem!");
    delay(500);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Erro de comunicação");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro de comunicação");
    delay(500);
    return p;
  }
  else if (p == FINGERPRINT_ENROLLMISMATCH)
  {
    Serial.println("As digitais não batem");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("As digitais não batem");
    delay(500);
    return p;
  }
  else
  {
    Serial.println("Erro desconhecido");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro desconhecido");
    delay(500);
    return p;
  }
  Serial.print("ID ");
  Serial.println(IDgravar);
  p = finger.storeModel(IDgravar);
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Armazenado!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Armazenado!");
    delay(500);
    //delay(800);
    //return p;
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Erro de comunicação");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro de comunicação");
    delay(500);
    return p;
  }
  else if (p == FINGERPRINT_BADLOCATION)
  {
    Serial.println("Não foi possível gravar neste local da memória");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Não foi possível gravar neste local da memória");
    delay(500);
    return p;
  }
  else if (p == FINGERPRINT_FLASHERR)
  {
    Serial.println("Erro durante escrita na memória flash");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro durante escrita na memória flash");
    delay(500);
    return p;
  }
  else
  {
    Serial.println("Erro desconhecido");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro desconhecido");
    delay(500);
    return p;
  }
  return p;
}

int cadastrarDigital(uint8_t IDgravar)
{
  WiFiClient client;
  char status[32] = {0};
  client.setTimeout(10000);
  if (!client.connect(host, 8080))
  {
    Serial.println(("Connection failed"));
    return -1;
  }
  int aux = IDgravar;
  Serial.println(("Connected!"));
  client.print(("GET /espcad/"));
  client.print((aux));
  client.println((" HTTP/1.0"));
  client.println(("Host: ***.***.***.***"));
  client.println(("Connection: close"));
  if (client.println() == 0)
  {
    Serial.println(("Failed to send request"));
    return -1;
  }
  //status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(("Unexpected response: "));
    Serial.println(status);
    return -1;
  }
  char endOfHeaders[] = "\r\n\r\n";
  //char aux[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(("Invalid response"));
    return -1;
  }
  delay(3000);
  client.stop();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup()
{
  Wire.begin();
  lcd.begin(16, 2);
  lcd.createChar(1, setaCima);
  lcd.createChar(2, setaBaixo);
  Serial.begin(9600);
  lcd.clear();
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  String texto = "CONECTE AO WI-FI ";
  lcd.print(texto);
  finger.begin(57600);
  kpd.begin(makeKeymap(teclas));
  //finger.emptyDatabase();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  //wifiManager.resetSettings();
  //WiFi.config(ip, gateway, subnet);
  wifiManager.autoConnect("MeuRelogioPonto");
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
  while (finger.verifyPassword() == false)
  {
    lcd.clear();
    lcd.setBacklight(HIGH);
    lcd.setCursor(0, 0);
    String texto = "Sensor biométrico não encontrado";
    lcd.print(texto);
    delay(800);
    for (int positionCounter = 0; positionCounter < texto.length() - 16; positionCounter++)
    {
      lcd.scrollDisplayLeft();
      delay(800);
    }
    for (int positionCounter = 0; positionCounter < texto.length() - 16; positionCounter++)
    {
      lcd.scrollDisplayLeft();
      delay(800);
    }
  }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop()
{
  finger.getTemplateCount();
  //Serial.println(finger.templateCount);
  if (WiFi.status() == WL_CONNECTED)
  {
    //digitalWrite(LED_BUILTIN, HIGH);
    tecla = kpd.getKey();
    if (timeStatus() != timeNotSet)
    {
      if (now() != prevDisplay)
      {
        prevDisplay = now();
        displayDataHora();
        //delay(10);
      }
      if (tecla == '1')
      {
        finger.getTemplateCount();
        finger.templateCount;
        if (finger.templateCount >= 162)
        {
          lcd.clear();
          lcd.setBacklight(HIGH);
          lcd.setCursor(0, 0);
          String texto = "SENSOR BIOMETRICO CHEIO";
          lcd.print(texto);
          delay(800);
          for (int positionCounter = 0; positionCounter < texto.length() - 16; positionCounter++)
          {
            lcd.scrollDisplayLeft();
            delay(800);
          }
        }
        else
        {
          finger.getTemplateCount();
          finger.templateCount;
          //modoGravacaoID(finger.templateCount);
          if (modoGravacaoID(finger.templateCount) == FINGERPRINT_OK)
          {
            cadastrarDigital(finger.templateCount);
          }
          //yield();
        }
      }
      getFingerprintIDez();
    }
    /*if (tecla == '4')
    {
      wifiManager.resetSettings();
      //digitalWrite(LED_BUILTIN, LOW);
      delay(50);
      lcd.clear();
      ESP.restart();
    }*/
  }
  else
  {
    lcd.clear();
    lcd.setBacklight(HIGH);
    lcd.setCursor(0, 0);
    //digitalWrite(LED_BUILTIN, LOW);
    String texto = " CONECTE AO WI-FI ";
    lcd.print(texto);
    delay(800);
    for (int positionCounter = 0; positionCounter < texto.length() - 16; positionCounter++)
    {
      lcd.scrollDisplayLeft();
      delay(800);
    }
  }
  yield();
}
