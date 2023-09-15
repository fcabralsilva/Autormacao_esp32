/*
   FUNÇÃO QUE REALIZA LEITURA DO SENSOR DE TENSÃO DC 25V
*/
float tensao_dc(int SENSOR_T_DC) {
  //VARIÁVEL PARA ARMAZENAR O VALOR DE TENSÃO DE ENTRADA DO SENSOR
  float tensaoEntrada = 0.0;
  float lt = 0.0;
  //VARIÁVEL PARA ARMAZENAR O VALOR DA TENSÃO MEDIDA PELO SENSOR
  float tensaoMedida = 0.0;
  //VALOR DO RESISTOR 1 DO DIVISOR DE TENSÃO
  float valorR1 = 30000;
  // VALOR DO RESISTOR 2 DO DIVISOR DE TENSÃO
  float valorR2 = 7500;
  //VARIÁVEL PARA ARMAZENAR A LEITURA DO PINO ANALÓGICO
  int leituraSensor = 0;
  //FAZ A LEITURA DO PINO ANALÓGICO E ARMAZENA NA VARIÁVEL O VALOR LIDO
  int i;
  for (i = 0; i < 20; i++) {
    float lt = analogRead(SENSOR_T_DC);
    leituraSensor = leituraSensor + lt;
  }
  //VARIÁVEL RECEBE O RESULTADO DO CÁLCULO
  tensaoEntrada = ((leituraSensor / 20) * 3.6) / 4096.0;
  //VARIÁVEL RECEBE O VALOR DE TENSÃO DC MEDIDA PELO SENSOR
  tensaoMedida = tensaoEntrada / (valorR2 / (valorR1 + valorR2));
  leituraSensor = 0;
  tensaoEntrada = 0.0;
  return tensaoMedida;
}
/*
  --------------------------------
  ROTINA DO SENSOR DHT XX
  --------------------------------
*/
float sensorTemp(int sensor) {
  float umidade_dht_f, temperatura_dht_f, valor_s;
  sensors_event_t event;
  if (millis() >= timeDht + (50000) || ler_dht == true) {
    if ((umidade_dht_f < 101 || temperatura_dht_f < 70) || (umidade_dht_f > 200 || temperatura_dht_f > 200)) {
      /*
      * LEITURA DA TEMPERATURA E UMIDADE DHTXX
      */
      dht.temperature().getEvent(&event);
      dht.humidity().getEvent(&event);
      if (isnan(event.temperature) || isnan(event.relative_humidity) ) {
        gravaLog(" " + relogio_ntp(1) + " - E0109:DHT" + String(DHTTYPE) + " ERRO DE LEITURAS", logtxt, 3);
        temperatura_dht_f = 0.0;
        umidade_dht_f = 0.0;
      } else {
        temperatura_dht_f = event.temperature;
        umidade_dht_f = event.relative_humidity;
      }
      /*
       * LEITURA DA UMIDADE DHTXX
       */
      // dht.humidity().getEvent(&event);
      // if (isnan(event.relative_humidity)) {
      //   gravaLog(" " + relogio_ntp(1) + " - E0109:DHT" + String(DHTTYPE) + "ERRO LEITURA UMIDADE", logtxt, 1);
      //   umidade_dht_f = 0.0;
      // } else {
      //   umidade_dht_f = event.relative_humidity;
      // }
    } else {
      gravaLog(" " + relogio_ntp(1) + " - E0109-01:DHT" + String(DHTTYPE), logtxt, 1);
    }
    /*
     * VERIFICAR SE HOUVE ERRO DE LEITURA
     */
    //Verifique se alguma leitura falhou e saia mais cedo (para tentar novamente)
    //if (isnan(umidade_dht_f) || isnan(temperatura_dht_f)) {
      //gravaLog(" " + relogio_ntp(1) + " - E0109-1:DHT" + String(DHTTYPE), logtxt, 1);
    //} else {
      //gravaLog(" " + relogio_ntp(1) + " - T:" + String(int(temperatura_dht_f)) + " U:" + String(int(umidade_dht_f)), logtxt, 4);
    //}
    timeDht = millis();
  }

  /*
   * GERANDO VALOR DE SAIDA
   */
  if (sensor == 1) {
    valor_s = umidade_dht_f;
  }
  if (sensor == 2) {
    valor_s = temperatura_dht_f;
  }
  return valor_s;
}

void gravaDhtArray(){
  contaLeituraDht++;
  if(conta_temperatura <= conta_temperatura_valor)
  {
    linha_tr_tabela = "<tr><td><span style=\"font-family:arial,helvetica,sans-serif;\">";
    linha_tr_tabela += relogio_ntp(1);
    linha_tr_tabela += "</span></td><td style=\"text-align: center;\"><span style=\"font-family:arial,helvetica,sans-serif;\">";
    linha_tr_tabela += String(sensorTemp(2));
    linha_tr_tabela += "</span></td><td style=\"text-align: center;\"><span style=\"font-family:arial,helvetica,sans-serif;\">";
    linha_tr_tabela += String(sensorTemp(1));
    linha_tr_tabela += "</span></td> </tr>";
    temperatura[conta_temperatura] = linha_tr_tabela;
    Serial.println( " Valor:"+String(conta_temperatura) + String(temperatura[conta_temperatura]));
    conta_temperatura++;
  }else{
    // conta_temperatura = 0;
    String linha_tr_tabela = "<tr><td><span style=\"font-family:arial,helvetica,sans-serif;\">";
    linha_tr_tabela += relogio_ntp(1);
    linha_tr_tabela += "</span></td><td style=\"text-align: center;\"><span style=\"font-family:arial,helvetica,sans-serif;\">";
    linha_tr_tabela += String(sensorTemp(2));
    linha_tr_tabela += "</span></td><td style=\"text-align: center;\"><span style=\"font-family:arial,helvetica,sans-serif;\">";
    linha_tr_tabela += String(sensorTemp(1));
    linha_tr_tabela += "</span></td> </tr>";
    //temperatura[conta_temperatura] = linha_tr_tabela;
    // conta_temperatura++;
    // temperatura[0] = temperatura[1];
    // temperatura[1] = temperatura[2];
    // temperatura[2] = temperatura[3];
    // temperatura[3] = temperatura[4];
    // temperatura[4] = temperatura[5];
    // temperatura[5] = temperatura[6];
    // temperatura[6] = temperatura[7];
    // temperatura[7] = temperatura[8];
    // temperatura[8] = temperatura[9];
    // temperatura[9] = linha_tr_tabela;
    int b = 0;
    for(int i=0; i< conta_temperatura -1;i++){
      temperatura[i] = temperatura[i+1];
      b++;
    }
    temperatura[b] = linha_tr_tabela;
    
    //Serial.println( " Valor:"+String(conta_temperatura) + String(temperatura[conta_temperatura]));

    // String inf_Temp_Umid = "";
    // for(int i = 0; i<= 8 ; i++ ){
    //     inf_Temp_Umid += temperatura[i];                // TRANSFORMANDO ARRAY EM STRING PARA SALVAR NA MEMORIA O HISTÓRICO
    // }
    // gravarArquivo(inf_Temp_Umid, "sensor_dht.txt");
  }
}

void sensorMQ() {
  /*
    --------------------------------
    ROTINA DO SENSOR MQ-2
    --------------------------------
  */
  if (millis() >= timeMq2 + 15000) {
    sensorMq2 = analogRead(PIN_MQ2);

    GLP = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, GAS_LPG));
    FUMACA = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, SMOKE));
    CO = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, GAS_CO));

    if ((GLP.toInt()) > 1000) GLP = "0";
    if ((FUMACA.toInt()) > 1000) FUMACA = "0";
    if ((CO.toInt()) > 1000) CO = "0";

    contarParaGravar1++;

    gravaLog(" " + relogio_ntp(1) + " - MQ2:" + String(sensorMq2) + " GLP:" + GLP + " CO:" + CO + " FU:" + FUMACA + " L:" + contarParaGravar1, logtxt, 4);

    timeMq2 = millis();

    if ((GLP.toInt() >= String(LIMITE_MQ2).toInt()) || (FUMACA.toInt() > String(LIMITE_MQ2_FU).toInt())) {
      if (P_LEITURAS_MQ == 0) {
        sirene(true);
        digitalWrite(LED_VERMELHO, true);
      }
    } else {
      digitalWrite(LED_VERMELHO, false);
      sirene(false);
    }
    if (P_LEITURAS_MQ == 1) {
      sirene(false);
    }
    //GRAVA NO BANCO O VALOR LIDO APOS X LEITURAS
    if ((contarParaGravar1 == 20) || (GLP.toInt() >= String(LIMITE_MQ2).toInt()) || (FUMACA.toInt() > String(LIMITE_MQ2_FU).toInt())) {
      buff = "sensor=mq-2&valor=mq-2;" + String(GLP) + ";&central=" + String(ipLocalString) + "&p=" + String(PIN_MQ2);
      gravarBanco(buff);
      contarParaGravar1 = 0;
      buff.remove(0);
    }
  }
}
