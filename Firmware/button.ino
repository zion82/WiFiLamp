// ---------------------------------------
void runEffect(uint8_t eff) {
  FastLED.setBrightness(modes[eff].Brightness);
  loadingFlag = true;
  settChanged = true;
  eepromTimeout = millis();
  //FastLED.clear();
  //delay(1);

  if (random_on && FavoritesManager::FavoritesRunning) {
    selectedSettings = 1U;
  }
#if (USE_MQTT)
  if (espMode == 1U) MqttManager::needToPublish = true;
#endif
#ifdef USE_BLYNK
  updateRemoteBlynkParams();
#endif
}

// ---------------------------------------
void prevEffect() {
  if (Favorit_only) {
    uint8_t lastMode = currentMode;
    do {
      if (++currentMode >= MODE_AMOUNT) currentMode = 0;
    } while (FavoritesManager::FavoriteModes[currentMode] == 0 && currentMode != lastMode);
    if (currentMode == lastMode)                        // если ни один режим не добавлен в избранное, всё равно куда-нибудь переключимся
      if (++currentMode >= MODE_AMOUNT) currentMode = 0;
  }
  else if (++currentMode >= MODE_AMOUNT) currentMode = 0;
  runEffect(currentMode);
}

// ---------------------------------------
void nextEffect() {
  if (Favorit_only) {
    uint8_t lastMode = currentMode;
    do {
      if (--currentMode >= MODE_AMOUNT) currentMode = MODE_AMOUNT - 1;
    } while (FavoritesManager::FavoriteModes[currentMode] == 0 && currentMode != lastMode);
    if (currentMode == lastMode)                      // если ни один режим не добавлен в избранное, всё равно куда-нибудь переключимся
      if (--currentMode >= MODE_AMOUNT) currentMode = MODE_AMOUNT - 1;
  } else {
    if (--currentMode >= MODE_AMOUNT) currentMode = MODE_AMOUNT - 1;
  }
  runEffect(currentMode);
}

// ---------------------------------------
void cycleEffect() {
  FavoritesManager::FavoritesRunning = (FavoritesManager::FavoritesRunning == 1) ? 0 : 1;
}

// ---------------------------------------
void changeBrightness(bool Direction) {
  uint8_t delta = modes[currentMode].Brightness < 10U // определение шага изменения яркости: при яркости [1..10] шаг = 1, при [11..16] шаг = 3, при [17..255] шаг = 15
                  ? 1U
                  : 5U;
  modes[currentMode].Brightness =
    constrain(Direction
              ? modes[currentMode].Brightness + delta
              : modes[currentMode].Brightness - delta,
              1, 255);
  FastLED.setBrightness(modes[currentMode].Brightness);
}

// ---------------------------------------
void runOTA() {
#ifdef OTA
  if (otaManager.RequestOtaUpdate()) {
    ONflag = true;
    currentMode = EFF_MATRIX;                             // принудительное включение режима "Матрица" для индикации перехода в режим обновления по воздуху
    //FastLED.clear();
    //delay(1);
    changePower();
  }
#endif
}

// ---------------------------------------
void showIP() {
  if (espMode == 1U) {
    loadingFlag = true;

#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // установка сигнала в пин, управляющий MOSFET транзистором, матрица должна быть включена на время вывода текста
    digitalWrite(MOSFET_PIN, MOSFET_LEVEL);
#endif
    while (!fillString(WiFi.localIP().toString().c_str(), CRGB::White, false)) {
      delay(1);
      ESP.wdtFeed();
    }

#if defined(MOSFET_PIN) && defined(MOSFET_LEVEL)      // установка сигнала в пин, управляющий MOSFET транзистором, соответственно состоянию вкл/выкл матрицы или будильника
    digitalWrite(MOSFET_PIN, ONflag || (dawnFlag && !manualOff) ? MOSFET_LEVEL : !MOSFET_LEVEL);
#endif
    loadingFlag = true;
  }
}

// =====================================
#ifdef ESP_USE_BUTTON
bool brightDirection;
static bool startButtonHolding = false;                     // флаг: кнопка удерживается для изменения яркости/скорости/масштаба лампы кнопкой
static bool Button_Holding = false;
// --------------------------------------
void buttonTick() {
  if (!buttonEnabled) {                                     // события кнопки не обрабатываются, если она заблокирована
    return;
  }

  touch.tick();
  uint8_t clickCount = touch.hasClicks() ? touch.getClicks() : 0U;

  // однократное нажатие =======
  if (clickCount == 1U) {
    if (dawnFlag) {
      manualOff = true;
      dawnFlag = false;
      FastLED.setBrightness(modes[currentMode].Brightness);
      changePower();
    } else {
      ONflag = !ONflag;
      jsonWrite(configSetup, "Power", ONflag);
      changePower();
    }
    settChanged = true;
    eepromTimeout = millis();
    loadingFlag = true;

#if (USE_MQTT)
    if (espMode == 1U) {
      MqttManager::needToPublish = true;
    }
#endif
#ifdef USE_BLYNK
    updateRemoteBlynkParams();
#endif
  }

  // двухкратное нажатие =======
  if (ONflag && clickCount == 2U) {
    prevEffect();
  }

  // трёхкратное нажатие =======
  if (ONflag && clickCount == 3U) {
    nextEffect();
  }

  // четырёхкратное нажатие =======
  if (clickCount == 4U) {                                   // нa выбор
    // runOTA();                                            // редко используемый режим проще и удобней включить из приложения заменен на любимый эффект
    currentMode = EFF_FAV; runEffect(currentMode);          // или любимый эффект  
  }

  //  пятикратное нажатие =======
  //  • включить эффект огонь
  if (clickCount == 5U) {                                   // на выбор
    currentMode = EFF_FIRE; runEffect(currentMode);         // включить эффект огонь
    // showIP();                                          // вывод IP на лампу
  }

  // шестикратное нажатие =======
  if (clickCount == 6U) {                                    // нa выбор
    // printTime(thisTime, true, ONflag);                       // вывод текущего времени бегущей строкой
    cycleEffect();                                           // или включение показа эффектов в цикле
  }

  // семикратное нажатие =======
  if (clickCount == 7U) {  // if (ONflag &&                  // смена рабочего режима лампы: с WiFi точки доступа на WiFi клиент или наоборот

#ifdef RESET_WIFI_ON_ESP_MODE_CHANGE
    if (espMode) wifiManager.resetSettings();                // сброс сохранённых SSID и пароля (сброс настроек подключения к роутеру)
#endif
    espMode = (espMode == 0U) ? 1U : 0U;
    jsonWrite(configSetup, "ESP_mode", (int)espMode);
    saveConfig();

#ifdef GENERAL_DEBUG
    LOG.printf_P(PSTR("Рабочий режим лампы изменён и сохранён в энергонезависимую память\n\rНовый рабочий режим: ESP_MODE = %d, %s\n\rРестарт...\n\r"),
                 espMode, espMode == 0U ? F("WiFi точка доступа") : F("WiFi клиент (подключение к роутеру)"));
    delay(1000);
#endif

    showWarning(CRGB::Red, 3000U, 500U);                    // мигание красным цветом 3 секунды - смена рабочего режима лампы, перезагрузка
    ESP.restart();
  }
#ifdef GENERAL_DEBUG
  if (clickCount > 0U) {
    LOG.printf_P(PSTR("Button Click Count: %d\n\r"), clickCount);
  }
#endif

  // кнопка только начала удерживаться
  // if (ONflag && touch.isHolded())
  if (touch.isHolded()) {                                  // пускай для выключенной лампы удержание кнопки включает белую лампу
    brightDirection = !brightDirection;
    startButtonHolding = true;
  }

  // кнопка нажата и удерживается
  //  if (ONflag && touch.isStep())
  if (touch.isStep()) {
    if (ONflag && !Button_Holding) {
      int8_t but = touch.getHoldClicks();
      Serial.println (but);

      switch (but ) {
        case 0U: {                                               // просто удержание (до удержания кнопки кликов не было) - изменение яркости
            changeBrightness(brightDirection);
#ifdef GENERAL_DEBUG
            LOG.printf_P(PSTR("Новое значение яркости: %d\n\r"), modes[currentMode].Brightness);
#endif
            break;
          }

        case 1U: {                                               // удержание после одного клика - изменение скорости
            modes[currentMode].Speed = constrain(brightDirection ? modes[currentMode].Speed + 1 : modes[currentMode].Speed - 1, 1, 255);
            loadingFlag = true; // без перезапуска эффекта ничего и не увидишь

#ifdef GENERAL_DEBUG
            LOG.printf_P(PSTR("Новое значение скорости: %d\n\r"), modes[currentMode].Speed);
#endif

            break;
          }

        case 2U: {                                             // удержание после двух кликов - изменение масштаба
            modes[currentMode].Scale = constrain(brightDirection ? modes[currentMode].Scale + 1 : modes[currentMode].Scale - 1, 1, 100);
            loadingFlag = true; // без перезапуска эффекта ничего и не увидишь

#ifdef GENERAL_DEBUG
            LOG.printf_P(PSTR("Новое значение масштаба: %d\n\r"), modes[currentMode].Scale);
#endif
            break;
          }

        default:
          break;
      }

      settChanged = true;
      eepromTimeout = millis();
    } else {
      if (!Button_Holding) {
        Button_Holding = true;
        currentMode = EFF_WHITE_COLOR;
        ONflag = true;
        jsonWrite(configSetup, "Power", ONflag);
        changePower();
        settChanged = true;
        eepromTimeout = millis();
#ifdef USE_BLYNK
        updateRemoteBlynkParams();
#endif
      }
    }
  }
  // кнопка отпущена после удерживания
  if (ONflag && !touch.isHold() && startButtonHolding) {     // кнопка отпущена после удерживания, нужно отправить MQTT сообщение об изменении яркости лампы
    startButtonHolding = false;
    Button_Holding = false;
    loadingFlag = true;

#if (USE_MQTT)
    if (espMode == 1U) MqttManager::needToPublish = true;
#endif

#ifdef USE_BLYNK
    updateRemoteBlynkParams();
#endif

  }
}
#endif
