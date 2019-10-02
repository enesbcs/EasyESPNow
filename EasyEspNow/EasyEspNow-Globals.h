#ifndef EASYESPNOW_GLOBALS_H_
#define EASYESPNOW_GLOBALS_H_

// ********************************************************************************
//   User specific configuration
// ********************************************************************************

// Set default configuration settings if you want (not mandatory)
// You can always change these during runtime and save to eeprom
// After loading firmware, issue a 'reset' command to load the defaults.

//#define USE_WIFI
#define USE_ESPNOW

#define DEFAULT_NAME        "espnowdevice"      // Enter your device friendly name
#define DEFAULT_DELAY       60                  // Enter your Send delay in seconds

#define UNIT                       254          // own Node ID
#define DEFAULT_DESTINATION_NODE   0            // default destination ID (0=broadcast)

#define WIFI_CHANNEL 1                          // For ESPNOW
#define ALIVE_PERIOD 300000                     // in milliseconds

#ifdef USE_ESPNOW
 #if defined(ESP8266)
 extern "C"{
   #include <espnow.h>
  }
  #include <ESP8266WiFi.h>
 #elif defined(ESP32)
 extern "C"{
   #include <esp_now.h>
  }
  #include <WiFi.h>
  #include <esp_wifi.h>
 #else
  #error "This library supports ESP8266 and ESP32 only."
 #endif
#endif

//#define ESPNOW_ROLE ESP_NOW_ROLE_CONTROLLER // for SENDING only
#define ESPNOW_ROLE ESP_NOW_ROLE_COMBO        // for 2 WAY COMM

#define ESPNOW_SERIAL_GATEWAY             0  // for 2 way and serial GW
#define ESPNOW_SEND_ONLY_ENDPOINT         1
#define ESPNOW_RECEIVE_ONLY_ENDPOINT      2
#define ESPNOW_SEND_AND_RECEIVE_ENDPOINT  3  // for 2 WAY COMM

#define ESPNOW_PURPOSE   ESPNOW_SEND_AND_RECEIVE_ENDPOINT

#define DEFAULT_PROTOCOL    1                   // Protocol used for controller communications

#define MIN_SPIFFS_SECTORS  16  // 32=128k,16=64k

#define DEFAULT_AP_KEY      "configesp"         // Enter network WPA key for AP (config) mode
#define DEFAULT_MQTT_TEMPLATE false              // true or false enabled or disabled set mqqt sub and pub
#define DEFAULT_USE_STATIC_IP false           // true or false enabled or disabled set static IP
#define DEFAULT_SSID        "ssid"              // Enter your network SSID
#define DEFAULT_KEY         "wpakey"            // Enter your network WPA key

#ifdef USE_WIFI
 #define DEFAULT_SERVER      "192.168.0.8"       // Enter your Domoticz Server IP address
 #define DEFAULT_PORT        8080                // Enter your Domoticz Server port value
 #define DEFAULT_IP          "192.168.0.50"      // Enter your IP address
 #define DEFAULT_DNS         "192.168.0.1"       // Enter your DNS
 #define DEFAULT_GW          "192.168.0.1"       // Enter your gateway
 #define DEFAULT_SUBNET      "255.255.255.0"     // Enter your subnet

 #define DEFAULT_MQTT_PUB    "sensors/espeasy/%sysname%/%tskname%/%valname%" // Enter your pub
 #define DEFAULT_MQTT_SUB    "sensors/espeasy/%sysname%/#" // Enter your sub
#endif

// Enable FEATURE_ADC_VCC to measure supply voltage using the analog pin
// Please note that the TOUT pin has to be disconnected in this mode
// Use the "System Info" device to read the VCC value
#define FEATURE_ADC_VCC                  false

// ********************************************************************************
//   DO NOT CHANGE ANYTHING BELOW THIS LINE
// ********************************************************************************
#define ESP_PROJECT_PID           2016110801L
#define VERSION                             2
#define BUILD                             148
#define BUILD_NOTES                 " - ESPNOW"

#define MAX_FLASHWRITES_PER_DAY           100 // per 24 hour window

#define NODE_TYPE_ID_ESP_EASY_STD           1
#define NODE_TYPE_ID_ESP_EASYM_STD         17
#define NODE_TYPE_ID_ESP_EASY32_STD        33
#define NODE_TYPE_ID_ARDUINO_EASY_STD      65
#define NODE_TYPE_ID_NANO_EASY_STD         81
#define NODE_TYPE_ID                        NODE_TYPE_ID_ESP_EASYM_STD

#define PLUGIN_INIT_ALL                     1
#define PLUGIN_INIT                         2
#define PLUGIN_READ                         3
#define PLUGIN_ONCE_A_SECOND                4
#define PLUGIN_TEN_PER_SECOND               5
#define PLUGIN_DEVICE_ADD                   6
#define PLUGIN_EVENTLIST_ADD                7
#define PLUGIN_WEBFORM_SAVE                 8
#define PLUGIN_WEBFORM_LOAD                 9
#define PLUGIN_WEBFORM_SHOW_VALUES         10
#define PLUGIN_GET_DEVICENAME              11
#define PLUGIN_GET_DEVICEVALUENAMES        12
#define PLUGIN_WRITE                       13
#define PLUGIN_EVENT_OUT                   14
#define PLUGIN_WEBFORM_SHOW_CONFIG         15
#define PLUGIN_SERIAL_IN                   16
#define PLUGIN_UDP_IN                      17
#define PLUGIN_CLOCK_IN                    18
#define PLUGIN_TIMER_IN                    19
#define PLUGIN_FIFTY_PER_SECOND            20
#define PLUGIN_REMOTE_CONFIG               21

#define CPLUGIN_PROTOCOL_ADD                1
#define CPLUGIN_PROTOCOL_TEMPLATE           2
#define CPLUGIN_PROTOCOL_SEND               3
#define CPLUGIN_PROTOCOL_RECV               4
#define CPLUGIN_GET_DEVICENAME              5
#define CPLUGIN_WEBFORM_SAVE                6
#define CPLUGIN_WEBFORM_LOAD                7

#define NPLUGIN_PROTOCOL_ADD                1
#define NPLUGIN_GET_DEVICENAME              2
#define NPLUGIN_WEBFORM_SAVE                3
#define NPLUGIN_WEBFORM_LOAD                4
#define NPLUGIN_WRITE                       5
#define NPLUGIN_NOTIFY                      6

#define LOG_LEVEL_ERROR                     1
#define LOG_LEVEL_INFO                      2
#define LOG_LEVEL_DEBUG                     3
#define LOG_LEVEL_DEBUG_MORE                4
#define LOG_LEVEL_REPLY                     0

#define CMD_REBOOT                         89
#define CMD_WIFI_DISCONNECT               135

#define DEVICES_MAX                        64
#if defined(ESP8266)
  #define TASKS_MAX                          12 // max 12!
  #define MAX_GPIO                           16
  #define FILE_CONFIG       "config.dat"
  #define FILE_SECURITY     "security.dat"
  #define FILE_NOTIFICATION "notification.dat"
  #define FILE_RULES        "rules1.txt"
#endif
#if defined(ESP32)
  #define TASKS_MAX                          32
  #define MAX_GPIO                           39
  #define FILE_CONFIG       "/config.dat"
  #define FILE_SECURITY     "/security.dat"
  #define FILE_NOTIFICATION "/notification.dat"
  #define FILE_RULES        "/rules1.txt"
#endif
#define CONTROLLER_MAX                      3 // max 4!
#define NOTIFICATION_MAX                    3 // max 4!
#define VARS_PER_TASK                       4
#define PLUGIN_MAX                         64
#define PLUGIN_CONFIGVAR_MAX                8
#define PLUGIN_CONFIGFLOATVAR_MAX           4
#define PLUGIN_CONFIGLONGVAR_MAX            4
#define PLUGIN_EXTRACONFIGVAR_MAX          16
#define CPLUGIN_MAX                        16
#define NPLUGIN_MAX                         4
#define UNIT_MAX                           32 // Only relevant for UDP unicast message 'sweeps' and the nodelist.
#define RULES_TIMER_MAX                     8
#define SYSTEM_TIMER_MAX                    8
#define SYSTEM_CMD_TIMER_MAX                2
#define PINSTATE_TABLE_MAX                 32
#define RULES_MAX_SIZE                   2048
#define RULES_MAX_NESTING_LEVEL             3
#define RULESETS_MAX                        4

#define PIN_MODE_UNDEFINED                  0
#define PIN_MODE_INPUT                      1
#define PIN_MODE_OUTPUT                     2
#define PIN_MODE_PWM                        3
#define PIN_MODE_SERVO                      4

#define SEARCH_PIN_STATE                 true
#define NO_SEARCH_PIN_STATE             false

#define DEVICE_TYPE_SINGLE                  1  // connected through 1 datapin
#define DEVICE_TYPE_I2C                     2  // connected through I2C
#define DEVICE_TYPE_ANALOG                  3  // tout pin
#define DEVICE_TYPE_DUAL                    4  // connected through 2 datapins
#define DEVICE_TYPE_DUMMY                  99  // Dummy device, has no physical connection

#define SENSOR_TYPE_SINGLE                  1
#define SENSOR_TYPE_TEMP_HUM                2
#define SENSOR_TYPE_TEMP_BARO               3
#define SENSOR_TYPE_TEMP_HUM_BARO           4
#define SENSOR_TYPE_DUAL                    5
#define SENSOR_TYPE_TRIPLE                  6
#define SENSOR_TYPE_QUAD                    7
#define SENSOR_TYPE_SWITCH                 10
#define SENSOR_TYPE_DIMMER                 11
#define SENSOR_TYPE_LONG                   20

#define VALUE_SOURCE_SYSTEM                 1
#define VALUE_SOURCE_SERIAL                 2
#define VALUE_SOURCE_HTTP                   3
#define VALUE_SOURCE_MQTT                   4
#define VALUE_SOURCE_UDP                    5
#define VALUE_SOURCE_ESPNOW                66

#define BOOT_CAUSE_MANUAL_REBOOT            0
#define BOOT_CAUSE_COLD_BOOT                1
#define BOOT_CAUSE_EXT_WD                  10

#define DAT_TASKS_SIZE                   2048
#define DAT_TASKS_CUSTOM_OFFSET          1024
#define DAT_CONTROLLER_SIZE              1024
#define DAT_NOTIFICATION_SIZE            1024

#if defined(ESP8266)
  #define DAT_OFFSET_TASKS                 4096  // each task = 2k, (1024 basic + 1024 bytes custom), 12 max
  #define DAT_OFFSET_CONTROLLER           28672  // each controller = 1k, 4 max
  #define DAT_OFFSET_CUSTOMCONTROLLER    32768  // each custom controller config = 1k, 4 max.
  #define CONFIG_FILE_SIZE                65536
#endif
#if defined(ESP32)
  #define DAT_OFFSET_CONTROLLER            8192  // each controller = 1k, 4 max
  #define DAT_OFFSET_CUSTOMCONTROLLER    12288  // each custom controller config = 1k, 4 max.
  #define DAT_OFFSET_TASKS                32768  // each task = 2k, (1024 basic + 1024 bytes custom), 32 max
  #define CONFIG_FILE_SIZE               131072
#endif

#ifdef USE_WIFI
 #include <ESP8266WiFi.h>
 #include <DNSServer.h>
 #include <WiFiUdp.h>
 #include <ESP8266WebServer.h>
 #include <PubSubClient.h>
 #include <ESP8266HTTPUpdateServer.h>
 ESP8266HTTPUpdateServer httpUpdater(true);
#endif

#if defined(ESP32)
  #define ARDUINO_ESP8266_RELEASE "2_4_0"
  #define NODE_TYPE_ID                        NODE_TYPE_ID_ESP_EASY32_STD
  #define ICACHE_RAM_ATTR IRAM_ATTR
  #include "SPIFFS.h"
  #include <rom/rtc.h>
  #define PIN_D_MAX        39
  #define ESP_NOW_ROLE_IDLE       0
  #define ESP_NOW_ROLE_CONTROLLER 1
  #define ESP_NOW_ROLE_SLAVE      2
  #define ESP_NOW_ROLE_COMBO      3
  int8_t ledChannelPin[16]; 
#endif
#if defined(ESP8266)
extern "C" {
#include "user_interface.h"
}
extern "C" {
#include "spi_flash.h"
}
#include <Servo.h>
Servo myservo1;
Servo myservo2;
#endif

#include <Wire.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#define FS_NO_GLOBALS
#include <FS.h>

#ifdef FEATURE_SD
 #include <SD.h>
#endif

#include <base64.h>

#if FEATURE_ADC_VCC
ADC_MODE(ADC_VCC);
#endif

#ifdef USE_WIFI
 #ifndef LWIP_OPEN_SRC
  #define LWIP_OPEN_SRC
 #endif
 #include "lwip/opt.h"
 #include "lwip/udp.h"
 #include "lwip/igmp.h"
 #include "include/UdpContext.h"
#endif

#ifdef USE_WIFI
// Setup DNS, only used if the ESP has no valid WiFi config
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
#endif

#ifdef USE_WIFI
// MQTT client
WiFiClient mqtt;
PubSubClient MQTTclient(mqtt);

// WebServer
ESP8266WebServer WebServer(80);

// syslog stuff
WiFiUDP portUDP;
#endif

extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;
extern "C" uint32_t _SPIFFS_page;
extern "C" uint32_t _SPIFFS_block;

struct SecurityStruct
{
  char          WifiSSID[32];
  char          WifiKey[64];
  char          WifiSSID2[32];
  char          WifiKey2[64];
  char          WifiAPKey[64];
  char          ControllerUser[CONTROLLER_MAX][26];
  char          ControllerPassword[CONTROLLER_MAX][64];
  char          Password[26];
} SecuritySettings;

struct SettingsStruct
{
  unsigned long PID;
  int           Version;
  int16_t       Build;
  byte          IP[4];
  byte          Gateway[4];
  byte          Subnet[4];
  byte          DNS[4];
  byte          IP_Octet;
  byte          Unit;
  char          Name[26];
  char          NTPHost[64];
  unsigned long Delay;
  int8_t        Pin_i2c_sda;
  int8_t        Pin_i2c_scl;
  int8_t        Pin_status_led;
  int8_t        Pin_sd_cs;
  int8_t        PinBootStates[17];
  byte          Syslog_IP[4];
  unsigned int  UDPPort;
  byte          SyslogLevel;
  byte          SerialLogLevel;
  byte          WebLogLevel;
  byte          SDLogLevel;
  unsigned long BaudRate;
  unsigned long MessageDelay;
  byte          deepSleep;
  boolean       CustomCSS;
  boolean       DST;
  byte          WDI2CAddress;
  boolean       UseRules;
  boolean       UseSerial;
  boolean       UseSSDP;
  boolean       UseNTP;
  unsigned long WireClockStretchLimit;
  boolean       GlobalSync;
  unsigned long ConnectionFailuresThreshold;
  int16_t       TimeZone;
  boolean       MQTTRetainFlag;
  boolean       InitSPI; 
  byte          Protocol[CONTROLLER_MAX];
  byte          Notification[NOTIFICATION_MAX];
  byte          TaskDeviceNumber[TASKS_MAX];
  unsigned int  OLD_TaskDeviceID[TASKS_MAX];
  int8_t        TaskDevicePin1[TASKS_MAX];
  int8_t        TaskDevicePin2[TASKS_MAX];
  int8_t        TaskDevicePin3[TASKS_MAX];
  byte          TaskDevicePort[TASKS_MAX];
  boolean       TaskDevicePin1PullUp[TASKS_MAX];
  int16_t       TaskDevicePluginConfig[TASKS_MAX][PLUGIN_CONFIGVAR_MAX];
  boolean       TaskDevicePin1Inversed[TASKS_MAX];
  float         TaskDevicePluginConfigFloat[TASKS_MAX][PLUGIN_CONFIGFLOATVAR_MAX];
  long          TaskDevicePluginConfigLong[TASKS_MAX][PLUGIN_CONFIGLONGVAR_MAX];
  boolean       OLD_TaskDeviceSendData[TASKS_MAX];
  boolean       TaskDeviceGlobalSync[TASKS_MAX];
  byte          TaskDeviceDataFeed[TASKS_MAX];
  unsigned long TaskDeviceTimer[TASKS_MAX];
  boolean       TaskDeviceEnabled[TASKS_MAX];
  boolean       ControllerEnabled[CONTROLLER_MAX];
  boolean       NotificationEnabled[NOTIFICATION_MAX];
  unsigned int  TaskDeviceID[CONTROLLER_MAX][TASKS_MAX];
  boolean       TaskDeviceSendData[CONTROLLER_MAX][TASKS_MAX];
} Settings;

struct ControllerSettingsStruct
{
  boolean       UseDNS;
  byte          IP[4];
  unsigned int  Port;
  char          HostName[65];
  char          Publish[129];
  char          Subscribe[129];
};

struct NotificationSettingsStruct
{
  char          Server[65];
  unsigned int  Port;
  char          Domain[65];
  char          Sender[65];
  char          Receiver[65];
  char          Subject[129];
  char          Body[513];
  byte          Pin1;
  byte          Pin2;
};

struct ExtraTaskSettingsStruct
{
  byte    TaskIndex;
  char    TaskDeviceName[41];
  char    TaskDeviceFormula[VARS_PER_TASK][41];
  char    TaskDeviceValueNames[VARS_PER_TASK][41];
  long    TaskDevicePluginConfigLong[PLUGIN_EXTRACONFIGVAR_MAX];
  byte    TaskDeviceValueDecimals[VARS_PER_TASK];
  int16_t TaskDevicePluginConfig[PLUGIN_EXTRACONFIGVAR_MAX];
} ExtraTaskSettings;

struct EventStruct
{
  byte Source;
  byte TaskIndex; // index position in TaskSettings array, 0-11
  byte ControllerIndex; // index position in Settings.Controller, 0-3
  byte ProtocolIndex; // index position in protocol array, depending on which controller plugins are loaded.
  byte NotificationIndex; // index position in Settings.Notification, 0-3
  byte NotificationProtocolIndex; // index position in notification array, depending on which controller plugins are loaded.
  byte BaseVarIndex;
  int idx;
  byte sensorType;
  int Par1;
  int Par2;
  int Par3;
  byte OriginTaskIndex;
  String String1;
  String String2;
  byte *Data;
};

struct LogStruct
{
  unsigned long timeStamp;
  char* Message;
} Logging[10];
int logcount = -1;

struct DeviceStruct
{
  byte Number;
  byte Type;
  byte VType;
  byte Ports;
  boolean PullUpOption;
  boolean InverseLogicOption;
  boolean FormulaOption;
  byte ValueCount;
  boolean Custom;
  boolean SendDataOption;
  boolean GlobalSyncOption;
  boolean TimerOption;
  boolean TimerOptional;
  boolean DecimalsOnly;
} Device[DEVICES_MAX + 1]; // 1 more because first device is empty device

struct ProtocolStruct
{
  byte Number;
  boolean usesMQTT;
  boolean usesAccount;
  boolean usesPassword;
  int defaultPort;
  boolean usesTemplate;
  boolean usesID;
} Protocol[CPLUGIN_MAX];

struct NotificationStruct
{
  byte Number;
  boolean usesMessaging;
  byte usesGPIO;
} Notification[NPLUGIN_MAX];

struct NodeStruct
{
  byte ip[4];
  byte age;
  uint16_t build;
  char* nodeName;
  byte nodeType;
} Nodes[UNIT_MAX];

struct systemTimerStruct
{
  unsigned long timer;
  byte plugin;
  byte Par1;
  byte Par2;
  byte Par3;
} systemTimers[SYSTEM_TIMER_MAX];

struct systemCMDTimerStruct
{
  unsigned long timer;
  String action;
} systemCMDTimers[SYSTEM_CMD_TIMER_MAX];

struct pinStatesStruct
{
  byte plugin;
  byte index;
  byte mode;
  uint16_t value;
} pinStates[PINSTATE_TABLE_MAX];

struct RTCStruct
{
  byte ID1;
  byte ID2;
  boolean valid;
  byte factoryResetCounter;
  byte deepSleepState;
  byte rebootCounter;
  byte flashDayCounter;
  unsigned long flashCounter;
} RTC;

#ifdef USE_ESPNOW
uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#endif

#endif
