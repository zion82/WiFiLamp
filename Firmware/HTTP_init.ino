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
  HTTP.on("/cmd", handle_cmd);                      // web управление лампой http:ipLamp/cmd?val=3
  // ------------------------------------
  httpUpdater.setup(&HTTP);                         // Добавляем функцию Update для перезаписи прошивки по WiFi при 4М(1M SPIFFS) и выше
  HTTP.begin();                                     // Запускаем HTTP сервер
}

// ======================================
// Remote Control Command ---------------
void handle_cmd() {
  uint8_t cmd = HTTP.arg("cmd").toInt();
  String valStr = HTTP.arg("valStr");
  uint8_t val = HTTP.arg("val") ? HTTP.arg("val").toInt() : 0;
  String body = "";
  switch (cmd ) {
    case CMD_POWER:
      if (val == 3) {               // toggle
        ONflag = !ONflag;
      } else {                      // on
        ONflag = true;
      }
      changePower();
      break;

    case CMD_PREV_EFF:
      selectedSettings = 0U;
      prevEffect();
      break;

    case CMD_NEXT_EFF:
      selectedSettings = 0U;
      nextEffect();
      break;

    case CMD_FAVORITES:
      FavoritesManager::FavoritesRunning = val;
      break;

    case CMD_BRIGHT_UP:
      changeBrightness(true);
      break;

    case CMD_BRIGHT_DW:
      changeBrightness(false);
      break;

    case CMD_SPEED:
      modes[currentMode].Speed = val;
      loadingFlag = true;
      break;

    case CMD_SCALE:
      modes[currentMode].Scale = val;
      loadingFlag = true;
      break;

    case CMD_WHITE:
      currentMode = EFF_WHITE_COLOR;
      runEffect(currentMode);
      break;

    case CMD_AUTO:
      break;

    case CMD_TEXT: {
        String temStr = RUNNING_TEXT_DEFAULT;
        if (valStr != "") {
          temStr = valStr;
        }
        temStr.toCharArray(TextTicker, temStr.length() + 1);
        currentMode = EFF_TEXT;
        runEffect(currentMode);
      }
      break;

    case CMD_INTIM:
      currentMode = EFF_FIRE;
      runEffect(currentMode);
      break;
    case CMD_FAV:
      currentMode = EFF_FAV;
      runEffect(currentMode);
      break;
    case CMD_SCAN:
      //      body += "\"id\":" + String(cmd) + ",";
      //      body += getInfo();
      // showWarning(CRGB::Blue, 1000U, 500U);
      break;

    // effect ----------------
    case CMD_DEFAULT:
      modes[currentMode].Brightness = pgm_read_byte(&defaultSettings[currentMode][0]);
      modes[currentMode].Speed      = pgm_read_byte(&defaultSettings[currentMode][1]);
      modes[currentMode].Scale      = pgm_read_byte(&defaultSettings[currentMode][2]);
      loadingFlag = true;
      //      runEffect(currentMode);
      break;
    case CMD_RANDOM:
      selectedSettings = 1U;
      updateSets();
      break;
    case CMD_LIST:
      loadingFlag = false;
      // path -----------------
      if (SPIFFS.exists("/" + valStr)) {
        body += "\"status\":\"OK\",";
        body += "\"list\":" + readFile(valStr, 4096) + ",";
      } else {
        body += "\"status\":\"Error File Not Found\",";
      }
      break;

    case CMD_SHOW_EFF:
      currentMode = val;
      runEffect(currentMode);
      break;

    // configure commands ---
    case CMD_CONFIG:
      // LOG.println("config Setup:" + configSetup);
      body += "\"cfg\":" + configSetup + ",";
      break;
    case CMD_SAVE_CFG :
      configSetup = valStr;
      LOG.println("config save:" + configSetup);
      body += "\"cfg_save\":\"OK\",";
      writeFile("config.json", configSetup);
      valStr = "";
      break;

    // fs commands ----------
    case CMD_FS_DIR:
      loadingFlag = false;
      body += getLampID() + ",";
      body += getDirFS();
      sendResponse(cmd, body);
      return;

    // develop commands -----    
    case CMD_IP:
      showIP();
      return;
    case CMD_INFO:
      body += getLampID() + ",";
      body += getInfo();
      sendResponse(cmd, body);
      return;
    case CMD_ECHO:
      switch (val) {
        case 6: showWarning(CRGB::Blue, 1000U, 250U); break;
        case 7: showWarning(CRGB::Magenta, 1000U, 250U); break;
        default: showWarning(CRGB::Red, 1000U, 250U); break;
      }
      break;
    case CMD_DEL_FILE:
      // if (valStr == "") body += "\"status\":\"Error BAD ARGS\",";
      if (valStr == "/") body += "\"status\":\"Error BAD PATH\",";
      if (SPIFFS.exists(valStr)) {
        SPIFFS.remove(valStr);
        cmd = CMD_FS_DIR;
        body += "\"status\":\"OK\",";
        // send new list files ----
        body += getDirFS() + ",";
      } else {
        body += "\"status\":\"Error File Not Found\",";
      }
      break;

    case CMD_RESET:
      showWarning(CRGB::MediumSeaGreen, 2000U, 500U);
      ESP.restart();
      return;

    case CMD_OTA: runOTA(); break;

    default:
      break;
  }

  body += getCurState();
  sendResponse(cmd, body);
}

// ======================================
String getInfo() {
  byte mac[6];
  WiFi.macAddress(mac);
  IPAddress ip = WiFi.localIP();
  String ssid = jsonRead(configSetup, "ssid");
  String lamp_info = "";
  lamp_info += "\"ssid\":\"" + ssid + "\",";
  lamp_info += "\"mac\":\"" + (String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[2], HEX) + ":" + String(mac[3], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[5], HEX)) + "\",";
  lamp_info += "\"free_heap\":" + String(system_get_free_heap_size()) + ",";
  lamp_info += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  //  WIDTH | HEIGHT | MATRIX_TYPE | CONNECTION_ANGLE | STRIP_DIRECTION | COLOR_ORDER
  lamp_info += "\"matrix\":[" + String(WIDTH) + "," + String( HEIGHT) + "," + String(MATRIX_TYPE) + "," + String(CONNECTION_ANGLE) + "," + String(STRIP_DIRECTION) + "],";
  lamp_info += "\"ver\":\"" + VERSION + "\"";
  return lamp_info;
}

// ======================================
String getCurState() {
  String lamp_state = "";
  lamp_state += getLampID() + ",";
  lamp_state += "\"pass\":\"" + AP_PASS + "\",";
  lamp_state += "\"ver\":\"" + VERSION + "\",";
  lamp_state += "\"cycle\":" + String(FavoritesManager::FavoritesRunning) + ",";
  lamp_state += "\"list_idx\":" + String(currentMode) + ",";
  lamp_state += "\"max_eff\":" + String(MODE_AMOUNT) + ",";
  lamp_state += "\"bright\":" + String(modes[currentMode].Brightness) + ",";
  lamp_state += "\"speed\":" + String(modes[currentMode].Speed) + ",";
  lamp_state += "\"scale\":" + String(modes[currentMode].Scale);
  return lamp_state;
}

// ======================================
String getLampID() {
  IPAddress ip = WiFi.localIP();
  String id = "\"name\":\"" + LAMP_NAME + "\",";
  id += "\"ip\":\"" + ipToString(ip) + "\"";
  return id;
}

// ======================================
String ipToString(IPAddress ip) {
  String s = "";
  for (int i = 0; i < 4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

// ======================================
String getDirFS() {
  //  if (!HTTP.hasArg("dir")) {
  //    HTTP.send(500, "text/plain", "BAD ARGS");
  //    return;
  //  }
  String path = ""; //HTTP.arg("dir");
  Dir dir = SPIFFS.openDir(path);
  path = String();
  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  output += "]";
  return "\"fs\":" + output;
}

// ======================================
void sendResponse(uint8_t cmd, String json) {
  String body = "\"id\":" + String(cmd) + ",";
  body += "\"json\":{";
  body += json;
  body += "}";
  sendHTML(body);
}

// ======================================
void sendHTML(String body) {
  String output = "<!DOCTYPE html>\r\n";
  output += "<html lang='en'>\r\n";
  output += "<head>\r\n<meta charset='utf-8'>\r\n<title>ESP-8266</title>\r\n";
  output += "<script>window.onload=function(){window.parent.postMessage(document.body.innerHTML, '*')};</script>\r\n";
  output += "</head>\r\n";
  output += "<body>{" + body + "}</body>";
  output += "</html>\r\n";
  HTTP.send(200, "text/html", output);
}
