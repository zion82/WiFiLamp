
void HTTP_init(void) {
  // Выдаем данные configSetup ===========
  HTTP.on("/config.setup.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", configSetup);
  });

  // Обработка Restart ===================
  HTTP.on("/restart", HTTP_GET, []() {
    String restart = HTTP.arg("device");            // Получаем значение device из запроса
    if (restart == "ok") {                          // Если значение равно Ок
      HTTP.send(200, "text / plain", "Reset OK");   // Oтправляем ответ Reset OK
      ESP.restart();                                // перезагружаем модуль
    }
    else {                                          // иначе
      HTTP.send(200, "text / plain", "No Reset");   // Oтправляем ответ No Reset
    }
  });

  // Remote Control ---------------------
  HTTP.on("/list_eff", handle_list_eff);            // получение списка еффектов http:ipLamp/list_eff
  HTTP.on("/cmd", handle_cmd);                      // web управление лампой http:ipLamp/cmd?val=3
  // ------------------------------------
  httpUpdater.setup(&HTTP);                         // Добавляем функцию Update для перезаписи прошивки по WiFi при 4М(1M SPIFFS) и выше
  HTTP.begin();                                     // Запускаем HTTP сервер
}

// ======================================
void handle_list_eff() {
  String body = "";
  body += "\"id\":" + String(CMD_LIST) + ",";
  body += "\"json\":{\"list_eff\":\"" + efList_1 + efList_2 + efList_3 + "\"}";
  sendReq(body);
}

// ======================================
void handle_cmd() {
  uint8_t cmd = HTTP.arg("cmd").toInt();
  uint8_t val = HTTP.arg("val") ? HTTP.arg("val").toInt() : 0;
  String body = "";

  // modes[currentMode].Brightness
  switch (cmd ) {
    case CMD_POWER: {
        ONflag = !ONflag;
        changePower();
        break;
      }
    case CMD_PREV_EFF: {
        prevEffect();
        break;
      }
    case CMD_NEXT_EFF: {
        nextEffect();
        break;
      }
    case CMD_BRIGHT_UP: {
        changeBrightness(true);
        break;
      }
    case CMD_BRIGHT_DW: {
        changeBrightness(false);
        break;
      }
    case CMD_SPEED: {
        modes[currentMode].Speed = val;
        loadingFlag = true;
        break;
      }

    case CMD_SCALE: {
        //        modes[currentMode].Scale =
        showWarning(CRGB::Green, 1000U, 500U);
        break;
      }
    case CMD_WHITE: {
        currentMode = EFF_WHITE_COLOR;
        runEffect(currentMode);
        break;
      }
    case CMD_TEXT: {
        currentMode = EFF_TEXT;
        runEffect(currentMode);
        break;
      }
    case CMD_INTIM: {
        currentMode = EFF_FIRE;
        runEffect(currentMode);
        break;
      }
    // develop commands -----
    case CMD_INFO: {
        showWarning(CRGB::Blue, 2000U, 500U);
        // fillAll(CHSV(200U, 255U, 230U));
        body += "\"id\":" + String(cmd) + ",";
        body += getInfo();
        sendReq(body);
        break;
      }
    case CMD_OTA: {
        runOTA();
        break;
      }
    default:
      break;
  }
  body += "\"id\":" + String(cmd) + ",";
  if  (cmd == CMD_OTA) {
    body += getInfo() + ",";
  }
  body += getCurState();
  sendReq(body);
}

// ======================================
void sendReq(String body) {
  String output = "<!DOCTYPE html>\r\n";
  output += "<html lang='en'>\r\n";
  output += "<head>\r\n<meta charset='utf-8'>\r\n<title>ESP-8266</title>\r\n";
  output += "<script>window.onload=function(){window.parent.postMessage(document.body.innerHTML, '*')};</script>\r\n";
  output += "</head>\r\n";
  output += "<body>{" + body + "}</body>";
  output += "</html>\r\n";
  HTTP.send(200, "text/html", output);
}

// ======================================
String getCurState() {
  IPAddress ip = WiFi.localIP();
  String lamp_state = "\"json\":{";
  lamp_state += "\"name\":\"" + AP_NAME + "\",";
  lamp_state += "\"ip\":\"" + ipToString(ip) + "\",";
  lamp_state += "\"pass\":\"" + AP_PASS + "\",";
  lamp_state += "\"ver\":\"" + VERSION + "\",";
  lamp_state += "\"power\":" + String(ONflag) + ",";
  lamp_state += "\"list_idx\":" + String(currentMode) + ",";
  lamp_state += "\"bright\":" + String(modes[currentMode].Brightness) + ",";
  lamp_state += "\"speed\":" + String(modes[currentMode].Speed) + ",";
  lamp_state += "\"scale\":" + String(modes[currentMode].Scale);
  lamp_state += "}";
  return lamp_state;
}

// ======================================
String ipToString(IPAddress ip) {
  String s = "";
  for (int i = 0; i < 4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

// ======================================
String getInfo() {
  // IPAddress ip = WiFi.localIP();
  //  WiFi.softAPIP();
  String lamp_state = "\"json\":{";

  lamp_state += "}";
  return lamp_state;
}
// ======================================
String sendCurState() {
  String body = "\"id\":" + String(CMD_STATE) + ",";
  // body += getInfo() + ",";
  body += getCurState();
  sendReq(body);
}
