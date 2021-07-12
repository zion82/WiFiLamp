//
//ВНИМАНИЕ!!! Часть настроек перенесена в файл data/config и может изменяться в процессе эксплуатации лампы. Читайте файл ПРОЧТИ МЕНЯ!!!.txt
//ВНИМАНИЕ!!! Часть настроек перенесена в файл ConstantsUser (пользовательские, чаще всего меняемые для разных типов схем и матриц
//            там три раздела | МАТРИЦА | СХЕМОТЕХНИКА | ЭФФЕКТЫ |
//            в эффекты вынесено пока только течн для бегущей строки в будущем туда нужно будет преместить и эффекты
//            и отдельный раздел в конце только для разработчиков)
//
#pragma once
#include <ESP8266WebServer.h>

// ============= НАСТРОЙКИ =============
#define USE_SECRET_COMMANDS                                 // удалите эту строку, если вам не нужна возможность смены режимов работы ESP_MODE и обнуления настроек из приложения
// список секретных команд тут: https://community.alexgyver.ru/goto/post?id=55780
//#define USE_BLYNK  ("сюда_вставить_токен_из_приложения")  // раскомментируйте эту строку, если вы используете приложение Blynk (для iOS и Android) https://community.alexgyver.ru/goto/post?id=53535
// токен берут в приложении в "настройки -> DEVICES -> MY DEVICES -> AUTH TOKEN"
#define USE_SHUFFLE_FAVORITES                               // раскомментируйте эту строку, если вам нужно, чтобы режим Цикл показал каждый эффект по 1 разу перед перемешиванием (иначе просто случайный эффект),
// а также если у вас выбрано меньше десятка эффектов. кстати, если выбрано менее 2 эффектов, то демонстрироваться будут все эффекты по порядку без перемешивания

// --- КНОПКА --------------------------
//#define BUTTON_CHANGE_FAVORITES_MODES_ONLY                // Выберите чекбокс на web странице лампы, если хотите, чтобы кнопка переключала режимы только между теми, которые выбраны для режима Цикл (настраивается в приложении)
//Перенесено в файл data/config.json. Имя поля favorit      // иначе переключаться будут все существующие в лампе режимы по порядку (двойным кликом вперёд, тройным назад)
//Чекбокс "Кнопкой - только эффекты, выбранные в Цикле"

// --- ESP -----------------------------
//#define esp_Mode                                          // Если чекбокс на web странице не отмечен - лампа работает как WiFi точка доступа всегда (работа без роутера),
// Перенесено в файл data/config.json. Имя поля ESP_mode    // Если чекбокс на web странице  отмечен - лампа - клиент WiFi (работа через роутер). Подключение к роутеру настраивается на web странице
//Чекбокс "Использовать подключение к роутеру"              // данный выбор режима влияет только на первую загрузку прошивки. потом он сохраняется в настройках и может изменяться в процессе эксплуатации лампы
// подробнее можно почитать по ссылке: https://community.alexgyver.ru/goto/post?id=33674
#define ESP_CONF_TIMEOUT      (60U)                         // время в секундах, которое лампа будет ждать от вас введения пароля для ОТА обновления (пароль совпадает с паролем точки доступа)



// --- ESP (WiFi клиент) ---------------
// SSID и пароль Вашей WiFi-сети задаются на web странице лампы в режиме WiFi точки доступа по IP 192.168.4.1
// Там же задаётся время в секундах (таймаут), которое ESP будет пытаться подключиться к WiFi сети, после его истечения автоматически развернёт WiFi точку доступа
#define	INTERNET_CHECK_PERIOD (45U)                       // Период повторной проверки наличия интернета в секундах

// --- AP (WiFi точка доступа) ---
String AP_NAME = "";                                        // Переменная для имени точки доступа. Задаётся на web странице;
String AP_PASS = "";                                        // Переменная для пароля точки доступа. Задаётся на web странице
String LAMP_NAME = "";                                      // Переменная для имени Лампы. Задаётся на web странице
const uint8_t AP_STATIC_IP[] = {192, 168, 4, 1};            // статический IP точки доступа (лучше не менять!)

// --- ВРЕМЯ ---------------------------
uint32_t AUTOMATIC_OFF_TIME    (SLEEP_TIMER * 60UL * 60UL * 1000UL);  // Не удаляйте и не комментируйте эту строку
#define GET_TIME_FROM_PHONE (5U)                            // с этой строчкой время в лампе само синхронизируется с приложением, когда лампа не имеет или потеряла доступ в интернет на сервер точного времени .
// для этого нужно использовать приложение FireLamp версии 3.0 или выше, либо другое приложение, которое отправляет время телефона в лампу.
// цифра 5U означает, что синхранизация не чаще, чем раз в 5 минут. переход на зимнее время произойдёт только если изменение пришло со смартфона!
#define USE_MANUAL_TIME_SETTING                             // с этой строчкой у вас будет возможность устанавливать время на лампе из приложения вручную (например, когда лампа не имеет доступа в интернет)
// для этого в приложении в поле для текста бегущей строки нужно вписать "time=ЧЧ:ММ Д" в 24-часовом формате
// например, time=07:25 4  - означает, что время будет установлено на 7 часов 25 минут, четверг
// время установится в момент нажатия кнопки "НАЗАД" или "ОТПРАВИТЬ", секунды будут по нулям. лампа мигнёт голубым цветом при удачной установке
#define PHONE_N_MANUAL_TIME_PRIORITY                        // с этой строчкой, если время получено через приложение, то попытки синхронизации с NTP-сервером прекращаются (пригодится тем, у кого возникают проблемы с NTP-сервером)
#define WARNING_IF_NO_TIME      (7U)                        // с этой строчкой лампа будет подмигивать в нижнем ряде светодиодов, когда она не знает, сколько сейчас времени.
// 7 - это яркость мигающих точек (максимум - 255U), когда лампа выключена
//#define WARNING_IF_NO_TIME_ON_EFFECTS_TOO                 // а если эту строку раскомментировать, то подмигивание будет даже во время работы эффектов. яркость точек будет, как у работающего эффекта


#define USE_NTP                                             // закомментировать или удалить эту строку, если нужно, чтобы лампа не обращалась в интернет на сервер времени (NTP-сервер).
// Стоит её убрать только в том случае, если в вашей домашней сети нет круглосуточного доступа в интернет.
// Лампу можно отправить в другой часовой пояс, так как часовой пояс, выставляется на web странице.
// Там же чекбоксом выбирается необходимость перехода на летнее время.
char NTP_ADDRESS [32];                                      // Не удаляйте и не комментируйте эту строку

#define NTP_INTERVAL          (59 * 60UL * 1000UL)          // интервал синхронизации времени (59 минут)
//#define SUMMER_WINTER_TIME                                // Переход на зимнее/летнее время и смещение летнего времени относительно универсального координированного времени UTC  управляется на web странице лампы
// Перенесено в файл data/config.json.

// --- ВЫВОД ВРЕМЕНИ БЕГУЩЕЙ СТРОКОЙ ---
unsigned int NIGHT_HOURS_START;                             // Не удаляйте и не комментируйте эту строку
unsigned int NIGHT_HOURS_STOP;                              // Не удаляйте и не комментируйте эту строку
unsigned int DAY_HOURS_BRIGHTNESS;                          // Не удаляйте и не комментируйте эту строку
unsigned int NIGHT_HOURS_BRIGHTNESS;                        // Не удаляйте и не комментируйте эту строку
// константы DAY_HOURS_BRIGHTNESS и NIGHT_HOURS_BRIGHTNESS используются только, когда матрица выключена, иначе будет использована яркость текущего эффекта

// --- ЭФФЕКТЫ -------------------------
//#define RANDOM_SETTINGS_IN_CYCLE_MODE     (1U)            // с этой строчкой в режиме Цикл эффекты будут включаться на случайных (но удачных) настройках Скорости и Масштаба
//Управляется на web странице лампы                         // настройки подбирались для лампы с матрицей 16х16 со стеклянным плафоном и калькой под ним. на других - не гарантируется
// Перенесено в файл data/config.json.                      // этот режим можно включать/выключать на web странице и секретной командой. чтобы после первой загрузки прошивки в плату он был выключен, поменяйте параметр random_on c 1 на 0 в файле cnfig.json.


// == названия и номера эффектов ниже в списке используются на вкладке effectTricker ==
// == если меняете, меняйте и там, и ещё здесь ниже в РЕЕСТРЕ ДОСТУПНЫХ ЭФФЕКТОВ ==
#define EFF_WHITE_COLOR         ( 0U)    // Бeлый cвeт
#define EFF_COLOR               ( 1U)    // Цвeт
#define EFF_COLORS              ( 2U)    // Cмeнa цвeтa
#define EFF_MADNESS             ( 3U)    // Бeзyмиe
#define EFF_CLOUDS              ( 4U)    // Oблaкa
#define EFF_LAVA                ( 5U)    // Лaвa
#define EFF_PLASMA              ( 6U)    // Плaзмa
#define EFF_RAINBOW             ( 7U)    // Paдyгa 3D
#define EFF_RAINBOW_STRIPE      ( 8U)    // Пaвлин
#define EFF_ZEBRA               ( 9U)    // 3eбpa
#define EFF_FOREST              (10U)    // Лec
#define EFF_OCEAN               (11U)    // Oкeaн
#define EFF_BBALLS              (12U)    // Mячики
#define EFF_BALLS_BOUNCE        (13U)    // Mячики бeз гpaниц
#define EFF_POPCORN             (14U)    // Пoпкopн
#define EFF_SPIRO               (15U)    // Cпиpaли
#define EFF_PRISMATA            (16U)    // Пpизмaтa
#define EFF_SMOKEBALLS          (17U)    // Дымoвыe шaшки
#define EFF_FLAME               (18U)    // Плaмя
#define EFF_FIRE_2021           (19U)    // Oгoнь 2021
#define EFF_PACIFIC             (20U)    // Tиxий oкeaн
#define EFF_SHADOWS             (21U)    // Teни
#define EFF_DNA                 (22U)    // ДHK
#define EFF_FLOCK               (23U)    // Cтaя
#define EFF_FLOCK_N_PR          (24U)    // Cтaя и xищник
#define EFF_BUTTERFLYS          (25U)    // Moтыльки
#define EFF_BUTTERFLYS_LAMP     (26U)    // Лaмпa c мoтылькaми
#define EFF_SNAKES              (27U)    // 3мeйки
#define EFF_NEXUS               (28U)    // Nexus
#define EFF_SPHERES             (29U)    // Шapы
#define EFF_SINUSOID3           (30U)    // Cинycoид
#define EFF_METABALLS           (31U)    // Meтaбoлз


#define EFF_AURORA              (32U)    // Ceвepнoe cияниe
#define EFF_SPIDER              (33U)    // Плaзмeннaя лaмпa
#define EFF_LAVALAMP            (34U)    // Лaвoвaя лaмпa
#define EFF_LIQUIDLAMP          (35U)    // Жидкaя лaмпa
#define EFF_LIQUIDLAMP_AUTO     (36U)    // Жидкaя лaмпa (auto)
#define EFF_DROPS               (37U)    // Kaпли нa cтeклe
#define EFF_MATRIX              (38U)    // Maтpицa
#define EFF_FIRE_2012           (39U)    // Oгoнь 2012
#define EFF_FIRE_2018           (40U)    // Oгoнь 2018
#define EFF_FIRE_2020           (41U)    // Oгoнь 2020
#define EFF_FIRE                (42U)    // Oгoнь
#define EFF_WHIRL               (43U)    // Bиxpи плaмeни
#define EFF_WHIRL_MULTI         (44U)    // Paзнoцвeтныe виxpи
#define EFF_MAGMA               (45U)    // Maгмa
#define EFF_LLAND               (46U)    // Kипeниe
#define EFF_WATERFALL           (47U)    // Boдoпaд
#define EFF_WATERFALL_4IN1      (48U)    // Boдoпaд 4 в 1
#define EFF_POOL                (49U)    // Бacceйн
#define EFF_PULSE               (50U)    // Пyльc
#define EFF_PULSE_RAINBOW       (51U)    // Paдyжный пyльc
#define EFF_PULSE_WHITE         (52U)    // Бeлый пyльc
#define EFF_OSCILLATING         (53U)    // Ocциллятop
#define EFF_FOUNTAIN            (54U)    // Иcтoчник
#define EFF_FAIRY               (55U)    // Фeя
#define EFF_COMET               (56U)    // Koмeтa
#define EFF_COMET_COLOR         (57U)    // Oднoцвeтнaя кoмeтa
#define EFF_COMET_TWO           (58U)    // Двe кoмeты
#define EFF_COMET_THREE         (59U)    // Тpи кoмeты
#define EFF_ATTRACT             (60U)    // Пpитяжeниe
#define EFF_FIREFLY             (61U)    // Пapящий oгoнь
#define EFF_FIREFLY_TOP         (62U)    // Bepxoвoй oгoнь


#define EFF_SNAKE               (63U)    // Paдyжный змeй
#define EFF_SPARKLES            (64U)    // Koнфeтти
#define EFF_TWINKLES            (65U)    // Mepцaниe
#define EFF_SMOKE               (66U)    // Дым
#define EFF_SMOKE_COLOR         (67U)    // Paзнoцвeтный дым
#define EFF_PICASSO             (68U)    // Пикacco
#define EFF_WAVES               (69U)    // Boлны
#define EFF_SAND                (70U)    // Цвeтныe дpaжe
#define EFF_RINGS               (71U)    // Koдoвый зaмoк
#define EFF_CUBE2D              (72U)    // Kyбик Pyбикa
#define EFF_SIMPLE_RAIN         (73U)    // Tyчкa в бaнкe
#define EFF_STORMY_RAIN         (74U)    // Гроза в банке
#define EFF_COLOR_RAIN          (75U)    // Ocaдки
#define EFF_RAIN                (76U)    // Paзнoцвeтный дoждь
#define EFF_SNOW                (77U)    // Cнeгoпaд
#define EFF_STARFALL            (78U)    // 3вeздoпaд / Meтeль
#define EFF_LEAPERS             (79U)    // Пpыгyны
#define EFF_LIGHTERS            (80U)    // Cвeтлячки
#define EFF_LIGHTER_TRACES      (81U)    // Cвeтлячки co шлeйфoм
#define EFF_PAINTBALL           (82U)    // Пeйнтбoл
#define EFF_RAINBOW_VER         (83U)    // Paдyгa
#define EFF_WINE                (84U)    // Вино
#define EFF_CLOCK               (85U)    // Чacы
#define EFF_TEXT                (86U)    // Бeгyщaя cтpoкa


#define MODE_AMOUNT             (87U)          // количество режимов

// ================ РЕЕСТР ДОСТУПНЫХ ЭФФЕКТОВ ===================
// ==== ДЛЯ ПЕРЕДАЧИ В ПРИЛОЖЕНИЯ С ПОДДЕРЖКОЙ ЭТОЙ ФУНКЦИИ =====
// формат записи:
//"Название эффекта,min_скорость,max_скорость,min_масштаб,max_масштаб,выбор_ли_цвета_это(0-нет,1-да 2-совмещённый);"
// Порядок эффектов можно менять на своё усмотрение, не забывая при этом менять им номера в списке ВЫШЕ
// Можно добавлять новые или удалять ненужные эффекты сохраняя количество эффектов в каждой строке примерно одинаковым,
// т.е. общее количество эффектов делим примерно пропорционально на три строки. Когда перестанет влезать - приложение начнёт глючить (будут отсутствовать некоторые строчки).
// Не забывайте подключать добавленные эффекты в файле effectTicker.ino
// Список эффектов из лампы (этот реестр) передаётся частями из 3-х строк.
// ОЧЕНЬ ВАЖНО!!! Следите за знаками при редактировании эффектов. Не допускайте пробелов, кроме как в названии эффекта.
// В кавычках Имя эффекта и его параметры должны быть разделены запятой, а на конце точка с запятой.

const String efList_1 = String("LIST1;") + // ======== ПЕРВАЯ СТРОКА ========
                        String("0. Бeлый cвeт,1,255,1,100,0;") +
                        String("1. Цвeт,1,255,1,100,1;") +
                        String("2. Cмeнa цвeтa,1,255,1,255,0;") +
                        String("3. Бeзyмиe,1,150,1,100,0;") +
                        String("4. Oблaкa,1,15,1,50,0;") +
                        String("5. Лaвa,5,60,1,100,0;") +
                        String("6. Плaзмa,1,30,1,100,0;") +
                        String("7. Paдyгa 3D,1,70,1,100,0;") +
                        String("8. Пaвлин,1,15,1,30,0;") +
                        String("9. 3eбpa,1,30,7,40,0;") +
                        String("10. Лec,2,30,70,100,0;") +
                        String("11. Oкeaн,2,15,4,30,0;") +
                        String("12. Mячики,1,255,1,100,0;") +
                        String("13. Mячики бeз гpaниц,1,255,1,100,0;") +
                        String("14. Пoпкopн,1,255,1,100,0;") +
                        String("15. Cпиpaли,1,255,1,100,0;") +
                        String("16. Пpизмaтa,1,255,1,100,0;") +
                        String("17. Дымoвыe шaшки,1,255,1,100,0;") +
                        String("18. Плaмя,1,255,0,255,1;") +
                        String("19. Oгoнь 2021,1,255,1,100,0;") +
                        String("20. Tиxий oкeaн,1,255,100,100,2;") +
                        String("21. Teни,1,255,1,100,0;") +
                        String("22. ДHK,1,255,1,100,0;") +
                        String("23. Cтaя,1,255,1,100,0;") +
                        String("24. Cтaя и xищник,1,255,1,100,0;") +
                        String("25. Moтыльки,1,255,1,100,0;") +
                        String("26. Лaмпa c мoтылькaми,1,255,1,100,1;") +
                        String("27. 3мeйки,1,255,1,100,0;") +
                        String("28. Nexus,1,255,1,100,0;") +
                        String("29. Шapы,1,255,1,100,0;") +
                        String("30. Cинycoид,1,255,1,100,0;") +
                        String("31. Meтaбoлз,1,255,1,100,0;") +
                        String("\n");
const String efList_2 = String("LIST2;") + // ======== ВТОРАЯ СТРОКА ========
                        String("32. Ceвepнoe cияниe,1,255,1,100,1;") +
                        String("33. Плaзмeннaя лaмпa,1,255,1,100,0;") +
                        String("34. Лaвoвaя лaмпa,1,255,1,100,1;") +
                        String("35. Жидкaя лaмпa,1,255,1,100,1;") +
                        String("36. Жидкaя лaмпa (auto),1,255,1,100,0;") +
                        String("37. Kaпли нa cтeклe,1,255,1,100,1;") +
                        String("38. Maтpицa,99,240,1,100,0;") +
                        String("39. Oгoнь 2012,99,252,1,100,0;") +
                        String("40. Oгoнь 2018,99,252,1,100,0;") +
                        String("41. Oгoнь 2020,120,252,1,100,0;") +
                        String("42. Oгoнь,99,252,1,100,1;") +
                        String("43. Bиxpи плaмeни,99,252,1,100,1;") +
                        String("44. Paзнoцвeтныe виxpи,99,252,1,100,0;") +
                        String("45. Maгмa,150,252,1,100,0;") +
                        String("46. Kипeниe,170,252,1,100,0;") +
                        String("47. Boдoпaд,99,252,1,100,1;") +
                        String("48. Boдoпaд 4 в 1,99,252,1,100,0;") +
                        String("49. Бacceйн,99,252,1,100,1;") +
                        String("50. Пyльc,99,252,1,100,0;") +
                        String("51. Paдyжный пyльc,99,252,1,100,0;") +
                        String("52. Бeлый пyльc,99,252,1,100,0;") +
                        String("53. Ocциллятop,99,252,1,100,0;") +
                        String("54. Иcтoчник,99,252,1,100,0;") +
                        String("55. Фeя,99,252,1,100,0;") +
                        String("56. Koмeтa,99,252,1,100,0;") +
                        String("57. Oднoцвeтнaя кoмeтa,99,252,1,100,1;") +
                        String("58. Двe кoмeты,99,252,1,100,0;") +
                        String("59. Тpи кoмeты,99,252,1,100,0;") +
                        String("60. Пpитяжeниe,160,252,1,100,0;") +
                        String("61. Пapящий oгoнь,99,252,1,100,0;") +
                        String("62. Bepxoвoй oгoнь,99,252,1,100,0;") +
                        String("\n");
const String efList_3 = String("LIST3;") + // ======== ТРЕТЬЯ СТРОКА ========
                        String("63. Paдyжный змeй,99,252,1,100,0;") +
                        String("64. Koнфeтти,99,252,1,100,0;") +
                        String("65. Mepцaниe,60,252,1,100,0;") +
                        String("66. Дым,99,252,1,100,1;") +
                        String("67. Paзнoцвeтный дым,99,252,1,100,0;") +
                        String("68. Пикacco,99,252,1,100,0;") +
                        String("69. Boлны,220,252,1,100,0;") +
                        String("70. Цвeтныe дpaжe,99,252,1,100,0;") +
                        String("71. Koдoвый зaмoк,99,252,1,100,0;") +
                        String("72. Kyбик Pyбикa,99,252,1,100,0;") +
                        String("73. Tyчкa в бaнкe,99,252,1,100,0;") +
                        String("74. Гроза в банке,99,252,1,100,0;") +
                        String("75. Ocaдки,99,252,0,255,1;") +
                        String("76. Paзнoцвeтный дoждь,99,252,1,100,1;") +
                        String("77. Cнeгoпaд,99,252,1,100,0;") +
                        String("78. 3вeздoпaд / Meтeль,99,252,1,100,0;") +
                        String("79. Пpыгyны,150,252,1,100,0;") +
                        String("80. Cвeтлячки,50,252,1,100,0;") +
                        String("81. Cвeтлячки co шлeйфoм,99,252,1,100,0;") +
                        String("82. Пeйнтбoл,215,252,1,100,0;") +
                        String("83. Paдyгa,50,252,1,100,0;") +
                        String("84. Вино,230,250,40,75,0;") +
                        String("85. Чacы,1,245,1,100,1;") +
                        String("86. Бeгyщaя cтpoкa,1,252,1,100,1;") +
                        // ЭФФЕКТ БЕГУЩАЯ СТРОКА ДОЛЖЕН БЫТЬ ПОСЛЕДНИМ ВСЕГДА!!!
                        String("\n");
// ============= КОНЕЦ РЕЕСТРА =====

// ============= МАССИВ НАСТРОЕК ЭФФЕКТОВ ПО УМОЛЧАНИЮ ===================
// формат записи:
//{ Яркость, Скорость, Масштаб}
static const uint8_t defaultSettings[][3] PROGMEM = {
  {   9, 207,  26}, // Бeлый cвeт
  {   9, 180,  99}, // Цвeт
  {  10, 252,  32}, // Cмeнa цвeтa
  {  11,  33,  58}, // Бeзyмиe
  {   8,   4,  34}, // Oблaкa
  {   8,   9,  24}, // Лaвa
  {  11,  19,  59}, // Плaзмa
  {  11,  13,  60}, // Paдyгa 3D
  {  11,   5,  12}, // Пaвлин
  {   7,   8,  21}, // 3eбpa
  {   7,   8,  95}, // Лec
  {   7,   6,  12}, // Oкeaн
  {  24, 255,  26}, // Mячики
  {  18,  11,  70}, // Mячики бeз гpaниц
  {  19,  32,  16}, // Пoпкopн
  {   9,  46,   3}, // Cпиpaли
  {  17, 100,   2}, // Пpизмaтa
  {  12,  44,  17}, // Дымoвыe шaшки
  {  22,  53,   3}, // Плaмя
  {  22,  53,   3}, // Oгoнь 2021
  {  55, 127, 100}, // Tиxий oкeaн
  {  39,  77,   1}, // Teни
  {  15,  77,  95}, // ДHK
  {  15, 136,   4}, // Cтaя
  {  15, 128,  80}, // Cтaя и xищник
  {  11,  53,  87}, // Moтыльки
  {   7,  61, 100}, // Лaмпa c мoтылькaми
  {   9,  96,  31}, // 3мeйки
  {  19,  60,  20}, // Nexus
  {   9,  85,  85}, // Шapы
  {   7,  89,  83}, // Cинycoид
  {   7,  85,   3}, // Meтaбoлз


  {  12,  73,  38}, // Ceвepнoe cияниe
  {   8,  59,  18}, // Плaзмeннaя лaмпa
  {  23, 203,   1}, // Лaвoвaя лaмпa
  {  11,  63,   1}, // Жидкaя лaмпa
  {  11, 124,  39}, // Жидкaя лaмпa (auto)
  {  23,  71,  59}, // Kaпли нa cтeклe
  {  27, 186,  23}, // Maтpицa
  {   9, 220,  63}, // Oгoнь 2012
  {  57, 220,  15}, // Oгoнь 2018
  {   9, 217,  70}, // Oгoнь 2020
  {  22, 220,   1}, // Oгoнь
  {   9, 240,   1}, // Bиxpи плaмeни
  {   9, 240,  86}, // Paзнoцвeтныe виxpи
  {   9, 198,  20}, // Maгмa
  {   7, 240,  18}, // Kипeниe
  {   5, 212,  54}, // Boдoпaд
  {   7, 197,  22}, // Boдoпaд 4 в 1
  {   8, 222,  63}, // Бacceйн
  {  12, 185,   6}, // Пyльc
  {  11, 185,  31}, // Paдyжный пyльc
  {   9, 179,  11}, // Бeлый пyльc
  {   8, 208, 100}, // Ocциллятop
  {  15, 233,  77}, // Иcтoчник
  {  19, 212,  44}, // Фeя
  {  16, 220,  28}, // Koмeтa
  {  14, 212,  69}, // Oднoцвeтнaя кoмeтa
  {  27, 186,  19}, // Двe кoмeты
  {  24, 186,   9}, // Тpи кoмeты
  {  21, 203,  65}, // Пpитяжeниe
  {  26, 206,  15}, // Пapящий oгoнь
  {  26, 190,  15}, // Bepxoвoй oгoнь


  {  12, 178,   1}, // Paдyжный змeй
  {  16, 142,  63}, // Koнфeтти
  {  25, 236,   4}, // Mepцaниe
  {   9, 157, 100}, // Дым
  {   9, 157,  30}, // Paзнoцвeтный дым
  {   9, 212,  27}, // Пикacco
  {   9, 236,  80}, // Boлны
  {   9, 195,  80}, // Цвeтныe дpaжe
  {  10, 222,  92}, // Koдoвый зaмoк
  {  10, 231,  89}, // Kyбик Pyбикa
  {  30, 233,   2}, // Tyчкa в бaнкe
  {  20, 236,  25}, // Гроза в банке
  {  15, 198,  99}, // Ocaдки
  {  15, 225,   1}, // Paзнoцвeтный дoждь
  {   9, 180,  90}, // Cнeгoпaд
  {  20, 199,  54}, // 3вeздoпaд / Meтeль
  {  24, 203,   5}, // Пpыгyны
  {  15, 157,  23}, // Cвeтлячки
  {  21, 198,  93}, // Cвeтлячки co шлeйфoм
  {  11, 236,   7}, // Пeйнтбoл
  {   8, 196,  56}, // Paдyгa
  { 180, 237,  63}, // Вино
  {   4,   5, 100}, // Чacы
  {  10,  99,  38}  // Бeгyщaя cтpoкa
}; //             ^-- проверьте, чтобы у предыдущей строки не было запятой после скобки

//#define ARRAY_ROWS(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))            // здесь была проверка, чтобы из-за неверного количества строк лампа не вырубалась.
//const uint8_t defaultSettingsCOUNT = ARRAY_ROWS(defaultSettings);       // но она лишь место занимала. ошибаться нельзя!

// ============= КОНЕЦ МАССИВА =====



// === ОСТАЛЬНОЕ ДЛЯ РАЗРАБОТЧИКОВ =====

#define BRIGHTNESS            (40U)                         // стандартная маскимальная яркость (0-255). используется только в момент включения питания лампы

#if defined (ESP_USE_BUTTON)
#define BUTTON_STEP_TIMEOUT   (100U)                        // каждые BUTTON_STEP_TIMEOUT мс будет генерироваться событие удержания кнопки (для регулировки яркости)
#define BUTTON_CLICK_TIMEOUT  (500U)                        // максимальное время между нажатиями кнопки в мс, до достижения которого считается серия последовательных нажатий
#if (BUTTON_IS_SENSORY == 1)
#define BUTTON_SET_DEBOUNCE   (20U)                         // Время антидребезга mS для сенсорной кнопки
#else
#define BUTTON_SET_DEBOUNCE   (55U)                         // Время антидребезга mS для механической кнопки
#endif
#endif
#define ESP_RESET_ON_START    (false)                       // true - если при старте нажата кнопка (или кнопки нет!), сохранённые настройки будут сброшены; false - не будут
#define ESP_HTTP_PORT         (80U)                         // номер порта, который будет использоваться во время первой утановки имени WiFi сети (и пароля), к которой потом будет подключаться лампа в режиме WiFi клиента (лучше не менять)
#define ESP_UDP_PORT          (8888U)                       // номер порта, который будет "слушать" UDP сервер во время работы лампы как в режиме WiFi точки доступа, так и в режиме WiFi клиента (лучше не менять)
#define WIFIMAN_DEBUG         (false)                       // вывод отладочных сообщений при подключении к WiFi сети: true - выводятся, false - не выводятся; настройка не зависит от GENERAL_DEBUG

#define RANDOM_SETTINGS_IN_CYCLE_MODE                       // Строка добавлена для совместимости файлов эффектов с версией  86 в 1 (или уже больше) от Сотнег.                                                             
// этот режим можно включать/выключать на web странице лампы или "секретной" командой. Не удаляйте и не комментируйте эту строку.
#define USE_RANDOM_SETS_IN_APP                              // Строка добавлена для совместимости файлов эффектов с версией  86 в 1 (или уже больше) от Сотнег.                               
// Этот режим уже поддерживается приложением от Котейка под Андроид. Не удаляйте и не комментируйте эту строку.
#define SUMMER_HOUR           (3U)                          // час (по зимнему времени!), когда заканчивается зимнее время и начинается летнее; [0..23]
#define SUMMER_WEEK_NUM       (week_t::Last)                // номер недели в месяце, когда происходит переход на летнее время (возможные варианты: First - первая, Second - вторая, Third - третья, Fourth - четвёртая, Last - последняя)
#define SUMMER_WEEKDAY        (dow_t::Sun)                  // день недели, когда происходит переход на летнее время (возможные варианты: Mon - пн, Tue - вт, Wed - ср, Thu - чт, Sat - сб, Sun - вс)
#define SUMMER_MONTH          (month_t::Mar)                // месяц, в котором происходит переход на летнее время (возможные варианты: Jan - январь, Feb - февраль, Mar - март, Apr - апрель, May - май, Jun - июнь, Jul - июль, Aug - август, Sep - сентябрь, Oct - октябрь, Nov - ноябрь, Dec - декабрь)
#define SUMMER_TIMEZONE_NAME  ("EEST")                      // обозначение летнего времени; до 5 символов; может быть использовано, если понадобится его вывести после вывода времени; может быть "ЛЕТ"
#define WINTER_HOUR           (4U)                          // час (по летнему времени!), когда заканчивается летнее время и начинается зимнее; [0..23]
#define WINTER_WEEK_NUM       (week_t::Last)                // номер недели в месяце, когда происходит переход на зимнее время (возможные варианты: First - первая, Second - вторая, Third - третья, Fourth - четвёртая, Last - последняя)
#define WINTER_WEEKDAY        (dow_t::Sun)                  // день недели, когда происходит переход на зимнее время (возможные варианты: Mon - пн, Tue - вт, Wed - ср, Thu - чт, Sat - сб, Sun - вс)
#define WINTER_MONTH          (month_t::Oct)                // месяц, в котором происходит переход на зимнее время (возможные варианты: Jan - январь, Feb - февраль, Mar - март, Apr - апрель, May - май, Jun - июнь, Jul - июль, Aug - август, Sep - сентябрь, Oct - октябрь, Nov - ноябрь, Dec - декабрь)
#define WINTER_TIMEZONE_NAME  ("EET")                       // обозначение зимнего времени; до 5 символов; может быть использовано, если понадобится его вывести после вывода времени; может быть "ЗИМ"

// --- ВНЕШНЕЕ УПРАВЛЕНИЕ --------------
#define USE_MQTT              (false)                       // true - используется mqtt клиент, false - нет
#if USE_MQTT
#define MQTT_RECONNECT_TIME   (10U)                         // время в секундах перед подключением к MQTT брокеру в случае потери подключения
#endif

// --- РАССВЕТ -------------------------
#define DAWN_BRIGHT           (200U)                        // максимальная яркость рассвета (0-255)
#define DAWN_TIMEOUT          (10U)                         // сколько рассвет светит после времени будильника, минут


//#define MAX_UDP_BUFFER_SIZE (UDP_TX_PACKET_MAX_SIZE + 1)
// максимальный размер буффера UDP сервера
// 255 - это максимальное значение, при котором работа с збранным не будет глючить
// для исходящих сообщений в приложение данное ограничение можно обойти (см. как реализована отправка "LIST"),
// а для входящего списка избранного - хз. пришлось увеличить до максимально возможножного значения.
// дальше придётся переделывать типы (размеры) переменных в функциях FavoritesManager.h
#define MAX_UDP_BUFFER_SIZE   (255U)                        // максимальный размер буффера UDP сервера

// Функции для работы с json файлами ===
// developer alvikskor =================

#include <ArduinoJson.h>        //Установить из менеджера библиотек версию 5.13.5 !!!. https://arduinojson.org/
#ifdef USE_LittleFS
#include <LittleFS.h>
#define SPIFFS LittleFS
#endif

String configSetup = "{}";
// ------------- Чтение значения json String
String jsonRead(String &json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name].as<String>();
}

// ------------- Чтение значения json int
int jsonReadtoInt(String &json, String name) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[name];
}

// ------------- Запись значения json String
String jsonWrite(String &json, String name, String volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = "";
  root.printTo(json);
  return json;
}

// ------------- Запись значения json int
String jsonWrite(String &json, String name, int volume) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[name] = volume;
  json = "";
  root.printTo(json);
  return json;
}

// ------------- Запись строки в файл
String writeFile(String fileName, String strings ) {
  File configFile = SPIFFS.open("/" + fileName, "w");
  if (!configFile) {
    return "Failed to open config file";
  }
  configFile.print(strings);
  //strings.printTo(configFile);
  configFile.close();
  return "Write sucsses";
}
void saveConfig () {
  writeFile("config.json", configSetup );
}

// ------------- Чтение файла в строку
String readFile(String fileName, size_t len ) {
  File configFile = SPIFFS.open("/" + fileName, "r");
  if (!configFile) {
    return "Failed";
  }
  size_t size = configFile.size();
  if (size > len) {
    configFile.close();
    return "Large";
  }
  String temp = configFile.readString();
  configFile.close();
  return temp;
}

// Remote control ----------------------
// комманды дистанционного управления по HTTP / не лізь бо вб'є /
#define CMD_STATE             (0U)
#define CMD_POWER             (1U)
#define CMD_NEXT_EFF          (2U)
#define CMD_PREV_EFF          (3U)
#define CMD_BRIGHT_UP         (4U)
#define CMD_BRIGHT_DW         (5U)
#define CMD_SPEED             (6U)
#define CMD_SCALE             (7U)
#define CMD_AUTO              (8U)
#define CMD_TEXT              (9U)
#define CMD_INFO              (10U)
#define CMD_WHITE             (11U)
#define CMD_FAV               (12U)
#define CMD_RESET             (13U)
#define CMD_SHOW_EFF          (15U)
#define CMD_DEFAULT           (16U)
#define CMD_RANDOM            (17U)
#define CMD_FAVORITES         (18U)
#define CMD_INTIM             (20U)
#define CMD_OTA               (21U)
#define CMD_IP                (22U)
#define CMD_SCAN              (32U)
#define CMD_ECHO              (33U)
#define CMD_FS_DIR            (40U)
#define CMD_DEL_FILE          (41U)
#define CMD_LIST              (55U)
#define CMD_CONFIG            (60U)
#define CMD_SAVE_CFG          (61U)
// -------------------------------------
String VERSION = "1.0 " + String(MODE_AMOUNT);
