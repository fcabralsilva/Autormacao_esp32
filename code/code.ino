#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <FS.h>
#include <NTPClient.h>
#include <SPIFFS.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <WiFiManager.h>

String VERSAO = "V10.10 - 02/07/2021";

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
#define DS18B20               6
#define portaServidor         80
#define PIN_AP                0

struct botao1 {
  const short entrada = 32, rele = 33;
  boolean estado = 0, estado_atual = 0  , estado_antes = 0;
  int contador = 0;
  const char* modelo = "pulso";
  const char* nomeInter = "Com1";
  const char* tipo = "0";
  const char* agenda_in;
  const char* agenda_out;
  const char* timer;
} botao1;
struct botao2 {
  const short entrada = 25, rele = 26;
  boolean estado = 0, estado_atual = 0  , estado_antes = 0;
  int contador = 0;
  const char* modelo = "pulso";
  const char* tipo = "0";
  const char* nomeInter = "Com2";
  const char* agenda_in;
  const char* agenda_out;
} botao2;
struct botao3 {
  const short entrada = 14, rele = 27;
  boolean estado = 0, estado_atual = 0  , estado_antes = 0;
  int contador = 0;
  const char* tipo = "0";
  const char* modelo = "pulso";
  const char* nomeInter = "Com3";
  const char* agenda_in;
  const char* agenda_out;
} botao3;
struct botao4 {
  const short entrada = 12, rele = 13;
  boolean estado = 0, estado_atual = 0  , estado_antes = 0;
  int contador = 0;
  const char* tipo = "0";
  const char* modelo = "pulso";
  const char* nomeInter = "Com4";
  const char* agenda_in;
  const char* agenda_out;
} botao4;

String addressMac;
const String hostname = "ESP32_TESTE";
long milis = 0;        	// último momento que o LED foi atualizado
const char interval = 500;    // tempo de transição entre estados (milisegundos)
String ipLocalString, buff, URL, linha, GLP, FUMACA, retorno, serv, logtxt = "sim", hora_rtc, buf;
const char *json, *LIMITE_MQ2 = "99", *LIMITE_MQ2_FU = "99";
const char *ssid, *password, *servidor, *conslog, *nivelLog = "4";
int contarParaGravar1 = 0, nContar = 0, cont_ip_banco = 0, P_LEITURAS_MQ = 0, nivel_log = 4, estado_atual = 0, estado_antes = 0, freq = 2000, channel = 0, resolution = 8, n = 0, sensorMq2 = 0, agenda_ = 0;
short paramTempo = 60, t = 10, t_temp, u_temp ;
unsigned long time3, time3Param = 90000, timeDht, timeMq2 , tempo = 0, time_sirene;
struct tm data;//Cria a estrutura que contem as informacoes da data.
int hora;
char data_formatada[64];
int ATUALIZAR_DH, i_timer_valor;
String hora_ntp;
//const String comandos_txt = "<p><strong>PORTAS ENTRADA SA&Iacute;DA</strong></p><p>entrada 1 = 32, rele 1 = 33<br />entrada 2 = 25, rele 2 = 18<br />entrada 3 = 14, rele 3 = 27<br />entrada 4 = 12, rele 4 = 13</p><p><strong><span class=\"pl-c1\">LED'S PARA MONITORAMENTO</span></strong></p><p><span class=\"pl-c1\">LED_AZUL&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; 2<br />LED_VERDE&nbsp; &nbsp; &nbsp; &nbsp; &nbsp;4<br />LED_VERMELHO 16</span></p><p><strong><span class=\"pl-c1\">SENSORES</span></strong></p><p><span class=\"pl-c1\">BUZZER&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; 5<br />PIN_MQ2&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;34<br />DHTPIN&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;19</span></p><p><strong>REINCIAR CENTRAL POR COMANDA HTTP</strong>&nbsp;</p><p>HTTP://IP_HOST/?00000</p><p><strong>REINICIAS CONFIGURAÇÕES WIFI</strong>&nbsp;</p><p>HTTP://IP_HOST/?00002</p><p><strong>EXEMPLO NA CHAMADA WEB DESLIGAR LAMPADA</strong>&nbsp;</p><p>HTTP://IP_HOST/?porta=NN&amp;acao=(liga ou&nbsp;desligar)&amp;central=IP_HOST</p><p><strong>CALIBRAR SENSOR MQ2</strong></p><p>HTTP://IP_HOST/?0001</p><p><strong>APAGAR ARQUIVO DE LOG MANUALMENTE</strong></p><p>HTTP://IP_HOST/?00013</p><p><strong>APLICAR CONFIGURA&Ccedil;&Otilde;ES MINIMAS PARA FUNCIONAMENTO DA CENTRAL</strong></p><p>HTTP://IP_HOST/?00014</p><p><strong>DESLIGAR TODOS AS PORTAS OUTPUT DA CENTRAL</strong></p><p>HTTP://IP_HOST/?00015</p><p><strong>APLICAR AS CONFIGURA&Ccedil;&Otilde;ES AP&Oacute;S SEREM GRAVADAS NA CENTRAL</strong>&nbsp;</p><p>HTTP://IP_HOST/?00016</p>";
boolean estado_inter, cont_timer, ler_dht = true;
const int sistema_solar = 0;

//float corrente_s1 = 0.00, tensao_s1 = 0.00, corrente_s2 = 0.00, tensao_s2 = 0.00, corrente_s3 = 0.00, tensao_s3 = 0.00;
float LPGCurve[3]   =  {2.3, 0.20, -0.47};
float COCurve[3]    =  {2.3, 0.72, -0.34};
float SmokeCurve[3] =  {2.3, 0.53, -0.44};
float Ro = 10;

IPAddress ipHost;

WiFiUDP udp;

NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000);//Cria um objeto "NTP" com as configurações.utilizada no Brasil

DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);

void setup() {
  //setCpuFrequencyMhz(80);
  Serial.begin(115200);
  delay(1000);
  Serial.println("-----------------------------------------");
  Serial.print(" SDK: "); Serial.println(ESP.getSdkVersion());
  Serial.print(" FREQUENCIA DA CPU:        "); Serial.print(getCpuFrequencyMhz()); Serial.println("MHz");
  Serial.print(" APB FREQ:                 "); Serial.print(getApbFrequency() / 1000000.0, 1); Serial.println("MHz");
  Serial.print(" MEMORIA FLASH:            "); Serial.print(ESP.getFlashChipSize() / (1024.0 * 1024), 2); Serial.println("MB");
  Serial.print(" MEMORIA FLASH VELOCIDADE: "); Serial.print(ESP.getFlashChipSpeed()/ 1000000.0, 1); Serial.println("MHz");
  Serial.print(" MEMORIA RAM:              "); Serial.print(ESP.getHeapSize() / 1024.0, 2); Serial.println("KB");
  Serial.print(" MEMORIA RAM LIVRE:        "); Serial.print(ESP.getFreeHeap() / 1024.0, 2); Serial.println("KB");
  Serial.print(" MEMORIA RAM ALOCADA:      "); Serial.print(ESP.getMaxAllocHeap() / 1024.0, 2); Serial.println("KB");
  Serial.print(" TAMANHO DO CODIGO:        "); Serial.print(ESP.getSketchSize() / 1024.0, 2);Serial.println("KB");
  Serial.print(" MD5 DO SKET:              "); Serial.println(ESP.getSketchMD5());
  Serial.print(" Chip Mode:                "); Serial.println(ESP.getFlashChipMode());
  Serial.println("-----------------------------------------");
  //---------------------------------------
  //    CONECTANDO A REDE WIFI
  //---------------------------------------
 
  WiFiManager wifiManager;
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str()); //define hostname
  WiFi.getHostname();
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  if (!wifiManager.autoConnect("AUTOMACAO_ESP32", "")) {
    Serial.println(" FALHA NA CONEXÃO! ");
    ESP.restart();
  }
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
  pinMode(LED_AZUL, OUTPUT);
  Serial.println();
  openFS();
  criarArquivo("/param.txt");
  criarArquivo("/log.txt");

  server.begin();
  dht.begin();
  ntp.begin();
  ntp.forceUpdate();
  relogio_ntp(0);

  ipHost = WiFi.localIP();
  addressMac = WiFi.macAddress();
  Serial.println(" MAC Adress: "+addressMac);
  ipLocalString = String(ipHost[0]) + "." + String(ipHost[1]) + "." + String(ipHost[2]) + "." + String(ipHost[3]);

  calibrarSensor();

  arduino_ota();
  
  ArduinoOTA.begin();

  retorno = "SERVIDOR_CONECT";

  ledcSetup(channel, freq, resolution);
  ledcAttachPin(5, channel);
  gravarArquivo("\n *** INICIANDO SISTEMA *** \n \n " + VERSAO, "log.txt");
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
}

void loop()
{

  ArduinoOTA.handle();

  WiFiManager wifiManager;

  pisca_led(LED_VERDE, true);

  while (cont_ip_banco < 1)
  {
    // FAZENDO LEITURA DE PARAMETROS DO SISTEMA
    openFS();
    StaticJsonDocument<680> doc;
    json = lerArquivoParam().c_str();
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
      gravaLog(" " + relogio_ntp(1) + " - E0101:Arquivo json: ", logtxt, 1);
      Serial.println(error.c_str());
      cont_ip_banco++;
      return;
    }
    JsonObject root = doc.as<JsonObject>();
    servidor          = root["sv"];
    botao1.nomeInter  = root["i_1"];
    botao1.tipo       = root["ti_1"];
    botao1.modelo     = root["s_1"];
    botao1.agenda_in  = root["h_i_1"];
    botao1.agenda_out = root["h_o_1"];
    botao1.timer      = root["t_1"];
    botao2.nomeInter  = root["i_2"];
    botao2.tipo       = root["ti_2"];
    botao2.modelo     = root["s_2"];
    botao2.agenda_in  = root["h_i_2"];
    botao2.agenda_out = root["h_o_2"];
    botao3.nomeInter  = root["i_3"];
    botao3.tipo       = root["ti_3"];
    botao3.modelo     = root["s_3"];
    botao3.agenda_in  = root["h_i_3"];
    botao3.agenda_out = root["h_o_3"];
    botao4.nomeInter  = root["i_4"];
    botao4.tipo       = root["ti_4"];
    botao4.modelo     = root["s_4"];
    botao4.agenda_in  = root["h_i_4"];
    botao4.agenda_out = root["h_o_4"];
    conslog           = root["l"];
    logtxt            = String(conslog);
    nivelLog          = root["n"];
    LIMITE_MQ2        = root["mq"];
    LIMITE_MQ2_FU     = root["fu"];
    cont_ip_banco++;
    serv = String(servidor);
    gravaLog(" " + relogio_ntp(1) + " - BD:" + String(servidor), logtxt, 2);
    doc.clear();
    root.clear();
    }
  WiFiClient client = server.available();
  if (Serial.available() > 0) {                                                             // Se receber algo pela serial

    String recebido = leStringSerial();                                                     // Lê toda string recebida
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
    if(quebraString( "rede",recebido))
    {
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
  if ((relogio_ntp(3) == botao1.agenda_in) && (botao1.estado == false) )
  {
    acionaPorta(botao1.rele, "", "liga");
    botao1.estado = true;
  }
  if ((relogio_ntp(3) == botao1.agenda_out) && (botao1.estado == true))
  {
    acionaPorta(botao1.rele, "", "desl");
    botao1.estado = false;
  }
  /*
    ------------------------------------------------
    FIM DA FUNÇÃO AGENDAMENTO
    ------------------------------------------------
    ------------------------------------------------
    VERIFICA SE EXISTE VALOR DE TIMER CONFIGURADO
    ------------------------------------------------
  */
  String s_timer_valor = String(botao1.timer);
  if (s_timer_valor.toInt() > 0)
  {
    if (cont_timer == 1)
    {
      i_timer_valor = s_timer_valor.toInt();
      cont_timer = 0;
    }
    if (botao1.estado)
    {
      int i_timer = i_timer_valor-- ;
      Serial.println( "timer configurado : " + String(i_timer));
      delay(1000);
      if (i_timer_valor == 0)
      {
        acionaPorta(botao1.rele, "", "desl");
        cont_timer = 0;
      }
    }
  }
  /*
    ---------------------------------------
    INICIO DA FUNÇÃO BOTÃO POR PULSO
    ---------------------------------------
  */
  if (String(botao1.modelo) == "pulso")
  {
    if (digitalRead(botao1.entrada) == (String(botao1.tipo).toInt()))
    {
      if (nContar == 0)Serial.println("\n"); Serial.println("\n E1 Pulso");
      while ((digitalRead(botao1.entrada) == (String(botao1.tipo).toInt())) && (nContar <= 300) )
      {
        if (millis() >= tempo + paramTempo)
        {
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
  } else if (String(botao1.modelo) == "interruptor")
  {
    botao1.estado_atual = digitalRead(botao1.entrada);
    if (botao1.estado_atual != botao1.estado_antes )
    {
      if (nContar == 0)Serial.println(" E1 Inter");
      botao1.estado_antes = botao1.estado_atual;
      botao1.contador = 3;
    }
    /*
      ---------------------------------------
      FIM DA FUNÇÃO BOTÃO POR INTERRUPTOR
      ---------------------------------------
      ---------------------------------------
      INICIO DA FUNÇÃO POR PRESENÇA
      ---------------------------------------
    */
    //    boolean PIR_1_STATUS = digitalRead(PIR_1);
    //    if(PIR_1_STATUS)
    //    {
    //      Serial.print(".");
    //      digitalWrite(LED_AZUL, !digitalRead(LED_AZUL));
    //      delay(80);
    //      MV_DETC = true;
    //      MV_DETC_CONTAR = 0;
    //    }
    //    if(MV_DETC == true)
    //      {
    //        if (millis() - TEMP_4 > 1000)
    //        {
    //          TEMP_4 = millis();
    //          MV_DETC_CONTAR++;
    //          Serial.println( " "+String(MV_DETC_CONTAR)+"s");
    //          if(MV_DETC_CONTAR == PIR_1_INTRVL)
    //          {
    //            MV_DETC_CONTAR = 0;
    //            MV_DETC = false;
    //            if(botao1.estado_antes == true)
    //            {
    //              //Desligar lampada
    //              botao1.estado_antes = botao1.estado_atual;
    //              botao1.contador = 3;
    //            }
    //          }
    //        }
    //     }
    //  } else if (s_modelo_1 == "pir")
    //  {
    //    boolean PIR_1_STATUS = digitalRead(PIR_1);
    //    if(PIR_1_STATUS)
    //    {
    //      Serial.println(" Mov. Detec.");
    //      digitalWrite(LED_AZUL, !digitalRead(LED_AZUL));
    //      delay(80);
    //    }
    /*
      ---------------------------------------
      FIM DA FUNÇÃO BOTÃO PRESENÇA
      ---------------------------------------
      ---------------------------------------
      ACIONAMENTO APÓS FUNÇÕES ACIMA
      ---------------------------------------
    */
  }
  if ((botao1.contador >= 1) && (botao1.contador <= 9))
  {
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
  if ((relogio_ntp(3) == botao2.agenda_in) && (botao2.estado == false) )
  {
    acionaPorta(botao2.rele, "", "liga");
    botao2.estado = true;
  }
  if ((relogio_ntp(3) == botao2.agenda_out) && (botao2.estado == true))
  {
    acionaPorta(botao2.rele, "", "desl");
    botao2.estado = false;
  }
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
  if (String(botao2.modelo) == "pulso")
  {
    if (digitalRead(botao2.entrada) == (String(botao2.tipo).toInt()))
    {
      if (nContar == 0)Serial.println("\n"); Serial.println(" E2 Pulso");
      while ((digitalRead(botao2.entrada) == (String(botao2.tipo).toInt())) && (nContar <= 300) )
      {
        if (millis() >= tempo + paramTempo)
        {
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
  if (String(botao2.modelo) == "interruptor")
  {
    botao2.estado_atual = digitalRead(botao2.entrada);
    if (botao2.estado_atual != botao2.estado_antes )
    {
      if (nContar == 0)Serial.println("\n"); Serial.println(" E2 Inter ");
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
  if ((botao2.contador >= 1) && (botao2.contador <= 9))
  {
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
  if (String(botao3.modelo) == "pulso")
  {
    if (digitalRead(botao3.entrada) == (String(botao3.tipo).toInt()))
    {
      if (nContar == 0)Serial.println("\n"); Serial.println(" E3 Pulso");
      while ((digitalRead(botao3.entrada) == (String(botao3.tipo).toInt())) && (nContar <= 300) )
      {
        if (millis() >= tempo + paramTempo)
        {
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
  if (String(botao3.modelo) == "interruptor")
  {
    botao3.estado_atual = digitalRead(botao3.entrada);
    if (botao3.estado_atual != botao3.estado_antes )
    {
      if (nContar == 0)Serial.println("\n"); Serial.print(" E3 Inter");
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
  if ((botao3.contador >= 2) && (botao3.contador <= 9))
  {
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
  if (String(botao4.modelo) == "pulso")
  {
    if (digitalRead(botao4.entrada) == (String(botao4.tipo).toInt()))
    {
      if (nContar == 0)Serial.println("\n"); Serial.println("\n E4 Pulso");
      while ((digitalRead(botao4.entrada) == (String(botao4.tipo).toInt())) && (nContar <= 300) )
      {
        if (millis() >= tempo + paramTempo)
        {
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
  if (String(botao4.modelo) == "interruptor")
  {
    botao4.estado_atual = digitalRead(botao4.entrada);
    if (botao4.estado_atual != botao4.estado_antes )
    {
      if (nContar == 0)Serial.println(" E4 Inter");
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
  if ((botao4.contador >= 2) && (botao4.contador <= 9))
  {
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
    ---------------------------------------
    LEITURA DA REQUISIÇÕES DE CHAMADAS HTTP GET
    ---------------------------------------
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
      ------------------------------------------------------------------------------
      EXEMPLO NA CHAMADA WEB DESLIGAR LAMPADA -
      CHAMADA HTTP EX: HTTP://IP_HOST/?porta=20&acao=desligar&central=IP_HOST
      ------------------------------------------------------------------------------
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
      if (numeroInt == botao1.rele)
      {
        if (acao == "liga")
        {
          botao1.estado = true;
        } else {
          botao1.estado = false;
        }
      }
      if (numeroInt == botao2.rele)
      {
        if (acao == "liga")
        {
          botao2.estado = true;
        } else {
          botao2.estado = false;
        }
      }
      if (numeroInt == botao3.rele)
      {
        if (acao == "liga")
        {
          botao3.estado = true;
        } else {
          botao3.estado = false;
        }
      }
      if (numeroInt == botao4.rele)
      {
        if (acao == "liga")
        {
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
    if (requisicao == "00000")
    {
      gravaLog(" " + relogio_ntp(1) + " - REINICIANDO...", logtxt, 1);
      WiFiClient client = server.available();
      client.println("HTTP/1.1 200 OK");
      ESP.restart();
    }
    if (requisicao == "00001")
    {
      Serial.println(stringUrl);
    }

    /*
      ------------------------------------------------------------------------------
      SE FOR PRESSIONADO BOTÃO BOOT na placa, TODAS AS CONFIGURAÇÕES DA CENTRAL
      SERÃO DELETADAS(WIFI, PARAMETROS, ETC). VARIAVEL DO BOTÃO É PIN_AP.
      ------------------------------------------------------------------------------
    */
    if ( digitalRead(PIN_AP) == LOW || requisicao == "00002" )
    {

      /*
        ------------------------------------------------------------------------------
        Apagando dados de conexão WIFI da central
        ------------------------------------------------------------------------------
      */
      esp_wifi_restore();
      gravaLog(" " + relogio_ntp(1) + " - Apagando config WIFI", logtxt, 1);
      if (!wifiManager.startConfigPortal(" WIFI_AUT", "12345678") )
      {
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
    if (requisicao == "00010")
    {
      gravaLog(" " + relogio_ntp(1) + " - Recalibrando...", logtxt, 2);
      calibrarSensor();
    }
    /*
      ------------------------------------------------------------------------------
      APAGAR ARQUIVO DE LOG MANUALMENTE
      CHAMADA HTTP EX: HTTP://IP_HOST/?00013
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00013")
    {
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
    if (requisicao == "00014")
    {
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
    if (requisicao == "00015") //
    {
      botao1.contador = 11;
    }
    /*
      ------------------------------------------------------------------------------
      APLICAR AS CONFIGURAÇÕES APÓS SEREM GRAVADAS NA CENTRAL
      CHAMADA HTTP EX: HTTP://IP_HOST/?00016
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00016") //
    {
      cont_ip_banco = 0;
    }
    /*
      ------------------------------------------------------------------------------
      FUNÇÃO DE CONTROLE DO ALARME
      ATIVAR ALARME - CHAMADA HTTP EX: HTTP://IP_HOST/?00117
      DESATIVAR ALARME - CHAMADA HTTP EX: HTTP://IP_HOST/?00017
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00018")
    {
      relogio_ntp(0);
    }
    /*
      ------------------------------------------------------------------------------
      PAUSAR LEITURAS DO SENSOR DE GAS
      ------------------------------------------------------------------------------
    */
    if (requisicao == "00019")
    {
      String s_hora_min = relogio_ntp(4);                     //armazenda data e hora 00:00
      String s_hora = String(s_hora_min).substring(0, 2);     //separa o valor da hora
      String s_minuto = String(s_hora_min).substring(3, 5);   //separa o valor do minuto
      int i_minutos = s_minuto.toInt()+1;                     //adiciona minutos ao valor encontrado em s_minuto e converte em inteiro
      String valor_hora_fim = s_hora + ":" + String(i_minutos);
      P_LEITURAS_MQ =1;
    }
    requisicao.remove(0);

    /*
      ------------------------------------------------------------------------------
      GRAVA PARAMETROS NO SPIFFS(SISTEMA DE ARQUIVO) DA CENTRAL, ARQUIVO "param.txt"
      ------------------------------------------------------------------------------
    */
    String codidoExec = stringUrl.substring(10, 15);
    if (codidoExec == "00012")
    {
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
                    + "\"}", "param.txt");
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
  sensorTemp(1);
  sensorTemp(2);
  sensorMQ();
}
