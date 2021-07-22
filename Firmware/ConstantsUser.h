// =====================================
// User Constants
// =====================================
#include <ESP8266WebServer.h>
// можно перед прошивкой лампы прописать установки домашней сети и уже после настроить свои предпочтения по вкусу
// ОСНОВНЫЕ ПАРМЕТРЫ | использовать роутер |  ssid сети    |              пароль    | все остальное не так важно|
#define DEF_CONFIG "{\"ESP_mode\":1,\"ssid\":\"XXXXXXXXX\",\"password\":\"XXXXXXXXX\",\"button_on\":1,\"Power\":0,\"SSDP\":\"LedLampDEV\",\"Summer_Time\":1,\"TimeOut\":60,\"day_bright\":250,\"day_time\":8,\"effect_always\":1,\"favorit\":0,\"night_bright\":5,\"night_time\":22,\"ntp\":\"pl.pool.ntp.org\",\"passwordAP\":\"31415926\",\"print_time\":0,\"random_on\":0,\"run_text\":\"Lamp by Gyver\",\"ssidAP\":\"WiFi\",\"time\":\"00:00:00\",\"time_always\":0,\"timer5h\":1,\"timezone\":2}"
// !!! будьте внимательны менять нужно только XXXXXXXXX количество знаков такое как у SSID и пароля но никаких пробелов

// === СХЕМОТЕХНИКА  ===================
#define ESP_USE_BUTTON                                      // если строка не закомментирована, должна быть подключена кнопка (иначе лампа может регистрировать "фантомные" нажатия и некорректно устанавливать яркость)
#define BUTTON_IS_SENSORY     (1U)                           // если у вас не сенсорная, а обычная кнопка, поменяйте в этой строчке 1 на нолик 
#define BUTTON_LOCK_ON_START                                // с этой строкой, если в момент включения лампы в розетку успеть нажать кнопку, или если вы забудете кнопку подключить, лампа отключит реакцию на нажатие кнопки до следующего раза
#define LED_PIN               (0U)                          // пин ленты                (D3)
#define BTN_PIN               (4U)                          // пин кнопки               (D2)
//#define MOSFET_PIN            (5U)                        // пин MOSFET транзистора   (D1) - может быть использован для управления питанием матрицы/ленты (если раскомментировать строку)
//#define MOSFET_LEVEL          (HIGH)                      // логический уровень, в который будет установлен пин MOSFET_PIN, когда матрица включена - HIGH или LOW (если раскомментировать)
//#define ALARM_PIN             (16U)                       // пин состояния будильника (D0) - может быть использован для управления каким-либо внешним устройством на время работы будильника (если раскомментировать)
//#define ALARM_LEVEL           (HIGH)                      // логический уровень, в который будет установлен пин ALARM_PIN, когда "рассвет"/будильник включен (если раскомментировать)
//#define USE_LittleFS                                      // Раскомментируйте эту строку, если вместо файловой системы SPIFFS хотите использовать файловую систему LittlFS
// --- МАТРИЦА -------------------------
#define WIDTH                 (16U)                         // ширина матрицы
#define HEIGHT                (16U)                         // высота матрицы
#define COLOR_ORDER           (GRB)                         // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB
#define MATRIX_TYPE           (0U)                          // тип матрицы: 0 - зигзаг, 1 - параллельная
#define CONNECTION_ANGLE      (2U)                          // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define STRIP_DIRECTION       (2U)                          // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
#define CURRENT_LIMIT         (1800U)                       // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит

// при неправильной настройке матрицы вы получите предупреждение "Wrong matrix parameters! Set to default"
// шпаргалка по настройке матрицы здесь: https://alexgyver.ru/wp-content/uploads/2018/11/scheme3.jpg
// (но в ней, кажется, перепутаны местами квадратики в 4м столбце, а может, и ещё какие-то)

// --- ЛЕНТА ----------------------------
// если у вас не обычная матрица, а сборка из ленты или гирлянда, и возникают непонятной природы артефакты в рисунке эффекта, попробуйте менять следующие три строчки
//#define FASTLED_ALLOW_INTERRUPTS      (1)                 // default: 1; // Use this to force FastLED to allow interrupts in the clockless chipsets (or to force it to disallow), overriding the default on platforms that support this. Set the value to 1 to allow interrupts or 0 to disallow them.
#define FASTLED_INTERRUPT_RETRY_COUNT   (0)                 // default: 2; // Use this to determine how many times FastLED will attempt to re-transmit a frame if interrupted for too long by interrupts
#define FASTLED_ESP8266_RAW_PIN_ORDER                       // FASTLED_ESP8266_RAW_PIN_ORDER, FASTLED_ESP8266_D1_PIN_ORDER or FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define NUM_LEDS              (uint16_t)(WIDTH * HEIGHT)
#define SEGMENTS              (1U)                          // диодов в одном "пикселе" (для создания матрицы из кусков ленты). вряд ли эффекты будут корректно работать, если изменить этот параметр

// === ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ==========
//#define USE_OLD_IOS_APP                                   // раскомментируйте эту строку, если вы используете приложение для iOS "Arduino Lamp" (удалено из Апстора, исходники на форуме)

// === РЕЖИМЫ РАБОТЫ  ===================
uint8_t DONT_TURN_ON_AFTER_SHUTDOWN;                        // Не удаляйте и не комментируйте эту строку
uint8_t SLEEP_TIMER           (3U);                         // Время автоотключения лампы (час)
// === ЭФФЕКТЫ =========================
#define USE_RANDOM_SETS_IN_APP                              // раскоментируйте эту строчку, если в вашем приложении появилась поддержка команд "RND_"
#define EFF_FAV               (28U)                         // любимый эффект под него будет отдельная кнопка / выбирается по номеру / пример 26U Nexus

// --- НАСТРОЙКИ Бегущей Строки --------
#define TEXT_DIRECTION        (1U)                          // 1 - по горизонтали, 0 - по вертикали
#define MIRR_V                (0U)                          // отразить текст по вертикали (0 / 1)
#define MIRR_H                (0U)                          // отразить текст по горизонтали (0 / 1)

#define TEXT_HEIGHT           (2U)                          // высота, на которой бежит текст (от низа матрицы)
#define LET_WIDTH             (5U)                          // ширина буквы шрифта (обусловлено используемым шрифтом)
#define LET_HEIGHT            (8U)                          // высота буквы шрифта (обусловлено используемым шрифтом)
#define SPACE                 (1U)                          // пробел
#define LETTER_COLOR          (CRGB::White)                 // цвет букв по умолчанию
#define RUNNING_TEXT_DEFAULT  ("Version 1.2 87")            // текст, который будет выводиться в эффекте Бегущая строка по умолчанию. его можно менять в приложении по

// =====================================
// Developers Constants
// =====================================
// DEBUG -------------------------------
#define GENERAL_DEBUG                                       // если строка не закомментирована, будут выводиться отладочные сообщения
#define OTA                                                 // если строка не закомментирована, модуль будет ждать два последовательных запроса пользователя на прошивку по воздуху (два четрёхкратных нажатия на кнопку)
#ifdef OTA
#define ESP_OTA_PORT          (8266U)                       // номер порта, который будет "прослушиваться" в ожидании команды прошивки по воздуху
#endif

#define GENERAL_DEBUG_TELNET  (false)                       // true - отладочные сообщения будут выводиться в telnet вместо Serial порта (для удалённой отладки без подключения usb кабелем)
#define TELNET_PORT           (23U)                         // номер telnet порта
#if (defined(GENERAL_DEBUG) && GENERAL_DEBUG_TELNET)
WiFiServer telnetServer(TELNET_PORT);                       // telnet сервер
WiFiClient telnet;                                          // обработчик событий telnet клиента
bool telnetGreetingShown = false;                           // признак "показано приветствие в telnet"
#define LOG                   telnet
#else
#define LOG                   Serial
#endif
//--------------------------------------
