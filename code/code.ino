#include <NTP.h>
#include "time.h"
#include <ArduinoOTA.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>
#include <FS.h>
#include <NTPClient.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <WiFiManager.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP280.h>
//#include "EmonLib.h"           // USANDO PINO 36 NO SENSOR
#include <IRremote.hpp>          //INCLUSÃO DE BIBLIOTECA
#include <Ticker.h>


String VERSAO = "11.21 15/09/2023";

/*
 * VARIAVEIS DO SENSOR BMP280
 */
#define BMP_SCK (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS (10)

#define PIN_AP                0               //BOTÃO DE RESET DO WIFI
#define BUZZER                18              //SIRENE
#define VOLT_CAL              115.0           //VALOR DE CALIBRAÇÃO (DEVE SER AJUSTADO EM PARALELO COM UM MULTÍMETRO)

/*  INICIANDO TEMPORIZADORES TICKER
    VARIAVEIS DO AMBIENTE TICKER
 */
Ticker grava_leitura_dht;
Ticker grava_leitura_dht_0;
Ticker enviaGet_ticker;
int valor_grava_leitura_dht = 2500000;
int conta_temperatura_valor = 9;



/*
 * VARIAVEIS DE PARAMETRIZAÇÃO
 */
String logtxt =               "sim";          //GRAVAR LOG DE ERROS 
const char* nivelLog =        "4";            //DETALHAMENTO DE GRAVAÇÃO DO ERRO ARQUIVO JSON
//int nivel_log = 4;
const int sistema_solar =     1;              //DETALHAMENTO DE SENSORES NA PAGINA INICIAL
boolean ler_dht = true;                       //HABILITA LEITURA DO SENSOR DHT


/*
 * VARIAVEIS DO SENSOR DHT11 OU DHT22
 */
#define DHTPIN                19              //PINO ENTRADA SENSOR DHT11
#define DHTTYPE               DHT22           //TIPO DE SENSOR DHT22 OU DHT11
unsigned long timeDht; 
String temp_ext = "0.0";
String umid_ext = "00";
String glp_ = "0";
String fu_ = "0";
String nome_esp = "esp_none";
String tabela_sensores_ext = "0";
int contar_array_esp=0;

/*
 * VARIAVEIS DO SENSOR MQXX
 */
#define PIN_MQ2               34              //PINO ENTRADA SENSOR GAS
#define VRL_VALOR             5               //resistência de carga
#define RO_FATOR_AR_LIMPO     9.83            //resistência do sensor em ar limpo 9.83 de acordo com o datasheet
#define ITERACOES_CALIBRACAO  12              //numero de leituras para calibracao
#define ITERACOES_LEITURA     5               //numero de leituras para analise
#define GAS_LPG               0
#define GAS_CO                1
#define SMOKE                 2
float LPGCurve[3] =           { 2.3, 0.20, -0.47 };
float COCurve[3] =            { 2.3, 0.72, -0.34 };
float SmokeCurve[3] =         { 2.3, 0.53, -0.44 };
float Ro =                    10;
String GLP = "0", FUMACA = "0", CO = "0";
const char *LIMITE_MQ2 = "99", *LIMITE_MQ2_FU = "99";
int P_LEITURAS_MQ =           0;
unsigned long timeMq2;
int sensorMq2 =               0;
int contarParaGravar1 =       0;

int conta_temperatura = 0, contaLeituraDht = 0;
String temperatura[10];
String linha_tr_tabela;
float somaLeituraDht =        0.00;
String centrais_esp_valores[3][2];

/*
 * LEDS DE SINALIZAÇÃO
 */
#define LED_VERDE             15
#define LED_VERMELHO          4
#define LED_AZUL              2
const char interval =         500;  //VARIAVEL DO TEMPO DE INTERVALO DO PISCALED
long milis =                  0;


/*
 * VARIAVEIS DO SENSOR VS1868B Infravermelho
 */
#define IR_RECEIVE_PIN      17
#define ENABLE_LED_FEEDBACK LED_VERMELHO
int codigoControle[5] =     { 4077715200, 3877175040, 2707357440, 4144561920, 3810328320 };

/*
 * VARIAVEIS DE MATRIZ DE BOTÕES
 */
#define N_BOTAO             5
#define ENTRADA             35
int bt_select;
int bt_lido;
int bt_repete =             3;
int bt_conta;



/*
 * VARIAVEIS DOS BOTOES DE ENTRADA E DOS RELES
 */
struct botao1 {
  const short entrada = 32, rele = 33;
  boolean estado = 0, estado_atual = 0, estado_antes = 0;
  int contador = 0;
  const char* modelo = "interruptor";
  const char* nomeInter = "Com1";
  const char* tipo = "0";
  //const char* agenda_in="0000";
  //const char* agenda_out="0000";
  //const char* timer="0000";
  int pin_pir_1 = 32;
  int led_pir_1 = 2;
  unsigned long tempo_pir_1;
  bool conta_pir_1 = 0;
} botao1;

struct botao2 {
  const short entrada = 25, rele = 26;
  boolean estado = 0, estado_atual = 0, estado_antes = 0;
  int contador = 0;
  const char* modelo = "interruptor";
  const char* tipo = "0";
  const char* nomeInter = "Com2";
  //const char* agenda_in="0000";
  //const char* agenda_out="0000";
} botao2;

struct botao3 {
  const short entrada = 14, rele = 27;
  boolean estado = 0, estado_atual = 0, estado_antes = 0;
  int contador = 0;
  const char* tipo = "0";
  const char* modelo = "interruptor";
  const char* nomeInter = "Com3";
  //const char* agenda_in="0000";
  //const char* agenda_out="0000";
} botao3;

struct botao4 {
  const short entrada = 12, rele = 13;
  boolean estado = 0, estado_atual = 0, estado_antes = 0;
  int contador = 0;
  const char* tipo = "0";
  const char* modelo = "interruptor";
  const char* nomeInter = "Com4";
  //const char* agenda_in="0000";
  //const char* agenda_out="0000";
} botao4;

int i_timer_valor, estado_atual = 0, estado_antes = 0;
boolean cont_timer;
unsigned long tempo =           0;
short paramTempo =              60;
int nContar =                   0;
int agenda_ =                   0;

/*
 * VARIAVEIS DE REDE E WIFI
 */
String addressMac;
String ipLocalString;
const char *ssid, *password, *servidor;
String buff, URL, serv, buf;
#define portaServidor           80                  //PORTA DE COMUNICAÇÃO USADO NO WIFI

/*
 * CONTROLE DE ARQUIVO JSON
 */
const char* json;
int cont_ip_banco =             0;
const char* conslog =           "0";

int freq = 2000, channel = 0, resolution = 8, n = 0;
byte grau[8] = {
  B00001100,
  B00010010,
  B00010010,
  B00001100,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
};

Adafruit_BMP280 bmp;                      //  I2C Adafruit_BMP280

//EnergyMonitor emon1;                    //  CRIA UMA INSTÂNCIA

LiquidCrystal_I2C lcd(0x27, 16, 2);       //  FUNÇÃO DO TIPO "LiquidCrystal_I2C"

IPAddress ipHost;

WiFiUDP udp;
WiFiServer server(80);

const char* ntpServer =           "a.st1.ntp.br";
const long gmtOffset_sec =         0;
const int daylightOffset_sec =     -3 * 3600;


DHT_Unified dht(DHTPIN, DHTTYPE);



void setup() {

  Serial.begin(115200);
  Serial.println("-----------------------------------------");
  Serial.println("");

   /*
   * CONECTANDO A REDE WIFI
   * INICIANDO AS VARIAVEIS MAIS UTEIS NO SISTEMA
   */
  WiFiManager wifiManager;
  //wifiManager.setHostname("ESP_CENTRAL");
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  if (!wifiManager.autoConnect()) {
    Serial.println(" FALHA NA CONEXÃO! ");
    ESP.restart();
  }
  ipHost = WiFi.localIP();
  addressMac = WiFi.macAddress();
  gravarArquivo(" " + relogio_ntp(1) + " MAC: " + addressMac, "log.txt");
  ipLocalString = String(ipHost[0]) + "." + String(ipHost[1]) + "." + String(ipHost[2]) + "." + String(ipHost[3]);
  server.begin();

  /*
  * INICIANDO NTPClient PARA DATA E HORA NO ESP
  */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  /*  
   *  INICIALIZANDO PORTAS DE ENTRADA E SAIDA
   */
  pinMode(PIN_AP, INPUT_PULLUP);
  pinMode(botao1.rele, OUTPUT);
  pinMode(botao1.entrada, INPUT_PULLUP);
  //digitalWrite(botao1.rele, LOW);
  pinMode(botao2.rele, OUTPUT);
  pinMode(botao2.entrada, INPUT_PULLUP);
  //digitalWrite(botao2.rele, LOW);
  pinMode(botao3.rele, OUTPUT);
  pinMode(botao3.entrada, INPUT_PULLUP);
  //digitalWrite(botao3.rele, LOW);
  pinMode(botao4.rele, OUTPUT);
  pinMode(botao4.entrada, INPUT_PULLUP);
  //digitalWrite(botao4.rele, LOW);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  pinMode(LED_VERDE, OUTPUT);
  digitalWrite(LED_VERDE, LOW);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AZUL, OUTPUT);

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);  // Iniciar o receptor IR

  /*
   * INICIANDO SISTEMA DE ARQUIVOS 
   */
  openFS();
  //criarArquivo("/param.txt");
  criarArquivo("/log.txt");

  /*
   * INICIANDO SENSOR DHT
   */
  dht.begin();
  sensor_t sensor;

  /*
   * INICIANDO SENSOR DE TENSÃO ANALOGICO
   */
  //emon1.voltage(36, VOLT_CAL, 1.7);                    //PASSA PARA A FUNÇÃO OS PARÂMETROS (PINO ANALÓGIO / VALOR DE CALIBRAÇÃO / MUDANÇA DE FASE)
  /*
   * INICIANDO SENSOR BMP280
   */
  if (!bmp.begin(0x76)) {
    gravarArquivo(" " + relogio_ntp(1) + " - BMP280 não encontrado, verifique I2C!", "log.txt");
  }
  // Default settings from datasheet
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  // ----------------------------------------------

  calibrarSensor();

  arduino_ota();

  ArduinoOTA.begin();

  //---------------------------------------
  //    INICIALIZA O DISPLAY LCD
  //---------------------------------------
  // lcd.init();                           // INICIALIZA O DISPLAY LCD
  // lcd.setBacklight(HIGH);
  // lcd.createChar(0, grau);
  // ledcSetup(channel, freq, resolution);
  // ledcAttachPin(5, channel);
  // lcd.setCursor(0, 0);
  // lcd.print("    BEM VINDO   ");        //SETA A POSIÇÃO EM QUE O CURSOR INCIALIZA(LINHA 1)
  // lcd.setCursor(0, 1);
  // lcd.print(VERSAO);                    //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)
  // digitalWrite(BUZZER, HIGH);
  // delay(500);
  // digitalWrite(BUZZER, LOW);
  // delay(2500);
  // lcd.clear();
  // lcd.setCursor(2, 1);
  // lcd.print(ipLocalString);             //SETA A POSIÇÃO EM QUE O CURSOR RECEBE O TEXTO A SER MOSTRADO(LINHA 2)
  // delay(2500);
  // lcd.clear();

  grava_leitura_dht_0.once_ms(1000, gravaDhtArray);
  
  grava_leitura_dht.attach_ms(valor_grava_leitura_dht, gravaDhtArray);            //GRAVA NO ARRAY OS VALORES DE TEMPERATURA E UMIDADE NO ARRAY
  
  enviaGet_ticker.attach_ms(15000, enviaGet);

  Serial.println("Hostname: "+String(WiFi.getHostname()));
  
  gravarArquivo("\n\n +++ INICIANDO SISTEMA +++ Versão: " + VERSAO + "\n\n", "log.txt");

}
void loop() {

  ArduinoOTA.handle();

  WiFiManager wifiManager;

  pisca_led(2, true);

  //capturaIr();

  while (cont_ip_banco < 1) {
    // FAZENDO LEITURA DE PARAMETROS DO SISTEMA
    openFS();
    StaticJsonDocument<700> doc;
    json = lerArquivoParam().c_str();
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
      gravaLog(" " + relogio_ntp(1) + " - E0101:Arquivo json: ", logtxt, 1);
      Serial.println(error.c_str());
      //gravarArquivo("{\"servidor\":\"" + String(ipHost) + "\",\"int_1\":\"R1\",\"tipo_1\":\"0\",\"sinal_1\":\"pulso\",\"h_i_1\":\"0000\",\"h_o_1\":\"0000\",\"int_2\":\"R2\",\"tipo_2\":\"0\",\"sinal_2\":\"pulso\",\"h_i_2\":\"0000\",\"h_o_2\":\"0000\",\"int_3\":\"R3\",\"tipo_3\":\"0\",\"sinal_3\":\"pulso\",\"h_i_3\":\"0000\",\"h_o_3\":\"0000\",\"int_4\":\"R4\",\"tipo_4\":\"0\",\"sinal_4\":\"pulso\",\"h_i_4\":\"0000\",\"h_o_4\":\"0000\",\"log\":\"sim\",\"nivel\":\"4\",\"v_mq\":\"20\",\"v_mq_fu\":\"50\",\"s_a\":\"123\"}", "param.txt");
      gravarArquivo("{\"servidor\":\"0\",\"int_1\":\"R1\",\"tipo_1\":\"0\",\"sinal_1\":\"pulso\",\"h_i_1\":\"0000\",\"h_o_1\":\"0000\",\"int_2\":\"R2\",\"tipo_2\":\"0\",\"sinal_2\":\"pulso\",\"h_i_2\":\"0000\",\"h_o_2\":\"0000\",\"int_3\":\"R3\",\"tipo_3\":\"0\",\"sinal_3\":\"pulso\",\"h_i_3\":\"0000\",\"h_o_3\":\"0000\",\"int_4\":\"R4\",\"tipo_4\":\"0\",\"sinal_4\":\"pulso\",\"h_i_4\":\"0000\",\"h_o_4\":\"0000\",\"log\":\"sim\",\"nivel\":\"4\",\"v_mq\":\"999\",\"v_mq_fu\":\"999\"}", "param.txt");

      cont_ip_banco++;
      return;
    }
    JsonObject root = doc.as<JsonObject>();
    servidor = root["sv"];
    botao1.nomeInter = root["i_1"];
    botao1.tipo = root["ti_1"];
    botao1.modelo = root["s_1"];
    //botao1.agenda_in = root["h_i_1"];
    //botao1.agenda_out = root["h_o_1"];
    //botao1.timer = root["t_1"];
    botao2.nomeInter = root["i_2"];
    botao2.tipo = root["ti_2"];
    botao2.modelo = root["s_2"];
    //botao2.agenda_in = root["h_i_2"];
    //botao2.agenda_out = root["h_o_2"];
    botao3.nomeInter = root["i_3"];
    botao3.tipo = root["ti_3"];
    botao3.modelo = root["s_3"];
    //botao3.agenda_in = root["h_i_3"];
    //botao3.agenda_out = root["h_o_3"];
    botao4.nomeInter = root["i_4"];
    botao4.tipo = root["ti_4"];
    botao4.modelo = root["s_4"];
    //botao4.agenda_in = root["h_i_4"];
    //botao4.agenda_out = root["h_o_4"];
    conslog = root["l"];
    logtxt = String(conslog);
    nivelLog = root["n"];
    LIMITE_MQ2 = root["mq"];
    LIMITE_MQ2_FU = root["fu"];
    cont_ip_banco++;
    serv = String(servidor);
    gravaLog(" " + relogio_ntp(1) + " - BD:" + String(servidor), logtxt, 2);
    doc.clear();
    root.clear();
  }
  /*
   * LEITURA DO TELADO DE 5 BOTOES
   */
  //  for (int i = 0; i < N_BOTAO; i++) {
  //    if (analogRead_bt(i)) {
  //      bt_lido = i;
  //      if(bt_repete == bt_lido){
  //        if(bt_lido == 3){
  //          bt_conta++;
  //          delay(200);
  //          bt_repete = bt_lido;
  //        }
  //        if(bt_conta > 2){
  //          bt_conta = 0;
  //        }
  //      }
  //    }
  //  }
  //  lcd_temp_umid();
  //  switch (bt_lido) {
  //  case 0:
  //    lcd.clear();
  //    lcd_temp_umid();
  //    break;
  //  case 1:
  //
  //    lcd.clear();
  //    lcd_mq();
  //    break;
  //  default:
  //    // comando(s)
  //    break;
  //}
  /*
   * LEITURA DE COMANDOS TRANSMITIDO PELA SERIAL
   */
  WiFiClient client = server.available();
  if (Serial.available() > 0) {
    // Lê toda string recebida
    String recebido = leStringSerial();
    gravaLog(" " + relogio_ntp(1) + " - Serial: " + recebido, logtxt, 2);

    /*
       EXEMPLO DE DADOS RECEBIDO NA SERIAL:
       sc_1=1.20&sv_1=12&sc_2=1.30&sv_2=13
    */
    /*     if (quebraString( "sc_1", recebido))
        {
          corrente_s1 = quebraString( "sc_1", recebido).toFloat();
        }
        if (quebraString( "sv_1", recebido))
        {
          tensao_s1 = quebraString( "sv_1", recebido).toFloat();
        }
        if (quebraString( "sc_2", recebido))
        {
          corrente_s2 = quebraString( "sc_2", recebido).toFloat();
        }
        if (quebraString( "sv_2", recebido))
        {
          tensao_s2 = quebraString( "sv_2", recebido).toFloat();
        }
        if (quebraString( "sc_3", recebido))
        {
          corrente_s3 = quebraString( "sc_3", recebido).toFloat();
        }
        if (quebraString( "sv_3", recebido))
        {
          tensao_s3 = quebraString( "sv_3", recebido).toFloat();
        } */
    if (quebraString("rede", recebido) == "1") {
      printWifiData();
    }
    recebido.remove(0);
  }

  /*
    ---------------------------------------
     ENTRADA E SAIDA DE GPIO 1
    ---------------------------------------
    --------------------------------------
     INICIO DA FUNÇÃO AGENDAMENTO
    --------------------------------------
  */
  // if ((relogio_ntp(3) == botao1.agenda_in) && (botao1.estado == false)) {
  //   acionaPorta(botao1.rele, "", "liga");
  //   botao1.estado = true;
  // }
  // if ((relogio_ntp(3) == botao1.agenda_out) && (botao1.estado == true)) {
  //   acionaPorta(botao1.rele, "", "desl");
  //   botao1.estado = false;
  // }
  /*
    ------------------------------------------------
    FIM DA FUNÇÃO AGENDAMENTO
    ------------------------------------------------
    ------------------------------------------------
    VERIFICA SE EXISTE VALOR DE TIMER CONFIGURADO
    ------------------------------------------------
  */
  // String s_timer_valor = String(botao1.timer);
  // if (s_timer_valor.toInt() > 0) {
  //   if (cont_timer == 1) {
  //     i_timer_valor = s_timer_valor.toInt();
  //     cont_timer = 0;
  //   }
  //   if (botao1.estado) {
  //     int i_timer = i_timer_valor--;
  //     Serial.println("timer configurado : " + String(i_timer));
  //     delay(1000);
  //     if (i_timer_valor == 0) {
  //       acionaPorta(botao1.rele, "", "desl");
  //       cont_timer = 0;
  //     }
  //   }
  // }
  /*
    ---------------------------------------
    INICIO DA FUNÇÃO BOTÃO POR PULSO
    ---------------------------------------
  */
  if (String(botao1.modelo) == "pulso") {
    if (digitalRead(botao1.entrada) == (String(botao1.tipo).toInt())) {
      if (nContar == 0) Serial.println("\n");
      Serial.println("\n E1 Pulso");
      while ((digitalRead(botao1.entrada) == (String(botao1.tipo).toInt())) && (nContar <= 300)) {
        if (millis() >= tempo + paramTempo) {
          botao1.contador++;
          nContar++;
          Serial.print(botao1.contador, DEC);
          tempo = millis();
        }
      }
    }
    /*
      ---------------------------------------
      FIM DA FUNÇÃO BOTÃO POR PULSO
      ---------------------------------------
      ---------------------------------------
      INICIO DA FUNÇÃO BOTÃO POR INTERRUPTOR
      ---------------------------------------
    */
  } else if (String(botao1.modelo) == "interruptor") {
    botao1.estado_atual = digitalRead(botao1.entrada);
    if (botao1.estado_atual != botao1.estado_antes) {
      if (nContar == 0) Serial.println(" E1 Inter");
      botao1.estado_antes = botao1.estado_atual;
      botao1.contador = 3;
    }
  }
  /*
    ---------------------------------------
    FIM DA FUNÇÃO BOTÃO POR INTERRUPTOR
    ---------------------------------------
    ---------------------------------------
    INICIO DA FUNÇÃO POR PRESENÇA
    ---------------------------------------
  */
  if (String(botao1.modelo) == "pir") {
    bool isDetected = digitalRead(botao1.pin_pir_1);

    //Verifica se o sensor detectou presença, se detectado, altera
    //  variavel de contagem e liga led do pir
    if (isDetected) {
      botao1.tempo_pir_1 = millis();
      digitalWrite(botao1.led_pir_1, HIGH);
      if (botao1.conta_pir_1 == 0) {
        gravaLog(" " + relogio_ntp(1) + " - Prensença Detectada...", logtxt, 4);
        botao1.conta_pir_1 = 1;
        if (botao1.estado == false) {
          botao1.estado = true;
          acionaPorta(botao1.rele, "", "liga");
        }
      }
    } else {
      digitalWrite(botao1.led_pir_1, LOW);
    }
    //Variavel de contagem
    if (botao1.conta_pir_1 == 1) {
      //Se passar X' segundos muda variavel de contagem e desliga o led do pir
      if ((millis() - botao1.tempo_pir_1) >= 1200000) {
        botao1.tempo_pir_1 = millis();
        botao1.conta_pir_1 = 0;
        int temp_pir = botao1.tempo_pir_1 / 1000;
        gravaLog(" " + relogio_ntp(1) + " - " + temp_pir + " s", logtxt, 4);
        digitalWrite(botao1.led_pir_1, LOW);
        if (botao1.estado == true) {
          botao1.estado = false;
          acionaPorta(botao1.rele, "", "desl");
        }
      }
    }

    /*
      ---------------------------------------
      FIM DA FUNÇÃO BOTÃO PRESENÇA
      ---------------------------------------
      ---------------------------------------
      ACIONAMENTO APÓS FUNÇÕES ACIMA
      ---------------------------------------
    */
  }
  if ((botao1.contador >= 1) && (botao1.contador <= 9)) {
    nContar = 0;
    if (botao1.estado == false) {
      botao1.estado = true;
      botao1.contador = 0;
      acionaPorta(botao1.rele, "", "liga");
    } else {
      acionaPorta(botao1.rele, "", "desl");
      botao1.estado = false;
      botao1.contador = 0;
    }
  }
  /*
    ---------------------------------------
    FIM DA FUNÇÃO BOTÃO PRESENÇA
    ---------------------------------------
    ---------------------------------------
    FIM ENTRADA E SAIDA DE GPIO 1
    ---------------------------------------
  */

  /*
    ---------------------------------------
    ENTRADA E SAIDA DE GPIO 2
    ---------------------------------------
    --------------------------------------
    INICIO DA FUNÇÃO AGENDAMENTO
    --------------------------------------
  */
  // if ((relogio_ntp(3) == botao2.agenda_in) && (botao2.estado == false)) {
  //   acionaPorta(botao2.rele, "", "liga");
  //   botao2.estado = true;
  // }
  // if ((relogio_ntp(3) == botao2.agenda_out) && (botao2.estado == true)) {
  //   acionaPorta(botao2.rele, "", "desl");
  //   botao2.estado = false;
  // }
  /*
    ---------------------------------------
    FIM DA FUNÇÃO AGENDAMENTO
    ---------------------------------------
    ---------------------------------------
    VERIFICA SE EXISTE VALOR DE TIMER CONFIGURADO
    ---------------------------------------
  */

  /*
    ---------------------------------------
    INICIO DA FUNÇÃO BOTÃO POR PULSO
    ---------------------------------------
  */
  if (String(botao2.modelo) == "pulso") {
    if (digitalRead(botao2.entrada) == (String(botao2.tipo).toInt())) {
      if (nContar == 0) Serial.println("\n");
      Serial.println(" E2 Pulso");
      while ((digitalRead(botao2.entrada) == (String(botao2.tipo).toInt())) && (nContar <= 300)) {
        if (millis() >= tempo + paramTempo) {
          botao2.contador++;
          nContar++;
          Serial.print(botao2.contador, DEC);
          tempo = millis();
        }
      }
    }
  }
  /*
     ---------------------------------------
     FIM DA FUNÇÃO BOTÃO POR PULSO
     ---------------------------------------
     ---------------------------------------
     INICIO DA FUNÇÃO BOTÃO POR INTERRUPTOR
     ---------------------------------------
  */
  if (String(botao2.modelo) == "interruptor") {
    botao2.estado_atual = digitalRead(botao2.entrada);
    if (botao2.estado_atual != botao2.estado_antes) {
      if (nContar == 0) Serial.println("\n");
      Serial.println(" E2 Inter ");
      botao2.estado_antes = botao2.estado_atual;
      botao2.contador = 3;
      //Serial.print(botao2.contador, DEC);
    }
  }
  /*
    ---------------------------------------
    FIM DA FUNÇÃO BOTÃO POR INTERRUPTOR
    ---------------------------------------
    ---------------------------------------
    INICIO DA FUNÇÃO POR PRESENÇA
    ---------------------------------------
  */

  /*
     ---------------------------------------
     FIM DA FUNÇÃO BOTÃO PRESENÇA
     ---------------------------------------
     ---------------------------------------
     ACIONAMENTO APÓS FUNÇÕES ACIMA
     ---------------------------------------
  */
  if ((botao2.contador >= 1) && (botao2.contador <= 9)) {
    nContar = 0;
    if (botao2.estado == false) {
      botao2.estado = true;
      botao2.contador = 0;
      acionaPorta(botao2.rele, "", "liga");
    } else {
      acionaPorta(botao2.rele, "", "desl");
      botao2.estado = false;
      botao2.contador = 0;
    }
  }
  /*
    ---------------------------------------
    FIM ENTRADA E SAIDA DE GPIO 2
    ---------------------------------------
  */

  /*
     ---------------------------------------
     ENTRADA E SAIDA DE GPIO 3
     ---------------------------------------
     --------------------------------------
     INICIO DA FUNÇÃO AGENDAMENTO
     --------------------------------------
  */

  /*
     FIM DA FUNÇÃO AGENDAMENTO

      VERIFICA SE EXISTE VALOR DE TIMER CONFIGURADO
  */

  /*
    ---------------------------------------
    INICIO DA FUNÇÃO BOTÃO POR PULSO
    ---------------------------------------
  */
  if (String(botao3.modelo) == "pulso") {
    if (digitalRead(botao3.entrada) == (String(botao3.tipo).toInt())) {
      if (nContar == 0) Serial.println("\n");
      Serial.println(" E3 Pulso");
      while ((digitalRead(botao3.entrada) == (String(botao3.tipo).toInt())) && (nContar <= 300)) {
        if (millis() >= tempo + paramTempo) {
          botao3.contador++;
          nContar++;
          Serial.print(botao3.contador, DEC);
          tempo = millis();
        }
      }
    }
  }
  /*
     ---------------------------------------
     FIM DA FUNÇÃO BOTÃO POR PULSO
     ---------------------------------------
     ---------------------------------------
     INICIO DA FUNÇÃO BOTÃO POR INTERRUPTOR
     ---------------------------------------
  */
  if (String(botao3.modelo) == "interruptor") {
    botao3.estado_atual = digitalRead(botao3.entrada);
    if (botao3.estado_atual != botao3.estado_antes) {
      if (nContar == 0) Serial.println("\n");
      Serial.print(" E3 Inter");
      botao3.estado_antes = botao3.estado_atual;
      botao3.contador = 3;
      //Serial.print(botao3.contador, DEC);
    }
  }
  /*
     ---------------------------------------
     FIM DA FUNÇÃO BOTÃO POR INTERRUPTOR
     ---------------------------------------
     ---------------------------------------
     INICIO DA FUNÇÃO POR PRESENÇA
     ---------------------------------------
  */


  /*
     ---------------------------------------
     FIM DA FUNÇÃO BOTÃO PRESENÇA
     ---------------------------------------
     ---------------------------------------
     ACIONAMENTO APÓS FUNÇÕES ACIMA
     ---------------------------------------
  */
  if ((botao3.contador >= 2) && (botao3.contador <= 9)) {
    nContar = 0;
    if (botao3.estado == false) {
      botao3.estado = true;
      botao3.contador = 0;
      acionaPorta(botao3.rele, "", "liga");
    } else {
      acionaPorta(botao3.rele, "", "desl");
      botao3.estado = false;
      botao3.contador = 0;
    }
  }
  /*
    ---------------------------------------
    FIM ENTRADA E SAIDA DE GPIO 3
    ---------------------------------------
  */

  /*
    ---------------------------------------
    ENTRADA E SAIDA DE GPIO 4
    ---------------------------------------
    --------------------------------------
    INICIO DA FUNÇÃO AGENDAMENTO
    --------------------------------------
  */

  /*
    --------------------------------------
    FIM DA FUNÇÃO AGENDAMENTO
    --------------------------------------
    --------------------------------------
    VERIFICA SE EXISTE VALOR DE TIMER CONFIGURADO
    --------------------------------------
  */

  /*
    ---------------------------------------
    INICIO DA FUNÇÃO BOTÃO POR PULSO
    ---------------------------------------
  */
  if (String(botao4.modelo) == "pulso") {
    if (digitalRead(botao4.entrada) == (String(botao4.tipo).toInt())) {
      if (nContar == 0) Serial.println("\n");
      Serial.println("\n E4 Pulso");
      while ((digitalRead(botao4.entrada) == (String(botao4.tipo).toInt())) && (nContar <= 300)) {
        if (millis() >= tempo + paramTempo) {
          botao4.contador++;
          nContar++;
          Serial.print(botao4.contador, DEC);
          tempo = millis();
        }
      }
    }
  }
  /*
    ---------------------------------------
    FIM DA FUNÇÃO BOTÃO POR PULSO
    ---------------------------------------
    ---------------------------------------
    INICIO DA FUNÇÃO BOTÃO POR INTERRUPTOR
    ---------------------------------------
  */
  if (String(botao4.modelo) == "interruptor") {
    botao4.estado_atual = digitalRead(botao4.entrada);
    if (botao4.estado_atual != botao4.estado_antes) {
      if (nContar == 0) Serial.println(" E4 Inter");
      botao4.estado_antes = botao4.estado_atual;
      botao4.contador = 3;
      //Serial.print(botao4.contador, DEC);
    }
  }
  /*
    ---------------------------------------
    FIM DA FUNÇÃO BOTÃO POR INTERRUPTOR
    ---------------------------------------
    ---------------------------------------
    INICIO DA FUNÇÃO POR PRESENÇA
    ---------------------------------------
  */
  /*
    ---------------------------------------
    FIM DA FUNÇÃO BOTÃO PRESENÇA
    ---------------------------------------
    ---------------------------------------
    ACIONAMENTO APÓS FUNÇÕES ACIMA
    ---------------------------------------
  */
  if ((botao4.contador >= 2) && (botao4.contador <= 9)) {
    nContar = 0;
    if (botao4.estado == false) {
      botao4.estado = true;
      botao4.contador = 0;
      acionaPorta(botao4.rele, "", "liga");
    } else {
      acionaPorta(botao4.rele, "", "desl");
      botao4.estado = false;
      botao4.contador = 0;
    }
  }
  /*
    ---------------------------------------
    FIM ENTRADA E SAIDA DE GPIO 4
    ---------------------------------------
  */

  /*
     APAGAR TODAS AS PORTAS DE SAIDA
  */
  if ((botao1.contador >= 10)
      || (botao2.contador >= 10)
      || (botao3.contador >= 10)
      || (botao4.contador >= 10)) {
    gravaLog(" " + relogio_ntp(1) + "\n - DESL. RELES", logtxt, 2);
    acionaPorta(botao1.rele, "", "desl");
    botao1.estado = false;
    acionaPorta(botao2.rele, "", "desl");
    botao2.estado = false;
    acionaPorta(botao3.rele, "", "desl");
    botao3.estado = false;
    acionaPorta(botao4.rele, "", "desl");
    botao4.estado = false;
    botao4.contador = 0;
    botao3.contador = 0;
    botao2.contador = 0;
    botao1.contador = 0;
  }

  /*
    ---------------------------------------
    LEITURA DA REQUISIÇÕES DE CHAMADAS HTTP GET
    ---------------------------------------
  */
  if (client) {
    URL = "";
    URL = client.readStringUntil('\r');
  } else {
    URL = "vazio";
  }

  if (URL != "vazio") {
    /*
      ------------------------------------------------------------------------------
      EXEMPLO NA CHAMADA WEB DESLIGAR LAMPADA -
      CHAMADA HTTP EX: HTTP://IP_HOST/?porta=20&acao=desligar&central=IP_HOST
      ------------------------------------------------------------------------------
    */
    String stringUrl = URL;
    gravaLog(" " + relogio_ntp(1) + " - " + String(URL), logtxt, 4);
    URL = "";
    String requisicao = stringUrl.substring(6, 11);
    if (requisicao == "porta") {
      String numero = stringUrl.substring(12, 14);
      String acao = stringUrl.substring(20, 24);
      String central = stringUrl.substring(33, 40);
      int numeroInt = numero.toInt();
      nContar = 0;
      n = 0;
      acionaPorta(numeroInt, requisicao, acao);
      if (numeroInt == botao1.rele) {
        if (acao == "liga") {
          botao1.estado = true;
        } else {
          botao1.estado = false;
          botao1.conta_pir_1 = 0;
        }
      }
      if (numeroInt == botao2.rele) {
        if (acao == "liga") {
          botao2.estado = true;
        } else {
          botao2.estado = false;
        }
      }
      if (numeroInt == botao3.rele) {
        if (acao == "liga") {
          botao3.estado = true;
        } else {
          botao3.estado = false;
        }
      }
      if (numeroInt == botao4.rele) {
        if (acao == "liga") {
          botao4.estado = true;
        } else {
          botao4.estado = false;
        }
      }
    }

    /*
      ------------------------------------------------------------------------------
      REINCIAR CENTRAL POR COMANDA HTTP - CHAMADA HTTP EX: HTTP://IP_HOST/?00000
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00000") {
      gravaLog(" " + relogio_ntp(1) + " - REINICIANDO...", logtxt, 1);
      client.println("HTTP/1.1 200 OK");          //ESCREVE PARA O CLIENTE A VERSÃO DO HTTP
      client.println("Content-Type: text/html");  //ESCREVE PARA O CLIENTE O TIPO DE CONTEÚDO(texto/html)
      client.println("");
      client.println("<!DOCTYPE HTML>");  //INFORMA AO NAVEGADOR A ESPECIFICAÇÃO DO HTML
      client.println("<body style=\"background-color: rgb(255, 255, 204);\"><div style=\"text-align: center;\"><p><strong style=\"font-size: 24px;\">Reiniciando Sistema!</strong></p></div><div style=\"text-align: center;\"><strong>Clique em Voltar no navegador para Retornar a Pagina.</strong></div><div style=\"text-align: center;\">");
      client.println("<strong style=\"font-size: 24px;\"><img alt=\"\" src=\"https://olgacolor.com.br/wp-content/uploads/2021/01/loading-gears-animation-13-3.gif\" style=\"width: 40px; height: 40px;\" /></strong></div><div style=\"text-align: center;\"></div></body>");
      client.println("</html>");
      client.stop();
      delay(5000);
      ESP.restart();
    }
    if (requisicao == "00001") {
      Serial.println(stringUrl);
    }

    /*
      ------------------------------------------------------------------------------
      SE FOR PRESSIONADO BOTÃO BOOT na placa, TODAS AS CONFIGURAÇÕES DA CENTRAL
      SERÃO DELETADAS(WIFI, PARAMETROS, ETC). VARIAVEL DO BOTÃO É PIN_AP.
      ------------------------------------------------------------------------------
    */
    if (digitalRead(PIN_AP) == LOW || requisicao == "00002")
      if (requisicao == "00002") {

        /*
        ------------------------------------------------------------------------------
        Apagando dados de conexão WIFI da central
        ------------------------------------------------------------------------------
      */
        esp_wifi_restore();
        gravaLog(" " + relogio_ntp(1) + " - Apagando config WIFI", logtxt, 1);
        if (!wifiManager.startConfigPortal(" WIFI_AUT", "12345678")) {
          gravaLog(" " + relogio_ntp(1) + " - E0102: Modo AP", logtxt, 1);
          ESP.restart();
        }
      }
    /*
      ------------------------------------------------------------------------------
      CALIBRAR SENSOR MQ2
      CHAMADA HTTP EX: HTTP://IP_HOST/?00010
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00010") {
      gravaLog(" " + relogio_ntp(1) + " - Recalibrando...", logtxt, 2);
      calibrarSensor();
    }
    /*
      ------------------------------------------------------------------------------
      APAGAR ARQUIVO DE LOG MANUALMENTE
      CHAMADA HTTP EX: HTTP://IP_HOST/?00013
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00013") {
      deletarArquivo("/log.txt");
      criarArquivo("/log.txt");
      gravaLog(" " + relogio_ntp(1) + " - Apagando log...", logtxt, 2);
    }
    /*
      ------------------------------------------------------------------------------
      APLICAR CONFIGURAÇÕES MINIMAS PARA FUNCIONAMENTO DA CENTRAL
      CHAMADA HTTP EX: HTTP://IP_HOST/?00014
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00014") {
      openFS();
      listDir(SPIFFS, "/", 0);
      deletarArquivo("/param.txt");
      criarArquivo("/param.txt");
      //gravaLog(" " + relogio_ntp(1) + " - Configuração minima ", logtxt, 3);
      gravarArquivo("{\"servidor\":\"" + String(ipHost) + "\",\"int_1\":\"R1\",\"tipo_1\":\"0\",\"sinal_1\":\"pulso\",\"h_i_1\":\"0000\",\"h_o_1\":\"0000\",\"int_2\":\"R2\",\"tipo_2\":\"0\",\"sinal_2\":\"pulso\",\"h_i_2\":\"0000\",\"h_o_2\":\"0000\",\"int_3\":\"R3\",\"tipo_3\":\"0\",\"sinal_3\":\"pulso\",\"h_i_3\":\"0000\",\"h_o_3\":\"0000\",\"int_4\":\"R4\",\"tipo_4\":\"0\",\"sinal_4\":\"pulso\",\"h_i_4\":\"0000\",\"h_o_4\":\"0000\",\"log\":\"sim\",\"nivel\":\"4\",\"v_mq\":\"20\",\"v_mq_fu\":\"50\",\"s_a\":\"123\"}", "param.txt");
      closeFS();
    }
    /*
      ------------------------------------------------------------------------------
      DESLIGAR TODOS AS PORTAS OUTPUT DA CENTRAL
      CHAMADA HTTP EX: HTTP://IP_HOST/?00015
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00015")  //
    {
      botao1.contador = 11;
      botao1.conta_pir_1 = 0;
    }
    /*
      ------------------------------------------------------------------------------
      APLICAR AS CONFIGURAÇÕES APÓS SEREM GRAVADAS NA CENTRAL
      CHAMADA HTTP EX: HTTP://IP_HOST/?00016
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00016")
    {
      cont_ip_banco = 0;
    }
    /*
      ------------------------------------------------------------------------------
      ATUALIZAÇÃO DE DATA E HORA
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00018") {
      relogio_ntp(0);
    }
    /*
      ------------------------------------------------------------------------------
      PAUSAR LEITURAS DO SENSOR DE GAS
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00019") {
      String s_hora_min = relogio_ntp(4);                           //armazenda data e hora 00:00
      String s_hora = String(s_hora_min).substring(0, 2);           //separa o valor da hora
      String s_minuto = String(s_hora_min).substring(3, 5);         //separa o valor do minuto
      int i_minutos = s_minuto.toInt() + 1;                         //adiciona minutos ao valor encontrado em s_minuto e converte em inteiro
      String valor_hora_fim = s_hora + ":" + String(i_minutos);
      P_LEITURAS_MQ = 1;
    }

    /*
      ------------------------------------------------------------------------------
      ARMAZENA TEMPERATURA DE SENSOR DHT EXTERNO
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00020") {
      /*  
        EXEMPLO DE MENSAGEM: http://192.168.0.12/?00020&temp=15.20&umid=99&glp=1&fu=1&esp=Area-Externa
      */
      temp_ext = quebraString("temp", stringUrl);
      umid_ext = quebraString("umid", stringUrl);
      nome_esp = quebraString("esp", stringUrl);
      glp_ = quebraString("glp", stringUrl);
      fu_ = quebraString("fu", stringUrl);
      tabela_sensores_ext = quebraString("tabela", stringUrl);
      //Serial.println(tabela_sensores_ext);----
       

      montaCentraisEsp(nome_esp,tabela_sensores_ext);
      
      
    }
    
    requisicao.remove(0);

    /*
      ------------------------------------------------------------------------------
      GRAVA PARAMETROS NO SPIFFS(SISTEMA DE ARQUIVO) DA CENTRAL, ARQUIVO "param.txt"
      ------------------------------------------------------------------------------
    */
    String codidoExec = stringUrl.substring(10, 15);
    if (codidoExec == "00012") {
      openFS();
      SPIFFS.remove("/param.txt");
      criarArquivo("/param.txt");
      String i = stringUrl;
      stringUrl = "";
      int final_s = i.indexOf("HTTP/1.1");
      stringUrl = i.substring(0, final_s - 1);
      gravarArquivo("{\"sv\":\"" + quebraString("servidor", stringUrl)

                      + "\",\"i_1\":\"" + quebraString("int_1", stringUrl)
                      + "\",\"ti_1\":\"" + quebraString("tipo_1", stringUrl)
                      + "\",\"s_1\":\"" + quebraString("sinal_1", stringUrl)
                      + "\",\"h_i_1\":\"" + quebraString("hora1_in_1", stringUrl) + quebraString("hora1_in_2", stringUrl)
                      + "\",\"h_o_1\":\"" + quebraString("hora1_out_1", stringUrl) + quebraString("hora1_out_2", stringUrl)
                      + "\",\"t_1\":\"" + quebraString("timer_1", stringUrl)

                      + "\",\"i_2\":\"" + quebraString("int_2", stringUrl)
                      + "\",\"ti_2\":\"" + quebraString("tipo_2", stringUrl)
                      + "\",\"s_2\":\"" + quebraString("sinal_2", stringUrl)
                      + "\",\"h_i_2\":\"" + quebraString("hora2_in_1", stringUrl) + quebraString("hora2_in_2", stringUrl)
                      + "\",\"h_o_2\":\"" + quebraString("hora2_out_1", stringUrl) + quebraString("hora2_out_2", stringUrl)

                      + "\",\"i_3\":\"" + quebraString("int_3", stringUrl)
                      + "\",\"ti_3\":\"" + quebraString("tipo_3", stringUrl)
                      + "\",\"s_3\":\"" + quebraString("sinal_3", stringUrl)
                      + "\",\"h_i_3\":\"" + quebraString("hora3_in_1", stringUrl) + quebraString("hora3_in_2", stringUrl)
                      + "\",\"h_o_3\":\"" + quebraString("hora3_out_1", stringUrl) + quebraString("hora3_out_2", stringUrl)

                      + "\",\"i_4\":\"" + quebraString("int_4", stringUrl)
                      + "\",\"ti_4\":\"" + quebraString("tipo_4", stringUrl)
                      + "\",\"s_4\":\"" + quebraString("sinal_4", stringUrl)
                      + "\",\"h_i_4\":\"" + quebraString("hora4_in_1", stringUrl) + quebraString("hora4_in_2", stringUrl)
                      + "\",\"h_o_4\":\"" + quebraString("hora4_out_1", stringUrl) + quebraString("hora4_out_2", stringUrl)

                      + "\",\"l\":\"" + quebraString("log", stringUrl)
                      + "\",\"n\":\"" + quebraString("nivel", stringUrl)
                      + "\",\"mq\":\"" + quebraString("v_mq", stringUrl)
                      + "\",\"fu\":\"" + quebraString("v_mq_fu", stringUrl)
                      + "\"}",
                    "param.txt");
      cont_ip_banco = 0;
      closeFS();
      stringUrl.remove(0);
    }

    /*
      --------------------------------
      MONTAR E EXIBE PAGINA WEB DA CENTRAL
      --------------------------------
    */
    buf = pagina();
    client.print(buf);
    client.flush();
    client.stop();
    buf.remove(0);
    /*
      --------------------------------
      --------------------------------
    */
  }
  sensorMQ();
}
