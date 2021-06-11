void User_setings () {


  HTTP.on("/favorit", handle_favorit);    // включить \ выключить переход кнопкой только по эффектам из выбранных в режиме Цикл и
  HTTP.on("/random_on", handle_random);  // случайных настроек эффектов в режиме цикл без сохранения в EEPROM
  HTTP.on("/print_time", handle_print_time); //Периодичность вывода времени бегущей строкой
  HTTP.on("/button_on", handle_button_on);  // Вкл\Выкл кнопки лампы (дублирует в приложении, но на виду)
  //HTTP.on("/User_set", handle_User_set); // Сохранение random_on, time_always, button_on и favorit в EEPROM (Файл)
  HTTP.on("/ESP_mode", handle_ESP_mode); // Установка ESP Mode
  HTTP.on("/eff_reset", handle_eff_reset);
  HTTP.on("/summer_time", handle_summer_time);  //Переход на лнтнее время 1 - да , 0 - нет
  HTTP.on("/time_always", handle_time_always);     // Выводить или нет время бегущей строкой(если задано) на не активной лампе
  HTTP.on("/timeZone", handle_time_zone);    // Установка времянной зоны по запросу вида http://192.168.0.101/timeZone?timeZone=3

  // --------------------Получаем SSID со страницы
  HTTP.on("/ssid", HTTP_GET, []() {
    jsonWrite(configSetup, "ssid", HTTP.arg("ssid"));
    jsonWrite(configSetup, "password", HTTP.arg("password"));
    jsonWrite(configSetup, "TimeOut", HTTP.arg("TimeOut").toInt());
    ESP_CONN_TIMEOUT = jsonReadtoInt(configSetup, "TimeOut");
    saveConfig();                 // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  });
  // --------------------Получаем SSID AP со страницы
  HTTP.on("/ssidap", HTTP_GET, []() {
    jsonWrite(configSetup, "ssidAP", HTTP.arg("ssidAP"));
    jsonWrite(configSetup, "passwordAP", HTTP.arg("passwordAP"));
    saveConfig();                 // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  });

}

void handle_favorit() {
  jsonWrite(configSetup, "favorit", HTTP.arg("favorit").toInt());
  saveConfig();
  Favorit_only = jsonReadtoInt(configSetup, "favorit");
  HTTP.send(200, "text/plain", "OK");
}

void handle_random() {
  jsonWrite(configSetup, "random_on", HTTP.arg("random_on").toInt());
  saveConfig();
  random_on = jsonReadtoInt(configSetup, "random_on");
  HTTP.send(200, "text/plain", "OK");
}

void handle_print_time() {
  jsonWrite(configSetup, "print_time", HTTP.arg("print_time").toInt());
  PRINT_TIME = jsonReadtoInt(configSetup, "print_time");
  saveConfig();
  HTTP.send(200, "text/plain", "OK");
}

void handle_button_on() {
  jsonWrite(configSetup, "button_on", HTTP.arg("button_on").toInt());
  saveConfig();
  buttonEnabled = jsonReadtoInt(configSetup, "button_on");
  HTTP.send(200, "text/plain", "OK");
}

/*
  void handle_User_set () {
    handle_random();
	handle_favorit();
 	handle_time_always();
	handle_button_on();
    saveConfig();
    HTTP.send(200, "text/plain", "OK");
  }
*/

void handle_ESP_mode() {
  jsonWrite(configSetup, "ESP_mode", HTTP.arg("ESP_mode").toInt());
  saveConfig();
  espMode = jsonReadtoInt(configSetup, "ESP_mode");
  HTTP.send(200, "text/plain", "OK");
}

void handle_eff_reset() {
  restoreSettings();
  updateSets();
  showWarning(CRGB::Blue, 2000U, 500U);                    // мигание синим цветом 2 секунды
#ifdef USE_BLYNK
  updateRemoteBlynkParams();
#endif
  HTTP.send(200, "text/plain", "OK");
}

void handle_summer_time() {
  jsonWrite(configSetup, "Summer_Time", HTTP.arg("Summer_Time").toInt());
  saveConfig();
  summerTime.offset = winterTime.offset + jsonReadtoInt(configSetup, "Summer_Time") * 60;
  localTimeZone.setRules (summerTime, winterTime);
  HTTP.send(200, "text/plain", "OK");
}

void handle_time_always() {
  jsonWrite(configSetup, "time_always", HTTP.arg("time_always").toInt());
  saveConfig();
  time_always = jsonReadtoInt(configSetup, "time_always");
  HTTP.send(200, "text/plain", "OK");
}

// Установка параметров времянной зоны по запросу вида, например, http://192.168.0.101/timeZone?timeZone=3
void handle_time_zone() {
  jsonWrite(configSetup, "timezone", HTTP.arg("timeZone").toInt()); // Получаем значение timezone из запроса конвертируем в int сохраняем
  saveConfig();
  winterTime.offset = jsonReadtoInt(configSetup, "timezone") * 60;
  summerTime.offset = winterTime.offset + jsonReadtoInt(configSetup, "Summer_Time") * 60;
  localTimeZone.setRules (summerTime, winterTime);
  HTTP.send(200, "text/plain", "OK");
}
