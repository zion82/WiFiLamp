//
// ======================= ВНИМАНИЕ !!! =============================
//  Все настройки делаются на вкладке Constants.h
//  Почитайте там то, что на русском языке написано.
//  Либо ничего не трогайте, если собирали, по схемам из этого архива.
//  В любом случае ВНИМАТЕЛЬНО прочитайте файл ПРОЧТИ МЕНЯ!!!.txt из этого архива.
//
//  решение проблем можно поискать тут под спойлерами:
//  https://community.alexgyver.ru/goto/post?id=73929
// ==================================================================

// Ссылка для менеджера плат:
// https://arduino.esp8266.com/stable/package_esp8266com_index.json
// При установке выбираем версию 2.7.4

/*
  Скетч к проекту "Многофункциональный RGB светильник"
  Страница проекта (схемы, описания): https://alexgyver.ru/GyverLamp/
  Исходники авторской (старой) прошивки на GitHub: https://github.com/AlexGyver/GyverLamp/
  ==========================================================================================
  Исходники новой версии: https://community.alexgyver.ru/goto/post?id=73929
  ==========================================================================================
  Автор идеи и первой реализации: AlexGyver, AlexGyver Technologies, 2019
  https://AlexGyver.ru/
*/


// Ссылка для менеджера плат:
// https://arduino.esp8266.com/stable/package_esp8266com_index.json

#define FASTLED_USE_PROGMEM 1 // просим библиотеку FASTLED экономить память контроллера на свои палитры

#include "pgmspace.h"
#include "ConstantsUser.h"
#include "Constants.h"
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include "Types.h"
#include "timerMinim.h"
#ifdef ESP_USE_BUTTON
#include <GyverButton.h>
#endif
#include "fonts.h"
#ifdef USE_NTP
#include <NTPClient.h>
#include <Timezone.h>
#endif
#include <TimeLib.h>
#ifdef OTA
#include "OtaManager.h"
#endif
#if USE_MQTT
#include "MqttManager.h"
#endif
#include "TimerManager.h"
#include "FavoritesManager.h"
#include "EepromManager.h"
#ifdef USE_BLYNK
#include <BlynkSimpleEsp8266.h>
#endif
#include <ESP8266SSDP.h>
#include <ArduinoJson.h>        //Установить из менеджера библиотек версию 5.13.5 !!!. https://arduinojson.org/
#include <ESP8266HTTPUpdateServer.h>    // Обновление с web страницы
#ifdef USE_LittleFS
#include <LittleFS.h>
#define SPIFFS LittleFS
#endif
// --- ИНИЦИАЛИЗАЦИЯ ОБЪЕКТОВ ----------
CRGB leds[NUM_LEDS];
//WiFiManager wifiManager;
//WiFiServer wifiServer(ESP_HTTP_PORT);
WiFiUDP Udp;

#ifdef USE_NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, 0, NTP_INTERVAL); // объект, запрашивающий время с ntp сервера; в нём смещение часового пояса не используется (перенесено в объект localTimeZone); здесь всегда должно быть время UTC
TimeChangeRule summerTime  = { SUMMER_TIMEZONE_NAME, SUMMER_WEEK_NUM, SUMMER_WEEKDAY, SUMMER_MONTH, SUMMER_HOUR, 0 };
TimeChangeRule winterTime  = { WINTER_TIMEZONE_NAME, WINTER_WEEK_NUM, WINTER_WEEKDAY, WINTER_MONTH, WINTER_HOUR, 0 };
Timezone localTimeZone(summerTime, winterTime);
#ifdef PHONE_N_MANUAL_TIME_PRIORITY
bool stillUseNTP = true;
#endif
#endif

timerMinim timeTimer(3000);
bool ntpServerAddressResolved = false;
bool timeSynched = false;
uint32_t lastTimePrinted = 0U;

#if defined(USE_MANUAL_TIME_SETTING) || defined(GET_TIME_FROM_PHONE)
time_t manualTimeShift;
#endif

#ifdef GET_TIME_FROM_PHONE
time_t phoneTimeLastSync;
#endif

uint8_t selectedSettings = 0U;

#ifdef ESP_USE_BUTTON
#if (BUTTON_IS_SENSORY == 1)
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);  // для сенсорной кнопки LOW_PULL
#else
GButton touch(BTN_PIN, HIGH_PULL, NORM_OPEN); // для физической (не сенсорной) кнопки HIGH_PULL. ну и кнопку нужно ставить без резистора в разрыв между пинами D2 и GND
#endif
#endif

#ifdef OTA
OtaManager otaManager(&showWarning);
OtaPhase OtaManager::OtaFlag = OtaPhase::None;
#endif

#if USE_MQTT
AsyncMqttClient* mqttClient = NULL;
AsyncMqttClient* MqttManager::mqttClient = NULL;
char* MqttManager::mqttServer = NULL;
char* MqttManager::mqttUser = NULL;
char* MqttManager::mqttPassword = NULL;
char* MqttManager::clientId = NULL;
char* MqttManager::lampInputBuffer = NULL;
char* MqttManager::topicInput = NULL;
char* MqttManager::topicOutput = NULL;
bool MqttManager::needToPublish = false;
char MqttManager::mqttBuffer[] = {};
uint32_t MqttManager::mqttLastConnectingAttempt = 0;
SendCurrentDelegate MqttManager::sendCurrentDelegate = NULL;
#endif

ESP8266HTTPUpdateServer httpUpdater;  // Объект для обнавления с web страницы
ESP8266WebServer HTTP (ESP_HTTP_PORT);//ESP8266WebServer HTTP;  // Web интерфейс для устройства
File fsUploadFile;  // Для файловой системы



// --- ИНИЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ -------
uint16_t localPort = ESP_UDP_PORT;
char packetBuffer[MAX_UDP_BUFFER_SIZE];                     // buffer to hold incoming packet
char inputBuffer[MAX_UDP_BUFFER_SIZE];
static const uint8_t maxDim = max(WIDTH, HEIGHT);

ModeType modes[MODE_AMOUNT];
AlarmType alarms[7];

static const uint8_t dawnOffsets[] PROGMEM = {5, 10, 15, 20, 25, 30, 40, 50, 60};   // опции для выпадающего списка параметра "время перед 'рассветом'" (будильник); синхронизировано с android приложением
uint8_t dawnMode;
bool dawnFlag = false;
uint32_t thisTime;
bool manualOff = false;

uint8_t currentMode = 0;
bool loadingFlag = true;
bool ONflag = false;
uint32_t eepromTimeout;
bool settChanged = false;
bool buttonEnabled = true; // это важное первоначальное значение. нельзя делать false по умолчанию

unsigned char matrixValue[8][16]; //это массив для эффекта Огонь

bool TimerManager::TimerRunning = false;
bool TimerManager::TimerHasFired = false;
uint8_t TimerManager::TimerOption = 1U;
uint64_t TimerManager::TimeToFire = 0ULL;

uint8_t FavoritesManager::FavoritesRunning = 0;
uint16_t FavoritesManager::Interval = DEFAULT_FAVORITES_INTERVAL;
uint16_t FavoritesManager::Dispersion = DEFAULT_FAVORITES_DISPERSION;
uint8_t FavoritesManager::UseSavedFavoritesRunning = 0;
uint8_t FavoritesManager::FavoriteModes[MODE_AMOUNT] = {0};
uint32_t FavoritesManager::nextModeAt = 0UL;

//bool CaptivePortalManager::captivePortalCalled = false;

char TextTicker [43];
int Painting = 0; CRGB DriwingColor = CRGB(255, 255, 255);

uint8_t espMode ;
uint8_t random_on;
uint8_t Favorit_only;
//uint32_t my_time;
uint32_t my_timer;
uint8_t time_always;
bool connect = false;
uint32_t lastResolveTryMoment = 0xFFFFFFFFUL;
uint8_t ESP_CONN_TIMEOUT;
uint8_t PRINT_TIME ;




void setup()
{
  Serial.begin(115200);
  Serial.println();
  ESP.wdtEnable(WDTO_8S);


  // ПИНЫ
#ifdef MOSFET_PIN                                         // инициализация пина, управляющего MOSFET транзистором в состояние "выключен"
  pinMode(MOSFET_PIN, OUTPUT);
#ifdef MOSFET_LEVEL
  digitalWrite(MOSFET_PIN, !MOSFET_LEVEL);
#endif
#endif

#ifdef ALARM_PIN                                          // инициализация пина, управляющего будильником в состояние "выключен"
  pinMode(ALARM_PIN, OUTPUT);
#ifdef ALARM_LEVEL
  digitalWrite(ALARM_PIN, !ALARM_LEVEL);
#endif
#endif

  //HTTP
  User_setings ();
  LOG.print(F("\nСтарт файловой системы\n"));
  FS_init();  //Запускаем файловую систему
  LOG.print(F("Чтение файла конфигурации\n"));
  configSetup = readFile("config.json", 768);
  LOG.println(configSetup);
  //Настраиваем и запускаем SSDP интерфейс
  LOG.print(F("Старт SSDP\n"));
  SSDP_init();
  //Настраиваем и запускаем HTTP интерфейс
  LOG.print (F("Старт WebServer\n"));
  HTTP_init();


  //-----------Инициализируем переменные, хранящиеся в файле config.json--------------
  LAMP_NAME = jsonRead(configSetup, "SSDP");
  AP_NAME = jsonRead(configSetup, "ssidAP");
  AP_PASS = jsonRead(configSetup, "passwordAP");
  Favorit_only = jsonReadtoInt(configSetup, "favorit");
  random_on = jsonReadtoInt(configSetup, "random_on");
  espMode = jsonReadtoInt(configSetup, "ESP_mode");
  PRINT_TIME = jsonReadtoInt(configSetup, "print_time");
  buttonEnabled = jsonReadtoInt(configSetup, "button_on");
  ESP_CONN_TIMEOUT = jsonReadtoInt(configSetup, "TimeOut");
  time_always = jsonReadtoInt(configSetup, "time_always");
  (jsonRead(configSetup, "run_text")).toCharArray (TextTicker, (jsonRead(configSetup, "run_text")).length() + 1);
  NIGHT_HOURS_START = 60U * jsonReadtoInt(configSetup, "night_time");
  NIGHT_HOURS_BRIGHTNESS = jsonReadtoInt(configSetup, "night_bright");
  NIGHT_HOURS_STOP = 60U * jsonReadtoInt(configSetup, "day_time");
  DAY_HOURS_BRIGHTNESS = jsonReadtoInt(configSetup, "day_bright");
  DONT_TURN_ON_AFTER_SHUTDOWN = jsonReadtoInt(configSetup, "effect_always");
  AUTOMATIC_OFF_TIME = (5 * 60UL * 60UL * 1000UL) * ( uint32_t )(jsonReadtoInt(configSetup, "timer5h"));
#ifdef USE_NTP
  (jsonRead(configSetup, "ntp")).toCharArray (NTP_ADDRESS, (jsonRead(configSetup, "ntp")).length() + 1);
#endif
  jsonWrite(configSetup, "Power", ONflag);
  //aveConfig();
  Serial.print ("TextTicker = ");
  Serial.println (TextTicker);
#ifdef USE_NTP
  winterTime.offset = jsonReadtoInt(configSetup, "timezone") * 60;
  summerTime.offset = winterTime.offset + jsonReadtoInt(configSetup, "Summer_Time") * 60;
  localTimeZone.setRules (summerTime, winterTime);
#endif


  // TELNET
#if defined(GENERAL_DEBUG) && GENERAL_DEBUG_TELNET
  telnetServer.begin();
  for (uint8_t i = 0; i < 100; i++)                         // пауза 10 секунд в отладочном режиме, чтобы успеть подключиться по протоколу telnet до вывода первых сообщений
  {
    handleTelnetClient();
    delay(100);
    ESP.wdtFeed();
  }
#endif


  // КНОПКА
#if defined(ESP_USE_BUTTON)
  touch.setStepTimeout(BUTTON_STEP_TIMEOUT);
  touch.setClickTimeout(BUTTON_CLICK_TIMEOUT);
  touch.setDebounce(BUTTON_SET_DEBOUNCE);
#if ESP_RESET_ON_START
  delay(500);                                            // ожидание инициализации модуля кнопки ttp223 (по спецификации 250мс)
  if (digitalRead(BTN_PIN))
  {
    // wifiManager.resetSettings();
    LOG.println(F("Настройки WiFiManager сброшены"));
    //buttonEnabled = true;                                   // при сбросе параметров WiFi сразу после старта с зажатой кнопкой, также разблокируется кнопка, если была заблокирована раньше
    jsonWrite(configSetup, "ssid", "");                          // сброс сохранённых SSID и пароля при старте с зажатой кнопкой, если разрешено
    jsonWrite(configSetup, "password", "");
    saveConfig();                                       // Функция сохранения данных во Flash
  }
  ESP.wdtFeed();
#elif defined(BUTTON_LOCK_ON_START)
  delay(500);                                            // ожидание инициализации модуля кнопки ttp223 (по спецификации 250мс)
  if (digitalRead(BTN_PIN))
    buttonEnabled = false;
  ESP.wdtFeed();
#endif
#endif


  // ЛЕНТА/МАТРИЦА
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)/*.setCorrection(TypicalLEDStrip)*/;
  //FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(0xFFB0F0); // по предложению @kostyamat добавлена такая цветокоррекция "теперь можно получить практически чистый желтый цвет" и получилось плохо
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0)
  {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  }
  FastLED.clear();
  FastLED.show();

#ifdef USE_SHUFFLE_FAVORITES // первоначальная очередь избранного до перемешивания
  for (uint8_t i = 0; i < MODE_AMOUNT; i++)
    shuffleFavoriteModes[i] = i;
#endif

  // EEPROM
  EepromManager::InitEepromSettings(                        // инициализация EEPROM; запись начального состояния настроек, если их там ещё нет; инициализация настроек лампы значениями из EEPROM
    modes, alarms, &ONflag, &dawnMode, &currentMode,
    &(FavoritesManager::ReadFavoritesFromEeprom),
    &(FavoritesManager::SaveFavoritesToEeprom),
    &(restoreSettings)); // не придумал ничего лучше, чем делать восстановление настроек по умолчанию в обработчике инициализации EepromManager


  // WI-FI

  LOG.printf_P(PSTR("Рабочий режим лампы: ESP_MODE = %d\n"), espMode);
  //Запускаем WIFI
  LOG.println(F("Старуем WIFI"));

  WiFi.persistent(false);   // Побережём EEPROM

  if (espMode == 0U)                                        // режим WiFi точки доступа
  {
    // Отключаем WIFI
    WiFi.disconnect();
    // Меняем режим на режим точки доступа
    WiFi.mode(WIFI_AP);
    // Задаем настройки сети
    if (sizeof(AP_STATIC_IP))
    {
      WiFi.softAPConfig(
        IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], AP_STATIC_IP[3]),      // IP адрес WiFi точки доступа
        IPAddress(AP_STATIC_IP[0], AP_STATIC_IP[1], AP_STATIC_IP[2], 1),                    // первый доступный IP адрес сети
        IPAddress(255, 255, 255, 0));                                                       // маска подсети
    }
    // Включаем WIFI в режиме точки доступа с именем и паролем
    // хронящихся в переменных _ssidAP _passwordAP в фвйле config.json
    WiFi.softAP(AP_NAME, AP_PASS);
    LOG.print(F("Старт WiFi в режиме точки доступа\n"));
    LOG.print(F("IP адрес: "));
    LOG.println(WiFi.softAPIP());
#ifdef GENERAL_DEBUG
    LOG.println ("*******************************************");
    LOG.print ("Heap Size after connection AP mode = ");
    LOG.println(system_get_free_heap_size());
    LOG.println ("*******************************************");
#endif
    connect = true;
    delay (100);
  }
  else                                                      // режим WiFi клиента. Подключаемся к роутеру
  {
    LOG.println(F("Старт WiFi в режиме клиента (подключение к роутеру)"));
    //	WIFI_start_station_mode ();


    WiFi.persistent(false);

    // Попытка подключения к Роутеру
    WiFi.mode(WIFI_STA);
    String _ssid = jsonRead(configSetup, "ssid");
    String _password = jsonRead(configSetup, "password");
    if (_ssid == "" && _password == "") {
      espMode = 0;
      jsonWrite(configSetup, "ESP_mode", (int)espMode);
      saveConfig();
      ESP.restart();
    }
    else {
      WiFi.begin(_ssid.c_str(), _password.c_str());
    }

    delay (100);
#ifdef USE_BLYNK
    Blynk.config(USE_BLYNK);
#endif
  }     //if (espMode == 0U) {...} else {...

  ESP.wdtFeed();

  LOG.printf_P(PSTR("Порт UDP сервера: %u\n"), localPort);
  Udp.begin(localPort);


  // NTP
#ifdef USE_NTP
  timeClient.begin();
  ESP.wdtFeed();
#endif

  // MQTT
#if (USE_MQTT)
  if (espMode == 1U)
  {
    mqttClient = new AsyncMqttClient();
    MqttManager::setupMqtt(mqttClient, inputBuffer, &sendCurrent);    // создание экземпляров объектов для работы с MQTT, их инициализация и подключение к MQTT брокеру
  }
  ESP.wdtFeed();
#endif

  // ОСТАЛЬНОЕ
  memset(matrixValue, 0, sizeof(matrixValue)); //это массив для эффекта Огонь. странно, что его нужно залить нулями
  randomSeed(micros());
  changePower();
  loadingFlag = true;

  //TextTicker = RUNNING_TEXT_DEFAULT;
  delay (100);

  my_timer = millis();
}


void loop()
{
  if (espMode) {
    if (WiFi.status() != WL_CONNECTED) {
      if ((millis() - my_timer) >= 1000UL) {
        my_timer = millis();
        if (ESP_CONN_TIMEOUT--) {
          LOG.print(F("."));
          ESP.wdtFeed();
        }
        else {
          // Если не удалось подключиться запускаем в режиме AP
          espMode = 0;
          jsonWrite(configSetup, "ESP_mode", (int)espMode);
          saveConfig();
          ESP.restart();
          //StartAPMode();
        }
      }
    }
    else {
      // Иначе удалось подключиться отправляем сообщение
      // о подключении и выводим адрес IP
      LOG.print(F("\nПодключение к роутеру установлено\n"));
      LOG.print(F("IP адрес: "));
      LOG.println(WiFi.localIP());
      long rssi = WiFi.RSSI();
      LOG.print(F("Уровень сигнала сети RSSI = "));
      LOG.print(rssi);
      LOG.println(F(" dbm"));
      connect = true;
      //ESP_CONN_TIMEOUT = 0;
      lastResolveTryMoment = 0;
#ifdef GENERAL_DEBUG
      LOG.println ("***********************************************");
      LOG.print ("Heap Size after connection Station mode = ");
      LOG.println(system_get_free_heap_size());
      LOG.println ("***********************************************");
#endif
      delay (100);
    }
  }

  if (connect || !espMode)  {
    my_timer = millis();
  }

  do {

    //delay (10);   //Для одной из плат(NodeMCU v3 без металлического экрана над ESP и Flash памятью) пришлось ставить задержку. Остальные работали нормально.

    if ((connect || !espMode) && ((millis() - my_timer) >= 10UL)) //Пришлось уменьшить частоту обращений к обработчику запросов web страницы, чтобы не использовать delay (10);.
    {
      HTTP.handleClient(); // Обработка запросов web страницы.
      my_timer = millis();
    }

    parseUDP();
    if (Painting == 0) {

      effectsTick();

      EepromManager::HandleEepromTick(&settChanged, &eepromTimeout, &ONflag,
                                      &currentMode, modes, &(FavoritesManager::SaveFavoritesToEeprom));

      //#ifdef USE_NTP
#if defined(USE_NTP) || defined(USE_MANUAL_TIME_SETTING) || defined(GET_TIME_FROM_PHONE)
      //if (millis() > 30 * 1000U) можно попытаться оттянуть срок первой попытки синхронизации времени на 30 секунд, чтобы роутер успел не только загрузиться, но и соединиться с интернетом
      timeTick();
#endif

#ifdef ESP_USE_BUTTON
      //if (buttonEnabled) в процедуре ведь есть эта проверка
      buttonTick();
#endif

#ifdef OTA
      otaManager.HandleOtaUpdate();                             // ожидание и обработка команды на обновление прошивки по воздуху
#endif

      TimerManager::HandleTimer(&ONflag, &settChanged,          // обработка событий таймера отключения лампы
                                &eepromTimeout, &changePower);

      if (FavoritesManager::HandleFavorites(                    // обработка режима избранных эффектов
            &ONflag,
            &currentMode,
            &loadingFlag
            //#ifdef USE_NTP
#if defined(USE_NTP) || defined(USE_MANUAL_TIME_SETTING) || defined(GET_TIME_FROM_PHONE)
            , &dawnFlag
#endif
            , &random_on
            , &selectedSettings
          ))
      {
#ifdef USE_BLYNK
        updateRemoteBlynkParams();
#endif
        FastLED.setBrightness(modes[currentMode].Brightness);
      }

#if USE_MQTT
      if (espMode == 1U && mqttClient && WiFi.isConnected() && !mqttClient->connected())
      {
        MqttManager::mqttConnect();                             // библиотека не умеет восстанавливать соединение в случае потери подключения к MQTT брокеру, нужно управлять этим явно
        MqttManager::needToPublish = true;
      }

      if (MqttManager::needToPublish)
      {
        if (strlen(inputBuffer) > 0)                            // проверка входящего MQTT сообщения; если оно не пустое - выполнение команды из него и формирование MQTT ответа
        {
          processInputBuffer(inputBuffer, MqttManager::mqttBuffer, true);
        }

        MqttManager::publishState();
      }
#endif

#ifdef USE_BLYNK
      if (espMode == 1U && WiFi.isConnected())
        Blynk.run();
#endif

#if defined(GENERAL_DEBUG) && GENERAL_DEBUG_TELNET
      handleTelnetClient();
#endif
    }//if (Painting == 0)
    ESP.wdtFeed();
  } while (connect);
}
