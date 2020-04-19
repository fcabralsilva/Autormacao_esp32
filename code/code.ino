#include <ArduinoOTA.h>
#include <Alarme.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <FS.h>
#include <NTPClient.h>
#include <SPIFFS.h>
#include "soc/rtc_cntl_reg.h"
#include <Wire.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <WebServer.h>
#include <WiFiManager.h>

String VERSAO = "V09.12 - 17/04/2020";

#define BUZZER                18
#define PIN_MQ2               34
#define DHTPIN                19
#define DHTTYPE               DHT11
#define VRL_VALOR             5       //resistência de carga
#define RO_FATOR_AR_LIMPO     9.83    //resistência do sensor em ar limpo 9.83 de acordo com o datasheet
#define ITERACOES_CALIBRACAO  25      //numero de leituras para calibracao
#define ITERACOES_LEITURA     5       //numero de leituras para analise
#define GAS_LPG               0
#define GAS_CO                1
#define SMOKE                 2
#define LED_VERDE             15
#define LED_VERMELHO          4
#define LED_AZUL              2

struct botao1 {
  int entrada = 32, rele = 33;
  boolean estado = 0, estado_atual = 0  , estado_antes = 0;
  int contador = 0;
  const char* modelo = "pulso";
  const char* nomeInter = "Com1";
  const char* tipo = "0";
  const char* agenda_in;
  const char* agenda_out;
} botao1;
struct botao2 {
  int entrada = 25, rele = 26;
  boolean estado = 0, estado_atual = 0  , estado_antes = 0;
  int contador = 0;
  const char* modelo = "pulso";
  const char* tipo = "0";
  const char* nomeInter = "Com2";
  const char* agenda_in;
  const char* agenda_out;
} botao2;
struct botao3 {
  int entrada = 14, rele = 27;
  boolean estado = 0, estado_atual = 0  , estado_antes = 0;
  int contador = 0;
  const char* tipo = "0";
  const char* modelo = "pulso";
  const char* nomeInter = "Com3";
  const char* agenda_in;
  const char* agenda_out;
} botao3;
struct botao4 {
  int entrada = 12, rele = 13;
  boolean estado = 0, estado_atual = 0  , estado_antes = 0;
  int contador = 0;
  const char* tipo = "0";
  const char* modelo = "pulso";
  const char* nomeInter = "Com4";
  const char* agenda_in;
  const char* agenda_out;
} botao4;

const char WEB_HEAD[] PROGMEM            = "<!DOCTYPE html><html lang=\"pt-br\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta charset=\"utf-8\"><link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\" integrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr\" crossorigin=\"anonymous\"><link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\"><script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script><title>{v}</title>";
const char WEB_STYLE[] PROGMEM           = "<style type=\"text/css\">body .form-control{font-size:12px}input,button,select,optgroup,textarea {  margin: 5px;}.table td, .table th {padding:0px;}.th {width:100px;}.shadow-lg {box-shadow: 0px } #collapseExample {font-size:10px}</style>";
const char WEB_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char WEB_HEAD_END[] PROGMEM        = "</head><body>";
const char WEB_END[] PROGMEM             = "</div></body></html>";
const char WEB_DIV_CONTAINER[] PROGMEM   = "<div class=\"container shadow-lg p-3 mb-5 bg-white rounded\">";
const char WEB_NAV_MENU[] PROGMEM        = "<ul class=\"nav nav-pills mb-3\" id=\"pills-tab\" role=\"tablist\"><li class=\"nav-item\"><a class=\"nav-link active\" id=\"pills-home-tab\" data-toggle=\"pill\" href=\"#pills-home\" role=\"tab\" aria-controls=\"pills-home\" aria-selected=\"true\">Home</a></li> <li class=\"nav-item\"><a class=\"nav-link\" id=\"pills-profile-tab\" data-toggle=\"pill\" href=\"#pills-profile\" role=\"tab\" aria-controls=\"pills-profile\" aria-selected=\"false\">Configuração</a></li></ul>";
const char WEB_BOTAO_SUCCESS[] PROGMEM   = "<a href=\"?porta=\"{A}\" title=\"Porta:\"{B}\"><button type=\"button\"  class=\"btn btn-success\">\"{C}\"</button></a>";
const char A_HREF[] PROGMEM              = "  <a href=\"?porta={A}&acao={G}&central={B}\" title=\"Porta:{C} Botão:{D}\"><button type=\"button\"  class=\"{E}\">{F}</button></a>";
const char refresh[] PROGMEM             = "<img width=\"16px\" src=\"data:image/svg+xml;utf8;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iaXNvLTg4NTktMSI/Pgo8IS0tIEdlbmVyYXRvcjogQWRvYmUgSWxsdXN0cmF0b3IgMTkuMC4wLCBTVkcgRXhwb3J0IFBsdWctSW4gLiBTVkcgVmVyc2lvbjogNi4wMCBCdWlsZCAwKSAgLS0+CjxzdmcgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIiB4bWxuczp4bGluaz0iaHR0cDovL3d3dy53My5vcmcvMTk5OS94bGluayIgdmVyc2lvbj0iMS4xIiBpZD0iQ2FwYV8xIiB4PSIwcHgiIHk9IjBweCIgdmlld0JveD0iMCAwIDUxMiA1MTIiIHN0eWxlPSJlbmFibGUtYmFja2dyb3VuZDpuZXcgMCAwIDUxMiA1MTI7IiB4bWw6c3BhY2U9InByZXNlcnZlIiB3aWR0aD0iNTEycHgiIGhlaWdodD0iNTEycHgiPgo8cGF0aCBzdHlsZT0iZmlsbDojMTdBQ0U4OyIgZD0iTTQ0Ni44LDE2Ni44OTljLTQuNzk5LTcuNS0xNC43LTktMjEuODk5LTMuOUwzNzYuMywxOTkuNmMtNiw0LjQ5OS03LjgsMTIuOS0zLjksMTkuNDk5ICBjMTIuMjk5LDIxLDE4LjYsNDMuNSwxOC42LDY2LjkwMWMwLDc0LjM5OS02MC42MDEsMTM1LTEzNSwxMzVzLTEzNS02MC42MDEtMTM1LTEzNWMwLTY5LjMsNTIuNS0xMjYuNjAxLDEyMC0xMzQuMTAxVjE5NiAgYzAsOSw3LjIsMTQuNywxNSwxNWMyLjk5OSwwLDYuMzAxLTAuOTAxLDktMy4wMDFsMTIwLTg5Ljk5OGM4LjEwMS02LDguMTAxLTE4LjAwMSwwLTI0LjAwMUwyNjUsM2MtMi42OTktMi4wOTgtNi0yLjk5OS05LTIuOTk5ICBjLTcuOCwwLTE1LDYuMzAxLTE1LDE1djQ2LjYwMUMxMjMuOTk5LDY5LjEwMSwzMSwxNjYuODk5LDMxLDI4NmMwLDEyNC4yLDEwMC44LDIyNiwyMjUsMjI2czIyNS0xMDEuOCwyMjUtMjI2ICBDNDgxLDI0My45OTksNDY4Ljk5OSwyMDIuODk5LDQ0Ni44LDE2Ni44OTl6Ii8+CjxnPgoJPHBhdGggc3R5bGU9ImZpbGw6IzE2ODlGQzsiIGQ9Ik0yNjUsMjA3Ljk5OWMtMi42OTksMi4xLTYsMy4wMDEtOSwzLjAwMVYwYzIuOTk5LDAsNi4zMDEsMC45MDEsOSwyLjk5OWwxMjAsOTEgICBjOC4xMDEsNiw4LjEwMSwxOC4wMDEsMCwyNC4wMDFMMjY1LDIwNy45OTl6Ii8+Cgk8cGF0aCBzdHlsZT0iZmlsbDojMTY4OUZDOyIgZD0iTTQ4MSwyODZjMCwxMjQuMi0xMDAuOCwyMjYtMjI1LDIyNnYtOTFjNzQuMzk5LDAsMTM1LTYwLjYwMSwxMzUtMTM1ICAgYzAtMjMuNDAxLTYuMzAxLTQ1LjkwMS0xOC42LTY2LjkwMWMtMy45LTYuNTk5LTIuMS0xNSwzLjktMTkuNDk5bDQ4LjYtMzYuNjAxYzcuMi01LjA5OSwxNy4xLTMuNiwyMS44OTksMy45ICAgQzQ2OC45OTksMjAyLjg5OSw0ODEsMjQzLjk5OSw0ODEsMjg2eiIvPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+CjxnPgo8L2c+Cjwvc3ZnPgo=\" />";
//const char PAINEL_SENSOR []              = "<div class=\"row\"><div class=\"col-sm-2\"><h3><i class=\"fas fa-thermometer-half\" style=\"color:#059e8a;\"></i> {A}<sup class=\"units\">&deg;C</sup></h3></div><div class=\"col-sm-2\"><h3><i class=\"fas fa-tint\" style=\"color:#00add6;\"></i> {B}<sup class=\"units\">%</sup></h3></div><div class=\"col-sm-2\"><h3><i class=\"fas fa-burn\" style=\"color:#fb0102;\"></i> {C}<sup class=\"units\">ppm</sup></h3></div><div class=\"col-sm-2\"><h3><i class=\"fab fa-cloudversify\" style=\"color:#fb0102;\"></i> {D}<sup class=\"units\">ppm</sup></h3></div></div>";

long milis = 0;        	// último momento que o LED foi atualizado
long interval = 500;    // tempo de transição entre estados (milisegundos)
String ipLocalString, buff, URL, linha, GLP, FUMACA, retorno, serv, logtxt = "sim", hora_rtc, buf;
const char *json, *LIMITE_MQ2 = "99", *LIMITE_MQ2_FU = "99";
const char *ssid, *password, *servidor, *conslog, *nivelLog = "4", *verao, *s_senha_alarme = "123456";
const int PIN_AP = 0, i_sensor_alarme = 17, i_sirene_alarme = 18;
int portaServidor = 80;
int contarParaGravar1 = 0, nContar = 0, cont_ip_banco = 0, nivel_log = 4, estado_atual = 0, estado_antes = 0, freq = 2000, channel = 0, resolution = 8, n = 0, sensorMq2 = 0, MEM_EEPROM_MQ2 = 20;
short paramTempo = 60;
unsigned long time3, time3Param = 90000, timeDht, timeMq2 , tempo = 0, timeDhtParam = 300000, timeMq2Param = 10000, time_sirene;
IPAddress ipHost;
WiFiUDP udp;
NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);//Cria um objeto "NTP" com as configurações.utilizada no Brasil

struct tm data;//Cria a estrutura que contem as informacoes da data.
int hora;
char data_formatada[64];
int ATUALIZAR_DH;
String hora_ntp;

boolean estado_inter;

float umidade = 0, temperatura = 0;
float LPGCurve[3]   =  {2.3, 0.20, -0.47};
float COCurve[3]    =  {2.3, 0.72, -0.34};
float SmokeCurve[3] =  {2.3, 0.53, -0.44};
float Ro = 10;

boolean b_status_alarme = 0, agenda_ = 0;

DHT dht(DHTPIN, DHTTYPE);
WiFiServer server(80);
Alarme alarme;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);
  dht.begin();

  pinMode(PIN_AP, INPUT_PULLUP);

  pinMode(botao1.rele, OUTPUT);
  pinMode(botao1.entrada, INPUT_PULLUP);
  digitalWrite(botao1.rele, LOW);

  pinMode(botao2.rele, OUTPUT);
  pinMode(botao2.entrada, INPUT_PULLUP);
  digitalWrite(botao2.rele, LOW);

  pinMode(botao3.rele, OUTPUT);
  pinMode(botao3.entrada, INPUT_PULLUP);
  digitalWrite(botao3.rele, LOW);

  pinMode(botao4.rele, OUTPUT);
  pinMode(botao4.entrada, INPUT_PULLUP);
  digitalWrite(botao4.rele, LOW);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  pinMode(LED_VERDE, OUTPUT);
  digitalWrite(LED_VERDE, LOW);
  pinMode(LED_VERMELHO, OUTPUT);
  //digitalWrite(LED_VERMELHO, HIGH);
  pinMode(LED_AZUL, OUTPUT);

  alarme.sensores(i_sensor_alarme);
  alarme.sirene(i_sirene_alarme);

  openFS();
  criarArquivo("/param.txt");
  criarArquivo("/log.txt");

  //---------------------------------------
  //    CONECTANDO A REDE WIFI
  //---------------------------------------
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  if (!wifiManager.autoConnect("WIFI_AUTOMAÇÃO", "123")) {
    Serial.println(" FALHA NA CONEÇÃO! ");
    ESP.restart();
  }

  ntp.begin();          // Inicia o protocolo NTP data e hora
  ntp.forceUpdate();    // Atualização
  relogio_ntp(0);

  server.begin();

  ipHost = WiFi.localIP();
  ipLocalString = String(ipHost[0]) + "." + String(ipHost[1]) + "." + String(ipHost[2]) + "." + String(ipHost[3]);

  //---------------------------------------
  //PRIMEIRA LEITURA DOs SENSORES
  //---------------------------------------
  umidade = dht.readHumidity() * 1;
  temperatura = dht.readTemperature() * 1;
  calibrarSensor();
  sensorMq2 = analogRead(PIN_MQ2);
  GLP = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, GAS_LPG) );
  FUMACA = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, SMOKE));

  arduino_ota();
  ArduinoOTA.begin();

  retorno = "SERVIDOR_CONECT";

  ledcSetup(channel, freq, resolution);
  ledcAttachPin(5, channel);
  gravarArquivo("\n *** INICIANDO SISTEMA *** \n \n " + VERSAO, "log.txt");
  digitalWrite(BUZZER, HIGH);
  delay(1000);
  digitalWrite(BUZZER, LOW);

}

void loop()
{

  ArduinoOTA.handle();

  WiFiManager wifiManager;
  WiFiClient client = server.available();

  pisca_led(LED_VERDE, true);

  while (cont_ip_banco < 1)
  {
    // FAZENDO LEITURA DE PARAMETROS DO SISTEMA
    openFS();

    StaticJsonDocument<750> doc;
    json = lerArquivoParam().c_str();
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
      gravaLog(" " + relogio_ntp(1) + " - ERRO 0101 - Arquivo json: ", logtxt, 1);
      Serial.println(error.c_str());
      cont_ip_banco++;
      return;
    }
    JsonObject root = doc.as<JsonObject>();
    //gravaLog(" " + relogio_ntp(1) + " - Configurações da Central ", logtxt, 2);
    servidor      = root["servidor"];
    serv = String(servidor);
    gravaLog(" " + relogio_ntp(1) + "   BD:   " + String(servidor), logtxt, 2);

    botao1.nomeInter  = root["int_1"];
    botao1.tipo       = root["tipo_1"];
    botao1.modelo     = root["sinal_1"];
    botao1.agenda_in  = root["h_i_1"];
    botao1.agenda_out = root["h_o_1"];
    //gravaLog(" " + relogio_ntp(1) + "   Int 1: " + String(botao1.nomeInter) + " / " + String(botao1.tipo) + " / " + String(botao1.modelo)+ " / " + String(botao1.agenda_in)+ " / " + String(botao1.agenda_out), logtxt, 2);

    botao2.nomeInter  = root["int_2"];
    botao2.tipo       = root["tipo_2"];
    botao2.modelo     = root["sinal_2"];
    botao2.agenda_in  = root["h_i_2"];
    botao2.agenda_out = root["h_o_2"];
    //gravaLog(" " + relogio_ntp(1) + "   Int 2: " + String(botao2.nomeInter) + " / " + String(botao2.tipo) + " / " + String(botao2.modelo), logtxt, 2);

    botao3.nomeInter  = root["int_3"];
    botao3.tipo       = root["tipo_3"];
    botao3.modelo     = root["sinal_3"];
    botao3.agenda_in  = root["h_i_3"];
    botao3.agenda_out = root["h_o_3"];
    //gravaLog(" " + relogio_ntp(1) + "   Int 3: " + String(botao3.nomeInter) + " / " + String(botao3.tipo) + " / " + String(botao3.modelo), logtxt, 2);

    botao4.nomeInter  = root["int_4"];
    botao4.tipo       = root["tipo_4"];
    botao4.modelo     = root["sinal_4"];
    botao4.agenda_in  = root["h_i_4"];
    botao4.agenda_out = root["h_o_4"];
    //gravaLog(" " + relogio_ntp(1) + "   Int 4: " + String(botao4.nomeInter) + " / " + String(botao4.tipo) + " / " + String(botao4.modelo), logtxt, 2);

    conslog   = root["log"];
    logtxt = String(conslog);
    nivelLog = root["nivel"];
    LIMITE_MQ2 = root["v_mq"];
    LIMITE_MQ2_FU = root["v_mq_fu"];
    s_senha_alarme = root["s_a"];
    //gravaLog(" " + relogio_ntp(1) + "   Log ?: " + String(conslog) + " N: " + String(nivelLog), logtxt, 1);
    //gravaLog(" " + relogio_ntp(1) + "   Senha Alarme : " + String(s_senha_alarme), logtxt, 1);
    cont_ip_banco++;
  }
  /*
  	  SE FOR PRESSIONADO BOTÃO PIN_AP, TODAS AS CONFIGURAÇÕES DA CENTRAL
  	  SERÃO DELETADAS(WIFI, PARAMETROS, ETC). VARIAVEL DO BOTÃO É PIN_AP.
  */
  if ( digitalRead(PIN_AP) == LOW )
  {

    gravaLog(" " + relogio_ntp(1) + " - Modo AP Ativado ", logtxt, 1);
    /*
    	Apagando dados de conexão WIFI da central
    */
    esp_wifi_restore();
    Serial.println("\n Apagando configurações WIFI..."); //tenta abrir o portal
    if (!wifiManager.startConfigPortal(" WIFI_AUT", "12345678") )
    {
      gravaLog(" " + relogio_ntp(1) + " - ERRO 0102 - Falha no modo AP", logtxt, 1);
      ESP.restart();
    }
    Serial.println(" Modo config WIFI...");

  }

  /*
     VERIFICA SE POSSUI AGENDAMENTO DA PORTA GPIO
  */
  agendamento(botao1.rele, botao1.agenda_in, botao1.agenda_out, relogio_ntp(3));

  // MONTAR BOTOES DE INTERRUPTORES
  botao1.estado = portaIO(botao1.entrada, botao1.rele, botao1.tipo, botao1.modelo, botao1.contador, botao1.estado);
  botao2.estado = portaIO(botao2.entrada, botao2.rele, botao2.tipo, botao2.modelo, botao2.contador, botao2.estado);
  botao3.estado = portaIO(botao3.entrada, botao3.rele, botao3.tipo, botao3.modelo, botao3.contador, botao3.estado);
  botao4.estado = portaIO(botao4.entrada, botao4.rele, botao4.tipo, botao4.modelo, botao4.contador, botao4.estado);

  /*
      APAGAR TODAS AS PORTAS DE SAIDA
  */
  if ((botao1.contador >= 10)
      || (botao2.contador >= 10)
      || (botao3.contador >= 10)
      || (botao4.contador >= 10))
  {
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
    LEITURA DA REQUISIÇÃO DE CHAMADAS GET
  */
  if (client)
  {
    URL = "";
    URL = client.readStringUntil('\r');
  } else {
    URL = "vazio";
  }

  if (URL != "vazio")
  {
    /*
    	EXEMPLO NA CHAMADA WEB DESLIGAR LAMPADA - CHAMADA HTTP EX: HTTP://IP_HOST/?porta=20&acao=desligar&central=IP_HOST
    */
    String stringUrl = URL;
    gravaLog(" " + relogio_ntp(1) + " - " + String(URL), logtxt, 4);
    URL = "";
    String requisicao = stringUrl.substring(6, 11);
    if (requisicao == "porta")
    {
      String numero 	= stringUrl.substring(12, 14);
      String acao 		= stringUrl.substring(20, 24);
      String central 	= stringUrl.substring(33, 40);
      int numeroInt 	= numero.toInt();
      nContar = 0;
      n = 0;
      acionaPorta(numeroInt, requisicao, acao);
      botao1.estado = status_porta(numeroInt, botao1.rele, botao1.estado, acao);
      botao2.estado = status_porta(numeroInt, botao2.rele, botao2.estado, acao);
      botao3.estado = status_porta(numeroInt, botao3.rele, botao3.estado, acao);
      botao4.estado = status_porta(numeroInt, botao4.rele, botao4.estado, acao);
    }

    /*
      REINCIAR CENTRAL POR COMANDA HTTP - CHAMADA HTTP EX: HTTP://IP_HOST/?00000
    */
    if (requisicao == "00000")
    {
      gravaLog(" " + relogio_ntp(1) + " - REINIANDO", logtxt, 1);
      ESP.restart();
    }
    if (requisicao == "00001")
    {
      Serial.println(stringUrl);
    }

    /*
      CALIBRAR SENSOR MQ2 - CHAMADA HTTP EX: HTTP://IP_HOST/?00010
    */
    if (requisicao == "00010")
    {
      gravaLog(" " + relogio_ntp(1) + " - Recalibrando sensor MQ-X", logtxt, 2);
      calibrarSensor();
    }

    /*
      GRAVAR VALOR DE LEITURA DO SENSOR DE GAS NA EEPROM - CHAMADA HTTP EX: HTTP://IP_HOST/?00011
    */
    String codidoExec = stringUrl.substring(10, 15);
    /*
      GRAVA PARAMETROS NO SPIFFS(SISTEMA DE ARQUIVO) DA CENTRAL, ARQUIVO "param.txt"
    */
    if (codidoExec == "00012")
    {
      openFS();
      SPIFFS.remove("/param.txt");
      criarArquivo("/param.txt");
      String i = stringUrl;
      stringUrl = "";
      int final_s = i.indexOf("HTTP/1.1");
      stringUrl = i.substring(0, final_s - 1);
      String senha_;
      if (quebraString("senhaAlarme", stringUrl) == "")
      {
        senha_ = s_senha_alarme;
      } else
      {
        senha_ = quebraString("senhaAlarme", stringUrl);
      }

      gravarArquivo("{\"servidor\":\"" + quebraString("servidor", stringUrl)

                    + "\",\"int_1\":\"" + quebraString("int_1", stringUrl)
                    + "\",\"tipo_1\":\"" + quebraString("tipo_1", stringUrl)
                    + "\",\"sinal_1\":\"" + quebraString("sinal_1", stringUrl)
                    + "\",\"h_i_1\":\"" + quebraString("hora1_in_1", stringUrl) + quebraString("hora1_in_2", stringUrl)
                    + "\",\"h_o_1\":\"" + quebraString("hora1_out_1", stringUrl) + quebraString("hora1_out_2", stringUrl)

                    + "\",\"int_2\":\"" + quebraString("int_2", stringUrl)
                    + "\",\"tipo_2\":\"" + quebraString("tipo_2", stringUrl)
                    + "\",\"sinal_2\":\"" + quebraString("sinal_2", stringUrl)
                    + "\",\"h_i_2\":\"" + quebraString("hora2_in_1", stringUrl) + quebraString("hora2_in_2", stringUrl)
                    + "\",\"h_o_2\":\"" + quebraString("hora2_out_1", stringUrl) + quebraString("hora2_out_2", stringUrl)

                    + "\",\"int_3\":\"" + quebraString("int_3", stringUrl)
                    + "\",\"tipo_3\":\"" + quebraString("tipo_3", stringUrl)
                    + "\",\"sinal_3\":\"" + quebraString("sinal_3", stringUrl)
                    + "\",\"h_i_3\":\"" + quebraString("hora3_in_1", stringUrl) + quebraString("hora3_in_2", stringUrl)
                    + "\",\"h_o_3\":\"" + quebraString("hora3_out_1", stringUrl) + quebraString("hora3_out_2", stringUrl)

                    + "\",\"int_4\":\"" + quebraString("int_4", stringUrl)
                    + "\",\"tipo_4\":\"" + quebraString("tipo_4", stringUrl)
                    + "\",\"sinal_4\":\"" + quebraString("sinal_4", stringUrl)
                    + "\",\"h_i_4\":\"" + quebraString("hora4_in_1", stringUrl) + quebraString("hora4_in_2", stringUrl)
                    + "\",\"h_o_4\":\"" + quebraString("hora4_out_1", stringUrl) + quebraString("hora4_out_2", stringUrl)

                    + "\",\"log\":\"" + quebraString("log", stringUrl)
                    + "\",\"nivel\":\"" + quebraString("nivel", stringUrl)
                    + "\",\"v_mq\":\"" + quebraString("v_mq", stringUrl)
                    + "\",\"v_mq_fu\":\"" + quebraString("v_mq_fu", stringUrl)
                    + "\",\"s_a\":\"" + senha_
                    + "\"}", "param.txt");
      cont_ip_banco = 0;
      closeFS();
    }

    /*
      APAGAR ARQUIVO DE LOG MANUALMENTE - CHAMADA HTTP EX: HTTP://IP_HOST/?00013
    */
    if (requisicao == "00013")
    {
      deletarArquivo("/log.txt");
      criarArquivo("/log.txt");
    }

    /*
      APLICAR CONFIGURAÇÕES MINIMAS PARA FUNCIONAMENTO DA CENTRAL - CHAMADA HTTP EX: HTTP://IP_HOST/?00014
    */
    if (requisicao == "00014")
    {
      openFS();
      listDir(SPIFFS, "/", 0);
      deletarArquivo("/param.txt");
      criarArquivo("/param.txt");
      //gravaLog(" " + relogio_ntp(1) + " - Configuração minima ", logtxt, 3);
      gravarArquivo("{\"servidor\":\"" + String(ipHost) + "\",\"int_1\":\"P1\",\"int_2\":\"P2\",\"int_3\":\"P3\",\"int_4\":\"P4\",\"int_5\":\"P5\",\"tipo_1\":\"0\",\"tipo_2\":\"0\",\"tipo_3\":\"0\",\"tipo_4\":\"0\",\"tipo_5\":\"0\",\"sinal_1\":\"interruptor\",\"sinal_2\":\"interruptor\",\"sinal_3\":\"interruptor\",\"sinal_4\":\"interruptor\",\"sinal_5\":\"interruptor\",\"log\":\"sim\",\"verao\":\"nao\",\"nivel\":\"4\",\"s_a\":\"4\"}", "param.txt");
      closeFS();
    }
    /*
      DESLIGAR TODOS AS PORTAS OUTPUT DA CENTRAL - CHAMADA HTTP EX: HTTP://IP_HOST/?00015
    */
    if (requisicao == "00015") //
    {
      botao1.contador = 11;
    }
    /*
      APLICAR AS CONFIGURAÇÕES APÓS SEREM GRAVADAS NA CENTRAL - CHAMADA HTTP EX: HTTP://IP_HOST/?00016
    */
    if (requisicao == "00016") //
    {
      cont_ip_banco = 0;
    }
    /*
      FUNÇÃO DE CONTROLE DO ALARME
      ATIVAR ALARME - CHAMADA HTTP EX: HTTP://IP_HOST/?00117
      DESATIVAR ALARME - CHAMADA HTTP EX: HTTP://IP_HOST/?00017
    */
    if (requisicao == "00117")
    {
      b_status_alarme = 1;
      pisca_led(LED_VERMELHO, true);
    } else if (requisicao == "00017")
    {
      b_status_alarme = 0;
      alarme.desligado(i_sirene_alarme);

    }

    if (requisicao == "00001")
    {
      //Resetar configurações WIFI para trocar de rede.
      wifiManager.setBreakAfterConfig(true);
      wifiManager.resetSettings();
    }

    /* ALARME */
    alarme.monitoramento(i_sensor_alarme, i_sirene_alarme, b_status_alarme);

    if (requisicao == "00018")
    {
      relogio_ntp(0);
    }


    /*
       PAGINA WEB
    */
    String buf = FPSTR(WEB_HEAD);
    buf.replace("{v}", "Central Automação");
    buf += FPSTR(WEB_SCRIPT);
    buf += FPSTR(WEB_STYLE);
    buf += FPSTR(WEB_HEAD_END);
    buf += FPSTR(WEB_DIV_CONTAINER);
    buf += FPSTR(WEB_NAV_MENU);

    /*
       DIV PRINCIPAL
    */
    buf += "<div class=\"tab-content\" id=\"pills-tabContent\">";
    buf += "<div class=\"tab-pane fade show active\" id=\"pills-home\" role=\"tabpanel\" aria-labelledby=\"pills-home-tab\">";

    /*
       PAINEL DE SENSORES
    */
    buf += "<hr />";
    buf += "<div class=\"row\">";
    buf += "<div class=\"col-sm-2\">";
    int temp = temperatura;
    buf += "<h1><i class=\"fas fa-thermometer-half\" style=\"color:#059e8a;\"></i> " + String(temp) + "<sup class=\"units\">&deg;C</sup></h1>";
    buf += "</div>";
    buf += "<div class=\"col-sm-2\">";
    int umid = umidade;
    buf += "<h3><i class=\"fas fa-tint\" style=\"color:#00add6;\"></i> " + String(umid) + "<sup class=\"units\">%</sup></h3>";
    //buf +="<div class=\"progress\">";
    if (umid >= 60)
    {
      //buf +="<div class=\"progress-bar bg-success\" data-toggle=\"tooltip\" title=\"Nível recomendado\" style=\"width:"+String(umid)+"%\">Ideal </div>";
      buf += "<span class=\"badge badge-pill badge-success\" data-toggle=\"tooltip\" title=\"Nível recomendado\"> Recomendado </span>";
    } else if (umid > 31 && umid < 60)
    {
      //buf +="<div class=\"progress-bar bg-danger\" data-toggle=\"tooltip\" title=\"Perigo, risco respiratório!!!\" style=\"width:"+String(umid)+"%\">Atenção</div>";
      buf += "<span class=\"badge badge-pill badge-warning\"> Cuidado </span>";
    } else if (umid <= 30)
    {
      //buf +="<div class=\"progress-bar bg-warning\" style=\"width:"+String(umid)+"%\">Cuidado </div>";
      buf += "<span class=\"badge badge-pill badge-danger\"> Perigo </span>";
    }
    //buf +="</div>";
    buf += "</div>";
    buf += "<div class=\"col-sm-2\">";
    int limit_glp = String(LIMITE_MQ2).toInt();
    int glp = GLP.toInt();
    buf += "<h3><i class=\"fas fa-burn\" style=\"color:#fb0102;\"></i> " + String(GLP) + "<sup class=\"units\">ppm</sup></h3>";
    if (glp >= limit_glp )
    {
      //buf +="<div class=\"progress-bar bg-success\" data-toggle=\"tooltip\" title=\"Nível recomendado\" style=\"width:"+String(umid)+"%\">Ideal </div>";
      buf += "<span class=\"badge badge-pill badge-danger\"> Perigo </span>";

    } else if (glp <= (limit_glp / 3))
    {
      //buf +="<div class=\"progress-bar bg-warning\" style=\"width:"+String(umid)+"%\">Cuidado </div>";
      buf += "<span class=\"badge badge-pill badge-success\"> Recomendado </span>";
    }
    else {
      //buf +="<div class=\"progress-bar bg-danger\" data-toggle=\"tooltip\" title=\"Perigo, risco respiratório!!!\" style=\"width:"+String(umid)+"%\">Atenção</div>";
      buf += "<span class=\"badge badge-pill badge-warning\"> Cuidado </span>";
    }
    int limit_fu = String(LIMITE_MQ2_FU).toInt();
    int fu = FUMACA.toInt();
    buf += "</div>";
    /* FIM PAINEL SENSORES*/
    
    buf += "</div>";
    buf += "<div><hr />";
    
    /*
      BOTÃO 1
    */
    String bt1, bt1_a;
    int conta_botao = 0;
    if (botao1.estado == true)
    {
      bt1 = "btn btn-success";
      bt1_a = "desliga";
    } else
    {
      bt1 = "btn btn-danger";
      bt1_a = "liga";
    }
    if (String(botao1.nomeInter) != "0") {
      buf += FPSTR(A_HREF);
      buf.replace("{A}", String(botao1.rele));
      buf.replace("{B}", ipLocalString);
      buf.replace("{C}", String(botao1.rele));
      buf.replace("{D}", String(botao1.entrada));
      buf.replace("{E}", bt1);
      buf.replace("{G}", bt1_a);
      buf.replace("{F}", String(botao1.nomeInter));
      conta_botao ++;
    }

    /*
    	BOTÃO 2
    */
    String bt2, bt2_a;
    if (botao2.estado == true) {
      bt2 = "btn btn-success";
      bt2_a = "desliga";
    } else {
      bt2 = "btn btn-danger";
      bt2_a = "liga";
    }
    if (String(botao2.nomeInter) != "0") {
      buf += FPSTR(A_HREF);
      buf.replace("{A}", String(botao2.rele));
      buf.replace("{B}", ipLocalString);
      buf.replace("{C}", String(botao2.rele));
      buf.replace("{D}", String(botao2.entrada));
      buf.replace("{E}", bt2);
      buf.replace("{G}", bt2_a);
      buf.replace("{F}", String(botao2.nomeInter));
      conta_botao ++;
    }

    /*
    	BOTÃO 3
    */
    String bt3, bt3_a;
    if (botao3.estado == true)
    {
      bt3 = "btn btn-success";
      bt3_a = "desliga";
    } else {
      bt3 = "btn btn-danger";
      bt3_a = "liga";
    }
    if (String(botao3.nomeInter) != "0") {
      buf += FPSTR(A_HREF);
      buf.replace("{A}", String(botao3.rele));
      buf.replace("{B}", ipLocalString);
      buf.replace("{C}", String(botao3.rele));
      buf.replace("{D}", String(botao3.entrada));
      buf.replace("{E}", bt3);
      buf.replace("{G}", bt3_a);
      buf.replace("{F}", String(botao3.nomeInter));
      conta_botao ++;
    }

    /*
    	BOTÃO 4
    */
    String bt4, bt4_a;
    if (botao4.estado == true)
    {
      bt4 = "btn btn-success";
      bt4_a = "desliga";
    } else {
      bt4 = "btn btn-danger";
      bt4_a = "liga";
    }

    if (String(botao4.nomeInter) != "0") {
      buf += FPSTR(A_HREF);
      buf.replace("{A}", String(botao4.rele));
      buf.replace("{B}", ipLocalString);
      buf.replace("{C}", String(botao4.rele));
      buf.replace("{D}", String(botao4.entrada));
      buf.replace("{E}", bt4);
      buf.replace("{G}", bt4_a);
      buf.replace("{F}", String(botao4.nomeInter));
      conta_botao ++;
    }

    /*
     *  BOTÃO PARA DESLIGAR TODAS AS PORTAS
     */

    if (conta_botao > 1)
    {
      buf += "<a href=\"?00015\" title=\"Desligar\"><button type=\"button\"  class=\"btn btn-danger\">Desli. Tudo</button></a>";
    }
    
    /*
       BOTÃO_ALARME 
    */
    if (b_status_alarme == 0) {
      buf += " <td> <a href=\"?00117\" title=\"Desligar\"> <button type=\"button\" class=\"btn btn-success\">Ligar Alarme</button> </a> </td>";
    } else {
      buf += "<td> <div class=\"accordion\" id=\"accordionExample\"> <button class=\"btn btn-danger\" type=\"button\" data-toggle=\"collapse\" data-target=\"#collapseTwo\" aria-expanded=\"false\" aria-controls=\"collapseTwo\"> Desli. Alarme </button></td> <div id=\"collapseTwo\" class=\"collapse\" aria-labelledby=\"headingTwo\" data-parent=\"#accordionExample\"> <form name=\"calcform\" method=\"post\" action=\"\"> <fieldset> <input type=\"password\" name=\"visor\" id=\"visor\" /> <table id=\"calc\"> <tr> <td> <input type=\"button\" name=\"num1\" class=\"num\" value=\"1\" onclick=\"calcNum(1)\" /> </td> <td> <input type=\"button\" name=\"num2\" class=\"num\" value=\"2\" onclick=\"calcNum(2)\" /> </td> <td> <input type=\"button\" name=\"num3\" class=\"num\" value=\"3\" onclick=\"calcNum(3)\" /> </td> </tr> <tr> <td> <input type=\"button\" name=\"num4\" class=\"num\" value=\"4\" onclick=\"calcNum(4)\" /> </td> <td> <input type=\"button\" name=\"num5\" class=\"num\" value=\"5\" onclick=\"calcNum(5)\" /> </td> <td> <input type=\"button\" name=\"num6\" class=\"num\" value=\"6\" onclick=\"calcNum(6)\" /> </td> </tr> <tr> <td> <input type=\"button\" name=\"num7\" class=\"num\" value=\"7\" onclick=\"calcNum(7)\" /> </td> <td> <input type=\"button\" name=\"num8\" class=\"num\" value=\"8\" onclick=\"calcNum(8)\" /> </td> <td> <input type=\"button\" name=\"num9\" class=\"num\" value=\"9\" onclick=\"calcNum(9)\" /> </td> </tr> <tr> <td> <input type=\"button\" name=\"limpar\" class=\"num\" value=\"X\" onclick=\"calcLimpar()\" /> </td> <td> <input type=\"button\" name=\"num0\" class=\"num\" value=\"0\" onclick=\"calcNum(0)\" /> </td> <td> <input type=\"button\" name=\"igual\" class=\"num\" value=\"=\" onclick=\"calcParse('resultado')\" /> </td> </tr></table> </fieldset> </form> </div> </div> </td>";
    }
    buf += "</p>";
    buf += "</div></div>";
    /*
     * 
     */
    
    /*
     * TELA DE CONFIGURAÇÕES
    */
    buf += "<div class=\"tab-pane fade\" id=\"pills-profile\" role=\"tabpanel\" aria-labelledby=\"pills-profile-tab\">";
    buf += "<form class=\"form-group\" action=\"?00012\">";
    //buf += "<table border='0px'>";
    buf += "<input type=\"hidden\" name=\"cod\" value=\"00012\">";
    buf +=  "<div class=\"container\">";
    buf +=  "<hr>";
    buf +=  " <div class=\"row\">";
    buf +=  "   <div class=\"col-sm-4\">";
    buf +=  "       <strong>Servidor</strong><input maxlength=\"15\" style=\"width:130px\" type=\"text\"   name=\"servidor\" value=\"" + serv + "\">";
    buf +=  "   </div>";
    buf +=  "   <div class=\"col-sm-2\">";
    buf +=  "       <input style=\"border:0px;width:80px\" type=\"time\" value=\"" + relogio_ntp(3) + "\" disabled><a href='?00018' title='Atualizar data e hora da central'>" + refresh + "</a>";
    buf +=  "   </div>";
    buf +=  " </div>";
    buf +=  "<hr>";
    
    buf += gpio_html (1, botao1.entrada, botao1.nomeInter, botao1.tipo, botao1.modelo, botao1.agenda_in, botao1.agenda_out);
    buf += gpio_html (2, botao2.entrada, botao2.nomeInter, botao2.tipo, botao2.modelo, botao2.agenda_in, botao2.agenda_out);
    buf += gpio_html (3, botao3.entrada, botao3.nomeInter, botao3.tipo, botao3.modelo, botao3.agenda_in, botao3.agenda_out);
    buf += gpio_html (4, botao4.entrada, botao4.nomeInter, botao4.tipo, botao4.modelo, botao4.agenda_in, botao4.agenda_out);
    buf +=  "<hr>";

    buf +=  " <div class=\"row\">";
    buf +=  "   <div class=\"col-sm-4\">";
    buf += "        <strong>LOG</strong> <select class='' style='width:100%x' name='log'> <option value='sim' " + selectedHTNL(conslog, "sim ") + "> Sim</option> <option value='nao' " + selectedHTNL(conslog, "nao ") + ">Não</option> </select> <select class='' style='width:100%x' name='nivel' title='Nível do log'> <option value='1' " + selectedHTNL(nivelLog, "1") + ">1</option> <option value='2' " + selectedHTNL(nivelLog, "2") + ">2</option> <option value='3' " + selectedHTNL(nivelLog, "3") + ">3</option> <option value='4' " + selectedHTNL(nivelLog, "4") + ">4</option> </select>";
    buf += "        <a href='?00013' title='APAGAR TODOS OS REGISTROS!'><button type='button' class='btn btn-danger btn-sm'> X </button></a>";
    buf += "        <button class=\"btn btn-primary btn-sm\" type=\"button\" data-toggle=\"collapse\" data-target=\"#collapseExample\" aria-expanded=\"false\" aria-controls=\"collapseExample\">Log</button></td>";
    buf += "    </div>";
    buf += "  </div>";
    buf +=  " <div class=\"row\">";
    buf +=  "   <div class=\"col-sm-6\">";
    buf += "        <strong>Senha</strong></td><td><input maxlength=\"10\" style=\"width:80px\" type=\"password\"   name=\"senhaAlarme\" value=\"\">";
    buf += "    </div>";
    buf += "  </div>";
    buf +=  " <div class=\"row\">";
    buf +=  "   <div class=\"col-sm-8\">";
    buf += "        <strong>MQ2 </strong>";
    buf += "        <sup>GAS </sup><input maxlength=\"2\" style=\"width:24px\" type=\"text\" name=\"v_mq\" value=\"" + String(LIMITE_MQ2) + "\">";
    buf += "        <sup>Fuma&ccedil;a </sup><input maxlength=\"2\" style=\"width:24px\" type=\"text\" name=\"v_mq_fu\" value=\"" + String(LIMITE_MQ2_FU) + "\">";
    buf += "    </div>";
    buf += "  </div>";
    buf +=  " <div class=\"row\">";
    buf +=  "   <div class=\"col-sm-2\">";
    buf += "         <input class=\"btn btn-info\" type=\"submit\" value=\"Salvar\">";
    buf += "    </div>";
    buf += "  </div>";
    buf += "</div>";

    buf += "</form> ";

    buf += "<div class='collapse' id='collapseExample'> <div class='card card-body'> ";
    buf += lerArquivo();
    buf += "</div> </div> </div> </div> ";
    buf += "<a href=\"http://" + ipLocalString + "\"><p style=\"text-align:center\"><p style=\"text-align:right\"> <span class=\"badge badge-pill badge-primary\">IP: " + ipLocalString + " " + VERSAO + "</span></span></a></p>";
    buf += "<script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script>";
    buf += "<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"></script>";
    buf += "</body></html>";
    buf += "<script>$(document).ready(function(){$('[data-toggle=\"tooltip\"]').tooltip();});";
    buf += " function calcNum(num) { document.calcform.visor.type = 'password'; document.calcform.visor.value = document.calcform.visor.value + num; } var delay = 1500;  function calcParse(oper) { var valor = document.calcform.visor.value; if (valor == '') { document.calcform.visor.value = ''; } else { var senha = '" + String(s_senha_alarme) + "'; document.calcform.visor.type = 'text'; if (senha == valor) { document.calcform.visor.value = 'Senha Correta       '; window.location.href = \"?00017\"; valor = ''; } else { document.calcform.visor.value = 'Senha Incorreta      '; valor = ''; setTimeout(function() { calcLimpar(); }, delay); } } }  function calcLimpar() { document.calcform.visor.value = ''; } </script> ";
    buf += " </body>  </html> ";
    client.print(buf);
    client.flush();
    client.stop();
  }
  /*
     ROTINA DO SENSOR MQ-2
  */

  //---------------------------------------
  if (millis() >= timeMq2 + timeMq2Param)
  {
    sensorMq2 = analogRead(PIN_MQ2);
    digitalWrite(LED_AZUL, HIGH);
    delay(100);
    digitalWrite(LED_AZUL, LOW);
    GLP = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, GAS_LPG) );
    if (GLP == "2147483647") GLP = "0";

    FUMACA = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, SMOKE));
    if (FUMACA == "2147483647") FUMACA = "0";

    String CO = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, GAS_CO)  );
    if (CO == "2147483647") CO = "0";

    contarParaGravar1++;
    gravaLog(" " + relogio_ntp(1) + " - MQ2 A: " + String(sensorMq2) + " GLP:" + GLP + " " + "CO:" + CO + " " + "FU:" + FUMACA + " " + "L:" + contarParaGravar1, logtxt, 4);
    timeMq2 = millis();
    //buff = "sensor=mq-2&valor=mq-2;" + String(GLP) + ";&central=" + String(ipLocalString) + "&p=" + String(PIN_MQ2);
    if ((GLP.toInt() >= String(LIMITE_MQ2).toInt()) || (FUMACA.toInt() > String(LIMITE_MQ2_FU).toInt()))
    {
      digitalWrite(BUZZER, true);
      digitalWrite(LED_VERMELHO, true);
    } else
    {
      digitalWrite(LED_VERMELHO, false);
      digitalWrite(BUZZER, false);
    }
    //GRAVA NO BANCO O VALOR LIDO APOS X LEITURAS
    if ((contarParaGravar1 == 20) || (GLP.toInt() >= String(LIMITE_MQ2).toInt()) || (FUMACA.toInt() > String(LIMITE_MQ2_FU).toInt()) )
    {
      buff = "sensor=mq-2&valor=mq-2;" + String(GLP) + ";&central=" + String(ipLocalString) + "&p=" + String(PIN_MQ2);
      gravarBanco (buff);
      contarParaGravar1 = 0;
    }
  }

  /*
     ROTINA DO SENSOR DHT11
  */
  umidade = dht.readHumidity() * 1;
  temperatura = dht.readTemperature() * 1;

  if (millis() >= timeDht + (timeDhtParam)) {
    umidade = dht.readHumidity();
    temperatura = dht.readTemperature();
    digitalWrite(LED_AZUL, HIGH);
    delay(100);
    digitalWrite(LED_AZUL, LOW);
    int t = 1;
    if ((temperatura == int(temperatura)) && (umidade == int(umidade)) && (t == 1) )
    {
      timeDht = millis();
      buff = "sensor=dht11&valor=dht11;" + String(temperatura) + ";" + String(umidade) + ";&central=" + String(ipLocalString) + "&p=" + String(DHTPIN);
      gravarBanco(buff);
    } else {
      gravaLog(" " + relogio_ntp(1) + " - ERRO 0109 - Sensor DHT", logtxt, 1);
      t = 0;
      temperatura = 0;
      umidade = 0;
      timeDht = millis();

    }

  }
}
