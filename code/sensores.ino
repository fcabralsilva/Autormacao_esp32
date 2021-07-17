/*
   FUNÇÃO QUE REALIZA LEITURA DO SENSOR DE TENSÃO DC 25V
*/
float tensao_dc(int SENSOR_T_DC )
{
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
  for (i = 0; i < 20; i++)
  {
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
  ROTINA DO SENSOR DHT11
  --------------------------------
*/
float sensorTemp(int sensor) {
  float umidade, temperatura, valor_s;
  if (millis() >= timeDht + (30000) || ler_dht == true)
  {
    ler_dht = false;

    umidade = dht.readHumidity();
    temperatura = dht.readTemperature();
    
    if (umidade < 101 || temperatura < 50) {
      t_temp = temperatura;
      u_temp = umidade;
    }
    gravaLog(" " + relogio_ntp(1) + " - T:" + temperatura + " U:" + String(int(umidade)), logtxt, 4);

    //Verifique se alguma leitura falhou e saia mais cedo (para tentar novamente)
    if (isnan(umidade) || isnan(temperatura)) {
      gravaLog(" " + relogio_ntp(1) + " - E0109:DHT11", logtxt, 1);
      temperatura = t_temp;
      umidade     = u_temp;
    }
    timeDht = millis();
  }
  if(sensor == 1)
  {
    valor_s = u_temp;
  }
  if(sensor == 2)
  {
    valor_s = t_temp;
  }
  return valor_s;
}

void sensorMQ() {
  /*
    --------------------------------
    ROTINA DO SENSOR MQ-2
    --------------------------------
  */
  if (millis() >= timeMq2 + 15000)
  {
    sensorMq2 = analogRead(PIN_MQ2);
    
    digitalWrite(LED_AZUL, HIGH);
    delay(100);
    digitalWrite(LED_AZUL, LOW);
    
    GLP = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, GAS_LPG) );
    FUMACA = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, SMOKE));
    String CO = String(getQuantidadeGasMQ(leitura_MQ2(PIN_MQ2) / Ro, GAS_CO)  );
    
    if ((GLP.toInt())   > 1000) GLP     = "0";
    if ((FUMACA.toInt())> 1000) FUMACA  = "0";
    if ((CO.toInt())    > 1000) CO      = "0";
    
    contarParaGravar1++;
    
    gravaLog(" " + relogio_ntp(1) + " - MQ2:" + String(sensorMq2) + " GLP:" + GLP + " CO:" + CO + " FU:" + FUMACA + " L:" + contarParaGravar1, logtxt, 4);
    
    timeMq2 = millis();
    
    if ((GLP.toInt() >= String(LIMITE_MQ2).toInt()) || (FUMACA.toInt() > String(LIMITE_MQ2_FU).toInt()))
    {
      if (P_LEITURAS_MQ == 0)
      {
        sirene(true);
        digitalWrite(LED_VERMELHO, true);
      }
    } else
    {
      digitalWrite(LED_VERMELHO, false);
      sirene(false);
    }
    if (P_LEITURAS_MQ == 1)
    {
      sirene(false);
    }
    //GRAVA NO BANCO O VALOR LIDO APOS X LEITURAS
    if ((contarParaGravar1 == 20) || (GLP.toInt() >= String(LIMITE_MQ2).toInt()) || (FUMACA.toInt() > String(LIMITE_MQ2_FU).toInt()) )
    {
      buff = "sensor=mq-2&valor=mq-2;" + String(GLP) + ";&central=" + String(ipLocalString) + "&p=" + String(PIN_MQ2);
      gravarBanco (buff);
      contarParaGravar1 = 0;
      buff.remove(0);
    }
  }
}
