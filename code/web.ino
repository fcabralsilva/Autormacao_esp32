const char WEB_HEAD[] PROGMEM            = "<!DOCTYPE html><html lan8g=\"pt-br\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><meta charset=\"utf-8\"><link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\"><script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script><title>{v}</title>";
const char WEB_STYLE[] PROGMEM           = "<style type=\"text/css\">body .form-control{font-size:12px}input,button,select,optgroup,textarea {  margin: 5px;}.table td, .table th {padding:0px;border:0px}.th {width:100px;}.shadow-lg {box-shadow: 0px } #collapseExample {font-size:10px}</style>";
/*const char WEB_SCRIPT[] PROGMEM          = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>"; */
const char WEB_HEAD_END[] PROGMEM        = "</head><body>";
const char WEB_END[] PROGMEM             = "</div></body></html>";
const char WEB_DIV_CONTAINER[] PROGMEM   = "<div class=\"container shadow-lg p-3 mb-5 bg-white rounded\">";
const char WEB_NAV_MENU[] PROGMEM        = "<ul class=\"nav nav-pills mb-3\" id=\"pills-tab\" role=\"tablist\"><li class=\"nav-item\"><a class=\"nav-link active\" id=\"pills-home-tab\" data-toggle=\"pill\" href=\"#pills-home\" role=\"tab\" aria-controls=\"pills-home\" aria-selected=\"true\">Home</a></li> <li class=\"nav-item\"><a class=\"nav-link\" id=\"pills-profile-tab\" data-toggle=\"pill\" href=\"#pills-profile\" role=\"tab\" aria-controls=\"pills-profile\" aria-selected=\"false\">Configuração</a></li></ul>";
const char WEB_BOTAO_SUCCESS[] PROGMEM   = "<a href=\"?porta=\"{A}\" title=\"Porta:\"{B}\"><button type=\"button\"  class=\"btn btn-success\">\"{C}\"</button></a>";
const char A_HREF[] PROGMEM              = "  <a href=\"?porta={A}&acao={G}&central={B}\" title=\"Porta:{C} Botão:{D}\"><button type=\"button\"  class=\"{E}\">{F}</button></a>";
const char TABELA_TEMP_UMID[] PROGMEM    = "<table align=\"left\" border=\"0\" cellpadding=\"1\" cellspacing=\"0\" dir=\"ltr\" style=\"\"> <thead> <tr style=\"background:#007bff; color:white\"> <th scope=\"col\" style=\"text-align: center;\"> <span style=\"font-family:arial,helvetica,sans-serif;\">Data Hora</span></th> <th scope=\"col\" style=\"text-align: center;\"> <span style=\"font-family:arial,helvetica,sans-serif;\">Temperatura °C</span></th> <th scope=\"col\" style=\"text-align: center;\"> <span style=\"font-family:arial,helvetica,sans-serif;\">Umidade %</span></th> </tr> </thead> <tbody>"; 
//const char TABELA_TEMP_UMID_2[] PROGMEM  = "<tr> <td> <span style=\"font-family:arial,helvetica,sans-serif;\">15/06/2023 17:00</span></td> <td style=\"text-align: center;\"> <span style=\"font-family:arial,helvetica,sans-serif;\">25.0</span></td> <td style=\"text-align: center;\"> <span style=\"font-family:arial,helvetica,sans-serif;\">99</span></td> </tr>";
const char TABELA_TEMP_UMID_3[] PROGMEM  = "</tbody> </table>";

String pagina() {
  /*
    --------------------------------
    MONTAR PAGINA WEB DA CENTRAL
    --------------------------------
  */
  String buf = FPSTR(WEB_HEAD);
  buf.replace("{v}", "Central Automação");
  //buf += FPSTR(WEB_SCRIPT);
  buf += FPSTR(WEB_STYLE);
  buf += FPSTR(WEB_HEAD_END);
  buf += FPSTR(WEB_DIV_CONTAINER);
  buf += FPSTR(WEB_NAV_MENU);

  /*
     --------------------------------
     DIV PRINCIPAL
     --------------------------------
  */
  buf += "<div class=\"tab-content\" id=\"pills-tabContent\">";
  buf += "<div class=\"tab-pane fade show active\" id=\"pills-home\" role=\"tabpanel\" aria-labelledby=\"pills-home-tab\">";
  buf += "<div style=\"float: right;\"></div>";
  /*
     --------------------------------
     PAINEL DE SENSORES
     --------------------------------
  */
  buf += "<div class=\"row\">";
  
  //SENSOR DE TEMPERATURA INTERNO
  //buf += "<div class=\"col-sm-2\">";
  //buf += "<h4>&#127777;<font size=\"5\"> ";
  //buf += sensorTemp(2);
  //buf += "</font><sup class=\"units\">&deg;c</sup></h4>";
  //buf += "<h7>&#127777;<font size=\"2\"> ";
  //buf += temp_ext;
  //buf += "</font><sup class=\"units\">&deg;c</sup></h7>";
  //buf += "<h7>&#128167;<font size=\"2\"> " + String(umid_ext) + "<sup class=\"units\">%</sup></font></h7>";
  //buf += "</div>";

  //SENSOR DE UMIDADE DHTXX
  //buf += "<div class=\"col-sm-2\">";
  int umid = sensorTemp(1);
  //buf += "<h3>&#128167;<font size=\"5\"> " + String(umid) + "<sup class=\"units\">%</sup></font></h3>";
  // if (umid >= 60)
  // {
  //   buf += "<span class=\"badge badge-pill badge-success\" data-toggle=\"tooltip\" title=\"Nível recomendado\"> . </span>";
  // } else if (umid > 31 && umid < 60)
  // {
  //   buf += "<span class=\"badge badge-pill badge-warning\"> ! </span>";
  // } else if (umid <= 30)
  // {
  //   buf += "<span class=\"badge badge-pill badge-danger\"> ! </span>";
  // }
  //buf += "</div>";
  //buf += "<div class=\"col-sm-2\">";
  int limit_glp = String(LIMITE_MQ2).toInt();
  int glp = GLP.toInt();
  //buf += "<h3>&#128293;" + String(GLP) + "/" + String(FUMACA) + "<sup class=\"units\">ppm</sup></h3>";
  // if (glp >= limit_glp )
  // {
  //   buf += "<span class=\"badge badge-pill badge-danger\"> ! </span>";
  // } else if (glp <= (limit_glp / 3))
  // {
  //   buf += "<span class=\"badge badge-pill badge-success\"> . </span>";
  // }
  // else {
  //   buf += "<span class=\"badge badge-pill badge-warning\"> ! </span>";
  // }
  int limit_fu = String(LIMITE_MQ2_FU).toInt();
  int fu = FUMACA.toInt();
  //buf += "</div>";

buf +="<div class=\"table-responsive col\">";
buf +=  "<table class=\"table\">";
buf +=    "<tr>";
buf +=    "<th>Local</th><th>Temperatura</th><th>Umidade</th><th>GLP/Fumaça</th>";
buf +=    "</tr>";
buf +=    "<tr>";
buf +=    "<td><button class=\"btn btn-link\" type=\"button\" data-toggle=\"collapse\" data-target=\"#collapseSensor\" aria-expanded=\"false\" aria-controls=\"collapseSensor\">Cental</button></td>";
buf +=    "<td><font size=\"4\">&#127777 "+String(sensorTemp(2))+"</font><sup class=\"units\">°c</sup></td>";
buf +=    "<td><font size=\"4\">&#128167 " + String(umid) + "</font><sup class=\"units\">%</sup></td>";
buf +=    "<td><font size=\"4\">&#128293 " + String(GLP) + "/" + String(FUMACA) + "</font><sup class=\"units\">pmm</sup></td>";
buf +=    "</tr>";
buf +=    "<tr>";
buf +=    "<td><button class=\"btn btn-link\" type=\"button\" data-toggle=\"collapse\" data-target=\"#collapseSensor_2\" aria-expanded=\"false\" aria-controls=\"collapseSensor_2\">"+ nome_esp +"</button></td>";
buf +=    "<td><font size=\"4\">&#127777 "+String(temp_ext)+"</font><sup class=\"units\">°c</sup></td>";
buf +=    "<td><font size=\"4\">&#128167 " + String(umid_ext) + "</font><sup class=\"units\">%</sup></td>";
buf +=    "<td><font size=\"4\">&#128293 " + String(glp_) + "/" + String(fu_) + "</font><sup class=\"units\">pmm</sup></td>";
buf +=    "</tr>";
buf +=  "</table>";
buf +="</div>";

  buf += "</div>";
  
  buf += "<div>";
  //buf += "<button class=\"btn btn-link\" type=\"button\" data-toggle=\"collapse\" data-target=\"#collapseSensor\" aria-expanded=\"false\" aria-controls=\"collapseSensor\">Historico</button></td>";
  buf += "  <div class=\"row col-sm-12\">";
  buf += " <div class='collapse' id='collapseSensor' style='width:100%'> <div class='card card-body'> ";
  if(sistema_solar == 1)
  {
    buf += FPSTR(TABELA_TEMP_UMID);
    for(int i = 0; i<=9 ; i++){
      buf += temperatura[i];           //LINHAS GERADAS DENTRO DA FUNÇÃO gravaDhtArray()
    }
    buf += FPSTR(TABELA_TEMP_UMID_3);
  }
  buf += " </div> </div>";
  buf += "</div>";

  buf += "  <div class=\"row col-sm-12\">";
  buf += " <div class='collapse' id='collapseSensor_2' style='width:100%'> <div class='card card-body'> ";
  if(sistema_solar == 1)
  {
    buf += FPSTR(TABELA_TEMP_UMID);
    
      buf += tabela_sensores_ext;           //LINHAS GERADAS DENTRO DA FUNÇÃO gravaDhtArray()
    
    buf += FPSTR(TABELA_TEMP_UMID_3);
  }
  buf += " </div> </div>";
  buf += "</div>";
  
  /*
    --------------------------------
    FIM PAINEL SENSORES
    --------------------------------
  */
  
 
  /*
    --------------------------------
                BOTÃO 1
    --------------------------------
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
    --------------------------------
                BOTÃO 2
    --------------------------------
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
     --------------------------------
                  BOTÃO 3
     --------------------------------
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
    --------------------------------
                  BOTÃO 4
    --------------------------------
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
      --------------------------------
      BOTÃO PARA DESLIGAR TODAS AS PORTAS
      --------------------------------
  */

  if (conta_botao > 1)
  {
    buf += "<a href=\"?00015\" title=\"Desligar\"><button type=\"button\"  class=\"btn btn-danger\"> OFF </button></a>";
  }
  buf += "</p>";
  buf += "</div></div>";

  /*
     --------------------------------
     TELA DE CONFIGURAÇÕES
     --------------------------------
  */
  buf += "<div class=\"tab-pane fade\" id=\"pills-profile\" role=\"tabpanel\" aria-labelledby=\"pills-profile-tab\">";
  buf += "<form class=\"form-group\" action=\"?00012\">";
  buf += "<input type=\"hidden\" name=\"cod\" value=\"00012\">";
  buf +=  "<div class=\"container\">";
  buf +=  "<hr>";
  buf +=  " <div class=\"row\">";
  buf +=  "   <div class=\"col-sm-8\">";
  buf +=  "<div>";
  IPAddress mascara = WiFi.subnetMask();
  String masc_String = String(mascara[0]) + "." + String(mascara[1]) + "." +String(mascara[2]) + "." +String(mascara[3]);
  IPAddress router_gatway = WiFi.gatewayIP();
  String ssid = WiFi .SSID();
  int8_t rssi = WiFi.RSSI();
  String router_gatway_String = String(router_gatway[0]) + "." + String(router_gatway[1]) + "." +String(router_gatway[2]) + "." +String(router_gatway[3]);

  String html_samp = "<span dir=\"rtl\"><span style=\"font-size:16px;\"><span style=\"font-family:courier new,courier,monospace;\">";
  buf +=  "    <div>"+html_samp+"<strong>SERVIDOR</strong></span></span></span><input maxlength=\"15\" style=\"width:130px; outline:0px;\" type=\"text\"   name=\"servidor\" value=\"" + serv + "\"></div>";
  buf +=  "    <div>"+html_samp+"<strong>MAC: </strong> "+addressMac+" - "+masc_String+"</span></span></span></div>";
  buf +=  "    <div>"+html_samp+"<strong>Roteador: </strong>"+router_gatway_String+" / <strong>SSID: </strong>"+ssid+" "+String(rssi)+"dBm</span></span></span></div>";
  buf +=  "    <div>"+html_samp+"<strong>RAM: </strong>"+ESP.getFreeHeap()+"Bytes</span></span></span></div>"; 
  buf +=  "    <div>"+html_samp+"<strong>Versão : </strong>"+VERSAO+"</span></span></span></div>"; 
  buf +=       "<strong>Data Hora : </strong><a href='?00018' title='Atualizar data e hora da central'><input style=\"border:0px;width:150px\" value=\"" + relogio_ntp(1) + "\" disabled>&#8634;</a></div>";
  buf +=  "  </div>";
  buf +=  " </div>";
  buf +=  " </div>";
  buf +=  "<hr>";

  buf += gpio_html (1, botao1.entrada, botao1.rele, botao1.nomeInter, botao1.tipo, botao1.modelo);
  buf += gpio_html (2, botao2.entrada, botao2.rele, botao2.nomeInter, botao2.tipo, botao2.modelo);
  buf += gpio_html (3, botao3.entrada, botao3.rele, botao3.nomeInter, botao3.tipo, botao3.modelo);
  buf += gpio_html (4, botao4.entrada, botao4.rele, botao4.nomeInter, botao4.tipo, botao4.modelo);
  buf +=  "<hr>";

  buf +=  " <div class=\"row\">";
  buf +=  "   <div class=\"col-sm-8\">";
  buf += "        <strong>LOG</strong> <select class='' style='width:100%x' name='log'> <option value='sim' " + selectedHTNL(conslog, "sim ") + "> Sim</option> <option value='nao' " + selectedHTNL(conslog, "nao ") + ">Não</option> </select> <select class='' style='width:100%x' name='nivel' title='Nível do log'> <option value='1' " + selectedHTNL(nivelLog, "1") + ">1</option> <option value='2' " + selectedHTNL(nivelLog, "2") + ">2</option> <option value='3' " + selectedHTNL(nivelLog, "3") + ">3</option> <option value='4' " + selectedHTNL(nivelLog, "4") + ">4</option> </select>";
  buf += "        <a href='?00013' title='APAGAR TODOS OS REGISTROS!'> x </a>";
  buf += "        <button class=\"btn btn-primary btn-sm\" type=\"button\" data-toggle=\"collapse\" data-target=\"#collapseExample\" aria-expanded=\"false\" aria-controls=\"collapseExample\">Log</button></td>";
  buf += "    </div>";
  buf += " <div class='collapse' id='collapseExample' style='width: 100%'> <div class='card card-body'> ";
  buf +=    lerLog();
  buf += " </div> </div>";
  buf += "  </div>";
  buf += "  <hr>";
  buf +=  " <div class=\"row\">";
  buf +=  "   <div class=\"col-sm-6\">";
  buf += "        <strong>MQ2 </strong>";
  buf += "        <sup>GAS </sup><input maxlength=\"4\" style=\"width:45px\" type=\"text\" name=\"v_mq\" value=\"" + String(LIMITE_MQ2) + "\">";
  buf += "        <sup>Fuma&ccedil;a </sup><input maxlength=\"4\" style=\"width:45px\" type=\"text\" name=\"v_mq_fu\" value=\"" + String(LIMITE_MQ2_FU) + "\">";
  //buf += "         <a href='?00019' title=''>";
  //buf += "          <button type='button' class='btn btn-danger btn-sm'> Pausar Leitura </button>";
  //buf += "         </a>";
  buf += "    </div>";
  buf += "  </div>";
  buf += "  <hr>";
  buf +=  " <div class=\"row\">";
  buf +=  "   <div class=\"col-sm-4\">";
  buf += "         <input class=\"btn btn-info btn-sm\" type=\"submit\" value=\"Salvar\">";
  buf += "         <a href='?00000' title=''>";
  buf += "         <button type='button' class='btn btn-danger btn-sm'> Reiniciar</button>";
  buf += "         </a>";
  buf += "    </div>";
  buf += "  </div>";
  buf += "</div>";
  buf += "</form> ";
  buf += "</div> </div>";
  //buf += "<a href=\"http://" + ipLocalString + "\"><p style=\"text-align:center\"><p style=\"text-align:right\"> <span class=\"badge badge-pill badge-primary\">" + VERSAO + "</span></span></a></p>";
  buf += "<script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script>";
  buf += "<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"></script>";
  buf += "</body></html>";
  buf += "<script>$(document).ready(function(){$('[data-toggle=\"tooltip\"]').tooltip();});";
  //buf += " function calcNum(num) { document.calcform.visor.type = 'password'; document.calcform.visor.value = document.calcform.visor.value + num; } var delay = 1500;  function calcParse(oper) { var valor = document.calcform.visor.value; if (valor == '') { document.calcform.visor.value = ''; } else { var senha = '" + String(s_senha_alarme) + "'; document.calcform.visor.type = 'text'; if (senha == valor) { document.calcform.visor.value = 'Senha Correta       '; window.location.href = \"?00017\"; valor = ''; } else { document.calcform.visor.value = 'Senha Incorreta      '; valor = ''; setTimeout(function() { calcLimpar(); }, delay); } } }  function calcLimpar() { document.calcform.visor.value = ''; } </script> ";
  buf += "</script> </body>  </html> ";
  return buf;
  buf.remove(0);
}
