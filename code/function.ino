String leStringSerial() {
  String conteudo = "";
  char caractere;

  // Enquanto receber algo pela serial
  while (Serial.available() > 0) {
    // Lê byte da serial
    caractere = Serial.read();
    // Ignora caractere de quebra de linha
    if (caractere != '\n') {
      // Concatena valores
      conteudo.concat(caractere);
    }
    // Aguarda buffer serial ler próximo caractere
    delay(10);
  }

  //Serial.print(" " + relogio_ntp(1) + " - Recebido na Serial: ");
  //Serial.println(conteudo);

  return conteudo;

  /*CONTEUDO EXEMPLO QUE DEVE SER COLOCADO NA FUNÇÃO LOOP()
      // Se receber algo pela serial
      if (Serial.available() > 0){
        // Lê toda string recebida
        String recebido = leStringSerial();

        if (recebido == "LED1:1"){
          digitalWrite(led, HIGH);
        }

        if (recebido == "LED1:0"){
          digitalWrite(led, LOW);
        }
      }
  */
}
String gpio_html (int numero, int botao_entrada, int botao_rele, String botao_nomeInter, const char* botao_tipo, const char* botao_modelo, const char*botao_agenda_in, const char*botao_agenda_out)
{
  String buff;
  buff += "<div class=\"row\">";
  buff += "   <div class=\"col-sm-2\">";
  buff += "     <strong>GPIO E " + String(botao_entrada) + " S " + String(botao_rele) + "</strong><input maxlength=\"10\" style=\"width:100px\" type=\"text\"   name=\"int_" + String(numero) + "\" value=\"" + String(botao_nomeInter) + "\">";
  buff += "   </div>";
  buff += "   <div class=\"col-sm-2\">";
  buff += "      <strong> Sinal </strong><select   style=\"width:80px\"  name=\"tipo_" + String(numero) + "\"><option value=\"0\" " + selectedHTNL(botao_tipo, "0") + "> - </option><option value=\"1\" " + selectedHTNL(botao_tipo, "1") + "> + </option></select>";
  buff += "   </div>";
  buff += "   <div class=\"col-sm-2\">";
  buff += "      <strong>Tipo </strong><select   style=\"width:100px\" name=\"sinal_" + String(numero) + "\"><option value=\"pulso\" " + selectedHTNL(botao_modelo, "pulso") + "> Pulso</option><option value=\"interruptor\" " + selectedHTNL(botao_modelo, "interruptor") + ">Inter.</option><option value=\"pir\" " + selectedHTNL(botao_modelo, "pir") + ">PIR</option></select>";
  buff += "   </div>";
  buff += "   <div class=\"col-sm-4\">";
  buff += "     <strong>Agenda </strong>";
  String input_text_ = "<input maxlength=\"2\" style=\"width:26px\" type=\"text\"";
  buff += input_text_ + "name=\"hora" + String(numero) + "_in_1\" value=\"" + opcao_agenda(botao_agenda_in, botao_agenda_out, 1) + "\">:";
  buff += input_text_ + "name=\"hora" + String(numero) + "_in_2\" value=\"" + opcao_agenda(botao_agenda_in, botao_agenda_out, 2) + "\">-";
  buff += input_text_ + "name=\"hora" + String(numero) + "_out_1\" value=\"" + opcao_agenda(botao_agenda_in, botao_agenda_out, 3) + "\">:";
  buff += input_text_ + "name=\"hora" + String(numero) + "_out_2\" value=\"" + opcao_agenda(botao_agenda_in, botao_agenda_out, 4) + "\">";
  buff += "   </div>";
  buff += "   <div class=\"col-sm-2\">";
  buff += "     <strong>Timer</strong>";
  buff += "     <input maxlength=\"4\" style=\"width:80px\" type=\"text\" name=\"timer_" + String(numero) + "\">";
  buff += "   </div>";
  buff += "</div>";
  return buff;
}

boolean status_porta(int numero_Int, int rele, boolean estado, String _acao)
{
  if (numero_Int == rele)
  {
    if (_acao == "liga")
    {
      estado = true;
    } else {
      estado = false;
    }
  }
  return estado;
}
String opcao_agenda(const char *in, const char *out, int saida)
{
  String resultado;
  if (saida == 1) {
    resultado = String(in).substring(0, 2);
  }
  if (saida == 2) {
    resultado = String(in).substring(2, 4);
  }
  if (saida == 3) {
    resultado = String(out).substring(0, 2);
  }
  if (saida == 4) {
    resultado = String(out).substring(2, 4);
  }
  return resultado;

}

boolean agendamento(int gpio, String hora_ini, String hora_fim, String hora_atual, boolean estado )
{
  boolean estado_fim;
  /*
  	char agenda[2][12] = {"21:04:00", "21:05:00"};
  	String timer = timeClient.getFormattedTime();
  	agendamento(led, agenda[0], agenda[1], timer);
  */
  if ((hora_atual == hora_ini) && (estado == false) )
  {
    acionaPorta(gpio, "", "liga");
    estado_fim = true;
  }
  if ((hora_atual == hora_fim) && (estado == true))
  {
    acionaPorta(gpio, "", "desl");
    estado_fim = false;
  }
  hora_ini      = "";
  hora_fim      = "";
  hora_atual    = "";
  return estado_fim;
}

void arduino_ota()
{
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
}

String relogio_ntp(int retorno)
{
  if (retorno == 0 || ATUALIZAR_DH == 0)
  {
    //Serial.println(" Atualizando data e hora...");
    ntp.update();
    hora = ntp.getEpochTime(); //Atualizar data e hora usando NTP online
    //Serial.print(" NTP Unix: ");
    //Serial.println(hora);
    ntp.getFormattedTime();
    timeval tv;//Cria a estrutura temporaria para funcao abaixo.
    tv.tv_sec = hora;//Atribui minha data atual. Voce pode usar o NTP para isso ou o site citado no artigo!
    settimeofday(&tv, NULL);//Configura o RTC para manter a data atribuida atualizada.
    //strftime(data_formatada, 64, "%d/%m/%Y %H:%M:%S", &data);//Cria uma String formatada da estrutura "data"
    //Serial.print(" Data e hora atualizada:");
    //Serial.println(data_formatada);
    ATUALIZAR_DH = 1;
  }
  time_t tt = time(NULL);//Obtem o tempo atual em segundos. Utilize isso sempre que precisar obter o tempo atual
  data = *gmtime(&tt);//Converte o tempo atual e atribui na estruturaacao_porta
  if (retorno == 1)
  {
    //time_t tt = time(NULL);//Obtem o tempo atual em segundos. Utilize isso sempre que precisar obter o tempo atual
    //data = *gmtime(&tt);//Converte o tempo atual e atribui na estrutura
    //strftime(data_formatada, 64, "%d/%m/%Y %H:%M:%S", &data);//Cria uma String formatada da estrutura "data"
    strftime(data_formatada, 64, "%d/%m/%y %H:%M:%S", &data);//Cria uma String formatada da estrutura "data"
    hora_ntp   = data_formatada;
  }
  if (retorno == 2)
  {
    //time_t tt = time(NULL);//Obtem o tempo atual em segundos. Utilize isso sempre que precisar obter o tempo atual
    //data = *gmtime(&tt);//Converte o tempo atual e atribui na estrutura
    strftime(data_formatada, 64, "%d/%m/%Y", &data);//Cria uma String formatada da estrutura "data"
    hora_ntp = data_formatada;
  }
  if (retorno == 3)
  {
    //time_t tt = time(NULL);//Obtem o tempo atual em segundos. Utilize isso sempre que precisar obter o tempo atual
    //data = *gmtime(&tt);//Converte o tempo atual e atribui na estrutura
    strftime(data_formatada, 64, "%H%M", &data);//Cria uma String formatada da estrutura "data"
    hora_ntp = data_formatada;

  }
  if (retorno == 4)
  {
    //time_t tt = time(NULL);//Obtem o tempo atual em segundos. Utilize isso sempre que precisar obter o tempo atual
    //data = *gmtime(&tt);//Converte o tempo atual e atribui na estrutura
    strftime(data_formatada, 64, "%H:%M", &data);//Cria uma String formatada da estrutura "data"
    hora_ntp = data_formatada;

  }
  return hora_ntp;

}

void pisca_led(int LED, boolean estado)
{
  if (estado == true)
  {
    if (millis() - milis > interval)
    {
      milis = millis();
      digitalWrite(LED, !digitalRead(LED));
    }
  } else
  {
    digitalWrite(LED, LOW);
  }
}

void gravaLog(String mensagem, String permissao, int nivel)
{
  for (int i = 0; i <= 0 ; i++ )
  {
    if (permissao == "sim")
    {
      if ( String(nivel) <= String(nivelLog))
      {
        gravarArquivo(mensagem, "log.txt");
      }
    }
  }
}

String selectedHTNL(const char* tipo, String comp )
{
  String select;
  String s_tipo = String(tipo);
  String s_comp = String(comp);
  if (s_tipo == comp) {
    return select = "selected";
  } else {
    return select = "";
  }
}

String quebraString(String txtMsg, String string)
{
  unsigned int tamanho = txtMsg.length();
  int inicio_string = string.indexOf(txtMsg) + tamanho + 1;
  int final_string = string.indexOf("&", inicio_string);
  String resultado = string.substring(inicio_string, final_string);
  return resultado;
}

//---------------------------------------
//    FUNÇÃO PARA ACIONAMENTO DE PORTAS
//    GPIO
//---------------------------------------
void acionaPorta(int numeroF, String portaF, String acaoF) {
  gravaLog(" " + relogio_ntp(1) + " - Comando:" + String(numeroF) + "/" + acaoF, logtxt, 4);
  String acao_porta;
  if (acaoF == "liga") {
    digitalWrite(numeroF, HIGH );
    acao_porta = "&acao=liga&central=";
  } else if (acaoF == "desl") {
    digitalWrite(numeroF, LOW);
    acao_porta = "&acao=desliga&central=";
    linha = "";
  } else if (acaoF == "puls") {
    acao_porta = "&acao=pulso&central=";
    digitalWrite(numeroF, HIGH);
    delay(1000);
    digitalWrite(numeroF, LOW);
  }
  linha = "porta=" + String(numeroF) + acao_porta + ipLocalString;
  gravarBanco(linha);
  linha = "";
}

String teste_conexao() {
  WiFiClient client = server.available();
  if (millis() >= time3 + time3Param)
  {
    time3 = millis();
    if (servidor != "0")
    {
      int r = client.connect(servidor, portaServidor);
      if (r == 0)
      {
        WiFi.reconnect();
        gravaLog(" " + relogio_ntp(1) + " - E0104:Servidor Desconectado", logtxt, 1);
        retorno = "ERRO_SERVIDOR_DESC";
        pisca_led(LED_VERMELHO, true);

      } else if (r == 1)
      {
        Serial.println(" Servidor WEB/Banco OK ");
        retorno = "SERVIDOR_CONECT";
        pisca_led(LED_VERMELHO, false);
      }
    }
  }
  return retorno;
}

//---------------------------------------
//    FUNÇÃO PARA GRAVAR NO BANCO
//---------------------------------------
void gravarBanco (String buffer) {
  WiFiClient client = server.available();
  if (WiFi.status() != WL_CONNECTED)
  {
    gravaLog(" " + relogio_ntp(1) + " - E0105:Reconectando WIFI!", logtxt, 1);
    WiFi.reconnect();
  }
  //pisca_led(LED_VERMELHO,false);
  if ((client.connect(servidor, portaServidor) == true) && (teste_conexao() == "SERVIDOR_CONECT"))
  {
    //if (client.connect(servidor, 80)) {
    client.println("GET /web/gravar.php?" + buffer);
    gravaLog(" " + relogio_ntp(1) + " - BD: " + buffer, logtxt, 4);
    client.println();
    buffer = "";
  } else {
    gravaLog(" " + relogio_ntp(1) + " - E0104:Servidor Desconectado", logtxt, 1);
    buffer = "";
  }
  client.flush();
  client.stop();
}
//---------------------------------------


//---------------------------------------
//    FUNÇÃO DA SIRENE
//---------------------------------------
void sirene(boolean valor)
{
  if (valor == true)
  {
    if (millis() - time_sirene > 1000)
    {
      digitalWrite(BUZZER, true);
      time_sirene = millis();
    } else {
      digitalWrite(BUZZER, false);
    }
  }
  if (valor == false)
  {
    digitalWrite(BUZZER, false);
  }
}

//---------------------------------------
//    FUNÇOES DO MQ / SENSOR DE GAS
//---------------------------------------
void calibrarSensor()
{
  //CALIBRACAO INCIAL DO SENSOR DE GAS
  Serial.print(" Caligrando sensor");
  Ro = MQCalibration(PIN_MQ2);
  Serial.println("\n Calibrado 'Ro' = " + String(Ro) + " kohm");
}

float calcularResistencia(int tensao)   //funcao que recebe o tensao (dado cru) e calcula a resistencia efetuada pelo sensor. O sensor e a resistência de carga forma um divisor de tensão.
{
  return (((float)VRL_VALOR * (4095 - tensao) / tensao));
}

float MQCalibration(int mq_pin)   //funcao que calibra o sensor em um ambiente limpo utilizando a resistencia do sensor em ar limpo 9.83
{
  int i;
  float valor = 0;
  pisca_led(LED_VERDE, false);
  for (i = 0; i < ITERACOES_CALIBRACAO; i++) { //sao adquiridas diversas amostras e calculada a media para diminuir o efeito de possiveis oscilacoes durante a calibracao
    Serial.print(".");
    valor += calcularResistencia(analogRead(mq_pin));
    delay(500);
    digitalWrite(2, !digitalRead(2));//Faz o LED piscar (inverte o estado).
  }
  digitalWrite(2, false);
  valor = valor / ITERACOES_CALIBRACAO;
  valor = valor / RO_FATOR_AR_LIMPO; //o valor lido dividido pelo R0 do ar limpo resulta no R0 do ambiente
  return valor;
}
float leitura_MQ2(int mq_pin)
{
  int i;
  float rs = 0;
  pisca_led(LED_VERDE, false);
  for (i = 0; i < ITERACOES_LEITURA; i++)
  {
    rs += calcularResistencia(analogRead(mq_pin));
    digitalWrite(2, !digitalRead(2));//Faz o LED piscar (inverte o estado).
  }
  digitalWrite(2, false);
  rs = rs / ITERACOES_LEITURA;
  return rs;
}
int getQuantidadeGasMQ(float rs_ro, int gas_id)
{
  if ( gas_id == 0 ) {
    return calculaGasPPM(rs_ro, LPGCurve);
  } else if ( gas_id == 1 ) {
    return calculaGasPPM(rs_ro, COCurve);
  } else if ( gas_id == 2 ) {
    return calculaGasPPM(rs_ro, SmokeCurve);
  }

  return 0;
}
int  calculaGasPPM(float rs_ro, float * pcurve) //Rs/R0 é fornecido para calcular a concentracao em PPM do gas em questao. O calculo eh em potencia de 10 para sair da logaritmica
{
  return (pow(10, ( ((log(rs_ro) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}

//---------------------------------------
//    FUNÇÃO DE MANIPULAÇÃO DE ARQUIVOS
//---------------------------------------
void formatFS() {
  SPIFFS.format();
}
void criarArquivo(String nomeArquivo) {
  File wFile;
  //Cria o arquivo se ele não existir
  if (SPIFFS.exists(nomeArquivo)) {
    Serial.println(" Arquivo " + nomeArquivo + " já existe!");
  } else {
    Serial.println(" Criando arquivo " + nomeArquivo + ".");
    wFile = SPIFFS.open(nomeArquivo, "w+");
    //Verifica a criação do arquivo
    if (!wFile) {
      gravaLog(" " + relogio_ntp(1) + " - E0109:Criar arquivo " + nomeArquivo, logtxt, 1);
    } else {
      Serial.println(" Arquivo " + nomeArquivo + " criado!");
    }
  }
  wFile.close();
}


void deletarArquivo(String arquivo) {
  //Remove o arquivo
  if (!SPIFFS.remove(arquivo)) {
    gravaLog(" " + relogio_ntp(1) + " - E0105:Remover arquivo " + arquivo, logtxt, 1);
  } else {
    Serial.println(" Arquivo " + arquivo + " removido!");
  }
}

void gravarArquivo(String msg, String arq) {
  //Abre o arquivo para adição (append)
  //Inclue sempre a escrita na ultima linha do arquivo
  if (arq == "log.txt")
  {
    File logg = SPIFFS.open("/log.txt", "a+");
    int s = logg.size(); // verificar tamanho do arquivo
    if (s >= 15000) {
      deletarArquivo("/log.txt");
      criarArquivo("/log.txt");
      //delay(5);
      gravaLog(" " + relogio_ntp(1) + " - Log deletado! ", logtxt, 1);
    }
    if (!logg) {
      //Gravando log de Ena central.
      Serial.println(" E0108:Abrir arquivo " + arq);
    } else {
      logg.println(msg);
      Serial.println(msg);
    }
    logg.close();
  }

  if (arq == "param.txt")
  {
    File param1 = SPIFFS.open("/param.txt", "a+");
    if (!param1) {
      gravaLog(" " + relogio_ntp(1) + " - E0106:Abrir arquivo " + arq, logtxt, 1);
    } else {
      param1.println(msg);
      gravaLog(" " + relogio_ntp(1) + " - Gravando: " + msg, logtxt, 1);
    }
    param1.close();
  }
}

String lerLog() {
  //Faz a leitura do arquivo
  String buff;
  File ARQUIVO = SPIFFS.open("/log.txt", "r");
  int tamanhoLog = ARQUIVO.size(); // verificar tamanho do arquivo
  buff = "Log : " + String(tamanhoLog) + "<br />";
  while (ARQUIVO.available()) {
    String line = ARQUIVO.readStringUntil('\n');
    buff += line + "<br />";
  }
  ARQUIVO.close();
  return buff;
}
String lerArquivoParam(void) {
  String buff;
  File ARQUIVO = SPIFFS.open("/param.txt", "r");
  while (ARQUIVO.available()) {
    String line = ARQUIVO.readStringUntil('\n');
    buff += line;
  }
  ARQUIVO.close();
  return buff;
}
void closeFS() {
  SPIFFS.end();
}
void openFS() {
  //Abre o sistema de arquivos
  SPIFFS.begin(true);
  if (!SPIFFS.begin()) {
    gravaLog(" " + relogio_ntp(1) + " - E0107:Sistema arquivo", logtxt, 1);
  } else {
    gravaLog(" " + relogio_ntp(1) + " - Sistema arquivos ok!", logtxt, 4);
  }
}

//callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager * myWiFiManager) {
  Serial.println(" Entrou no modo de configuração ");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede

}

//callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
  //  Serial.println("Should save config");
  Serial.println(" Configuração salva ");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
}

void listDir(fs::FS & fs, const char * dirname, uint8_t levels) {
  Serial.printf("Dir.: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- Falha ao abrir diretório");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - Diretório não encontrado");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  Arquivo: ");
      Serial.print(file.name());
      Serial.print("\tTamanho: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
