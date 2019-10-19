/****************************************************************************************************************************\
 * Arduino project "ESP Easy" © Copyright www.letscontrolit.com
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You received a copy of the GNU General Public License along with this program in file 'License.txt'.
 *
 * IDE download    : https://www.arduino.cc/en/Main/Software
 * ESP8266 Package : https://github.com/esp8266/Arduino
 *
 * Source Code     : https://github.com/ESP8266nu/ESPEasy
 * Support         : http://www.letscontrolit.com
 * Discussion      : http://www.letscontrolit.com/forum/
 *
 * Additional information about licensing can be found at : http://www.gnu.org/licenses
\*************************************************************************************************************************/

// This file incorporates work covered by the following copyright and permission notice:

/****************************************************************************************************************************\
* Arduino project "Nodo" © Copyright 2010..2015 Paul Tonkes
*
* This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
* You received a copy of the GNU General Public License along with this program in file 'License.txt'.
*
* Voor toelichting op de licentievoorwaarden zie    : http://www.gnu.org/licenses
* Uitgebreide documentatie is te vinden op          : http://www.nodo-domotica.nl
* Compiler voor deze programmacode te downloaden op : http://arduino.cc
\*************************************************************************************************************************/

//   Simple Arduino sketch for ESP module, supporting:
//   =================================================================================
//   Simple switch inputs and direct GPIO output control to drive relais, mosfets, etc
//   Analog input (ESP-7/12 only)
//   Pulse counters
//   Dallas OneWire DS18b20 temperature sensors
//   DHT11/22/12 humidity sensors
//   BMP085 I2C Barometric Pressure sensor
//   PCF8591 4 port Analog to Digital converter (I2C)
//   RFID Wiegand-26 reader
//   MCP23017 I2C IO Expanders
//   BH1750 I2C Luminosity sensor
//   Arduino Pro Mini with IO extender sketch, connected through I2C
//   LCD I2C display 4x20 chars
//   HC-SR04 Ultrasonic distance sensor
//   SI7021 I2C temperature/humidity sensors
//   TSL2561 I2C Luminosity sensor
//   TSOP4838 IR receiver
//   PN532 RFID reader
//   Sharp GP2Y10 dust sensor
//   PCF8574 I2C IO Expanders
//   PCA9685 I2C 16 channel PWM driver
//   OLED I2C display with SSD1306 driver
//   MLX90614 I2C IR temperature sensor
//   ADS1115 I2C ADC
//   INA219 I2C voltage/current sensor
//   BME280 I2C temp/hum/baro sensor
//   MSP5611 I2C temp/baro sensor
//   BMP280 I2C Barometric Pressure sensor
//   SHT1X temperature/humidity sensors
//   Ser2Net server

// ********************************************************************************
//   User specific configuration
// ********************************************************************************

// Set default configuration settings if you want (not mandatory)
// You can always change these during runtime and save to eeprom
// After loading firmware, issue a 'reset' command to load the defaults.

#include "EasyEspNow-Globals.h"

int deviceCount = -1;
int protocolCount = -1;
int notificationCount = -1;

boolean printToWeb = false;
String printWebString = "";
boolean printToWebJSON = false;

float UserVar[VARS_PER_TASK * TASKS_MAX];
unsigned long RulesTimer[RULES_TIMER_MAX];

unsigned long timerSensor[TASKS_MAX];
unsigned long timer;
unsigned long timer100ms;
unsigned long timer20ms;
unsigned long timer1s;
unsigned long timerwd;
unsigned long lastSend;
unsigned int NC_Count = 0;
unsigned int C_Count = 0;
boolean AP_Mode = false;
byte cmd_within_mainloop = 0;
unsigned long connectionFailures;
unsigned long wdcounter = 0;

#if FEATURE_ADC_VCC
float vcc = -1.0;
#endif

boolean WebLoggedIn = false;
int WebLoggedInTimer = 300;

boolean (*Plugin_ptr[PLUGIN_MAX])(byte, struct EventStruct*, String&);
byte Plugin_id[PLUGIN_MAX];

boolean (*CPlugin_ptr[CPLUGIN_MAX])(byte, struct EventStruct*, String&);
byte CPlugin_id[CPLUGIN_MAX];

boolean (*NPlugin_ptr[NPLUGIN_MAX])(byte, struct EventStruct*, String&);
byte NPlugin_id[NPLUGIN_MAX];

String dummyString = "";

boolean systemOK = false;
byte lastBootCause = 0;

#ifdef USE_WIFI
boolean wifiSetup = false;
boolean wifiSetupConnect = false;
#endif
unsigned long start = 0;
unsigned long elapsed = 0;
unsigned long loopCounter = 0;
unsigned long loopCounterLast = 0;
unsigned long loopCounterMax = 1;
unsigned long lastsysinfo = 0;
unsigned long dailyResetCounter = 0;

String eventBuffer = "";

uint16_t lowestRAM = 0;
byte lowestRAMid=0;
/*
1 savetoflash - obsolete
2 loadfrom flash - obsolete
3 zerofillflash - obsolete
4 rulesprocessing
5 handle_download
6 handle_css
7 handlefileupload
8 handle_rules
9 handle_devices
*/

/*********************************************************************************************\
 * SETUP
\*********************************************************************************************/
void setup()
{
  lowestRAM = FreeMem();
  #if defined(ESP32)
    for(byte x = 0; x < 16; x++)
      ledChannelPin[x] = -1;
  #endif
  Serial.begin(115200);
  Serial.print(F("\r\n\r\nDevice MAC: "));
  Serial.println(WiFi.macAddress());
  if (SpiffsSectors() < MIN_SPIFFS_SECTORS)
  {
    Serial.println(F("\nNo (or too small) SPIFFS area..\nSystem Halted\nPlease reflash with 128k SPIFFS minimum!"));
    while (true)
      delay(1);
  }

  fileSystemCheck();
  emergencyReset();
  LoadSettings();
#ifdef USE_WIFI
  if (strcasecmp(SecuritySettings.WifiSSID, "ssid") == 0)
    wifiSetup = true;
#endif
  ExtraTaskSettings.TaskIndex = 255; // make sure this is an unused nr to prevent cache load on boot

  // if different version, eeprom settings structure has changed. Full Reset needed
  // on a fresh ESP module eeprom values are set to 255. Version results into -1 (signed int)
  if (Settings.Version == VERSION && Settings.PID == ESP_PROJECT_PID)
  {
    systemOK = true;
  }
  else
  {
    // Direct Serial is allowed here, since this is only an emergency task.
    Serial.print(F("\nPID:"));
    Serial.println(Settings.PID);
    Serial.print(F("Version:"));
    Serial.println(Settings.Version);
    Serial.println(F("INIT : Incorrect PID or version!"));
    delay(6000);
    ResetFactory();
  }

  if (systemOK)
  {
    if (Settings.UseSerial)
      Serial.begin(Settings.BaudRate);
//  while (! Serial); // DEBUG ONLY!
#ifdef USE_ESPNOW
    InitESPNow();
#endif
    if (Settings.Build != BUILD)
      BuildFixes();

    String log = F("\nINIT : Booting Build nr:");
    log += BUILD;
    addLog(LOG_LEVEL_INFO, log);
    log = F("INIT : Free RAM:");
    log += FreeMem();
    addLog(LOG_LEVEL_INFO, log);

    if (Settings.UseSerial && Settings.SerialLogLevel >= LOG_LEVEL_DEBUG_MORE)
      Serial.setDebugOutput(true);
#ifdef USE_WIFI
    WiFi.persistent(false); // Do not use SDK storage of SSID/WPA parameters
    WifiAPconfig();
    if (!WifiConnect(true,3))
      WifiConnect(false,3);
#endif    
    hardwareInit();
    PluginInit();
    CPluginInit();
    NPluginInit();

#ifdef USE_WIFI
    WebServerInit();

    // setup UDP
    if (Settings.UDPPort != 0)
      portUDP.begin(Settings.UDPPort);

    // Setup MQTT Client
    byte ProtocolIndex = getProtocolIndex(Settings.Protocol[0]);
    if (Protocol[ProtocolIndex].usesMQTT)
      MQTTConnect();

    sendSysInfoUDP(3);
#endif
    log = F("INIT : Boot OK");
    addLog(LOG_LEVEL_INFO, log);

    if (Settings.deepSleep)
    {
      log = F("INIT : Deep sleep enabled");
      addLog(LOG_LEVEL_INFO, log);
    }

    byte bootMode = 0;
    if (readFromRTC())
    {
      readUserVarFromRTC();
      bootMode = RTC.deepSleepState;
      if (bootMode == 1)
        log = F("INIT : Reboot from deepsleep");
      else
        log = F("INIT : Normal boot");
    }
    else
    {
      RTC.factoryResetCounter=0;
      RTC.deepSleepState=0;
      RTC.rebootCounter=0;
      RTC.flashDayCounter=0;
      RTC.flashCounter=0;
      saveToRTC();
 
      // cold boot situation
      if (lastBootCause == 0) // only set this if not set earlier during boot stage.
        lastBootCause = BOOT_CAUSE_COLD_BOOT;
      log = F("INIT : Cold Boot");
    }

    addLog(LOG_LEVEL_INFO, log);
#if defined(ESP32) // quickfix for esp32 unknown boot state
if (Settings.deepSleep){
 bootMode = 1;
}
#endif
    // Setup timers
    if (bootMode == 0)
    {
      for (byte x = 0; x < TASKS_MAX; x++)
        if (Settings.TaskDeviceTimer[x] !=0)
          timerSensor[x] = millis() + 30000 + (x * Settings.MessageDelay);
      
      timer = millis() + 30000; // startup delay 30 sec
    }
    else
    {
      for (byte x = 0; x < TASKS_MAX; x++)
        timerSensor[x] = millis() + 0;
      timer = millis() + 0; // no startup from deepsleep wake up
    }

    timer100ms = millis() + 100; // timer for periodic actions 10 x per/sec
    timer1s = millis() + 1000; // timer for periodic actions once per/sec
    timerwd = millis() + 30000; // timer for watchdog once per 30 sec

#ifdef USE_WIFI
    if (Settings.UseNTP)
      initTime();
#endif
#if FEATURE_ADC_VCC
    vcc = ESP.getVcc() / 1000.0;
#endif

#ifdef USE_WIFI
    // Start DNS, only used if the ESP has no valid WiFi config
    // It will reply with it's own address on all DNS requests
    // (captive portal concept)
    if (wifiSetup)
      dnsServer.start(DNS_PORT, "*", apIP);
#endif
#if defined(USE_ESPNOW) && ALIVE_PERIOD>0
 if ( (Settings.OLD_TaskDeviceID[1] == ESPNOW_SEND_ONLY_ENDPOINT) || (Settings.OLD_TaskDeviceID[1] == ESPNOW_SEND_AND_RECEIVE_ENDPOINT) ) {
    lastsysinfo = millis();
    ESPNOW_sendsysinfo();
 }
#endif
    if (Settings.UseRules)
    {
      String event = F("System#Boot");
      rulesProcessing(event);
    }
    RTC.deepSleepState=0;
    saveToRTC();
  }
  else
  {
    Serial.println(F("Entered Rescue mode!"));
  }
}


/*********************************************************************************************\
 * MAIN LOOP
\*********************************************************************************************/
void loop()
{
  loopCounter++;
#ifdef USE_WIFI
  if (wifiSetupConnect)
  {
    // try to connect for setup wizard
    WifiConnect(true,1);
    wifiSetupConnect = false;
  }
#endif
  if (Settings.UseSerial)
    if (Serial.available())
      if (!PluginCall(PLUGIN_SERIAL_IN, 0, dummyString))
        serial();

  if (systemOK)
  {
    if (millis() > timer20ms)
      run50TimesPerSecond();

    if (millis() > timer100ms)
      run10TimesPerSecond();

    if (millis() > timer1s)
      runOncePerSecond();

    if (millis() > timerwd)
      runEach30Seconds();

    backgroundtasks();

  }
  else
    delay(1);
}


/*********************************************************************************************\
 * Tasks that run 50 times per second
\*********************************************************************************************/

void run50TimesPerSecond()
{
  timer20ms = millis() + 20;
  PluginCall(PLUGIN_FIFTY_PER_SECOND, 0, dummyString);
}

/*********************************************************************************************\
 * Tasks that run 10 times per second
\*********************************************************************************************/
void run10TimesPerSecond()
{
  start = micros();
  timer100ms = millis() + 100;
  PluginCall(PLUGIN_TEN_PER_SECOND, 0, dummyString);
#ifdef USE_WIFI
  checkUDP();
#endif  
  if (Settings.UseRules && eventBuffer.length() > 0)
  {
    rulesProcessing(eventBuffer);
    eventBuffer = "";
  }
  elapsed = micros() - start;
}


/*********************************************************************************************\
 * Tasks each second
\*********************************************************************************************/
void runOncePerSecond()
{
  dailyResetCounter++;
  if (dailyResetCounter > 86400) // 1 day elapsed... //86400
  {
    RTC.flashDayCounter=0;
    saveToRTC();
    dailyResetCounter=0;
    String log = F("SYS  : Reset 24h counters");
    addLog(LOG_LEVEL_INFO, log);
  }
  
  timer1s = millis() + 1000;

  checkSensors();

  if (Settings.ConnectionFailuresThreshold)
    if (connectionFailures > Settings.ConnectionFailuresThreshold)
      delayedReboot(60);

  if (cmd_within_mainloop != 0)
  {
    switch (cmd_within_mainloop)
    {
      case CMD_WIFI_DISCONNECT:
        {
#ifdef USE_WIFI
          WifiDisconnect();
#endif          
          break;
        }
      case CMD_REBOOT:
        {
          reboot();
          break;
        }
    }
    cmd_within_mainloop = 0;
  }

  // clock events
  if (Settings.UseNTP)
    checkTime();

  unsigned long timer = micros();
  PluginCall(PLUGIN_ONCE_A_SECOND, 0, dummyString);

  checkSystemTimers();

  if (Settings.UseRules)
    rulesTimers();

  timer = micros() - timer;

  if (SecuritySettings.Password[0] != 0)
  {
    if (WebLoggedIn)
      WebLoggedInTimer++;
    if (WebLoggedInTimer > 300)
      WebLoggedIn = false;
  }

  // I2C Watchdog feed
  if (Settings.WDI2CAddress != 0)
  {
    Wire.beginTransmission(Settings.WDI2CAddress);
    Wire.write(0xA5);
    Wire.endTransmission();
  }

  if (Settings.SerialLogLevel == 5)
  {
    Serial.print(F("10 ps:"));
    Serial.print(elapsed);
    Serial.print(F(" uS  1 ps:"));
    Serial.println(timer);
  }
}

/*********************************************************************************************\
 * Tasks each 30 seconds
\*********************************************************************************************/
void runEach30Seconds()
{
  wdcounter++;
  timerwd = millis() + 30000;
  char str[60];
  str[0] = 0;
#ifdef USE_ESPNOW
  if (Settings.OLD_TaskDeviceID[1] != ESPNOW_SERIAL_GATEWAY) {
#endif
   sprintf_P(str, PSTR("Uptime %u ConnectFailures %u FreeMem %u"), wdcounter / 2, connectionFailures, FreeMem());
   String log = F("WD   : ");
   log += str;
   addLog(LOG_LEVEL_INFO, log);
#ifdef USE_ESPNOW
  }
#endif
#ifdef USE_WIFI
  sendSysInfoUDP(1);
  refreshNodeList();
  MQTTCheck();
  if (Settings.UseSSDP)
    SSDP_update();
#endif    
#if FEATURE_ADC_VCC
  vcc = ESP.getVcc() / 1000.0;
#endif
  loopCounterLast = loopCounter;
  loopCounter = 0;
  if (loopCounterLast > loopCounterMax)
    loopCounterMax = loopCounterLast;
#ifdef USE_WIFI
  WifiCheck();
#endif  
#if defined(USE_ESPNOW) && ALIVE_PERIOD>0
 if ( (Settings.OLD_TaskDeviceID[1] == ESPNOW_SEND_ONLY_ENDPOINT) || (Settings.OLD_TaskDeviceID[1] == ESPNOW_SEND_AND_RECEIVE_ENDPOINT) ) {
  if ( (millis() - lastsysinfo > ALIVE_PERIOD) || ((millis() - lastsysinfo)<0) ) {
   ESPNOW_sendsysinfo();
   lastsysinfo = millis();
  }
 }
#endif
}


/*********************************************************************************************\
 * Check sensor timers
\*********************************************************************************************/
void checkSensors()
{
  // Check sensors and send data to controller when sensor timer has elapsed
  // If deepsleep, use the single timer
  if (Settings.deepSleep)
  {
    if (millis() > timer)
    {
      timer = millis() + Settings.Delay * 1000;  // todo, does this make sense, we will cold boot later...
      SensorSend();
      deepSleep(Settings.Delay);
    }
  }
  else // use individual timers for tasks
  {
    for (byte x = 0; x < TASKS_MAX; x++)
    {
      if ((Settings.TaskDeviceTimer[x] != 0) && (millis() > timerSensor[x]))
      {
        timerSensor[x] = millis() + Settings.TaskDeviceTimer[x] * 1000;
        if (timerSensor[x] == 0) // small fix if result is 0, else timer will be stopped...
          timerSensor[x] = 1;
        SensorSendTask(x);
      }
    }
  }
  saveUserVarToRTC();
}


/*********************************************************************************************\
 * send all sensordata
\*********************************************************************************************/
void SensorSend()
{
  for (byte x = 0; x < TASKS_MAX; x++)
  {
    SensorSendTask(x);
  }
}


/*********************************************************************************************\
 * send specific sensor task data
\*********************************************************************************************/
void SensorSendTask(byte TaskIndex)
{
  if (Settings.TaskDeviceEnabled[TaskIndex])
  {
    byte varIndex = TaskIndex * VARS_PER_TASK;

    boolean success = false;
    byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[TaskIndex]);
    LoadTaskSettings(TaskIndex);

    struct EventStruct TempEvent;
    TempEvent.TaskIndex = TaskIndex;
    TempEvent.BaseVarIndex = varIndex;
    // TempEvent.idx = Settings.TaskDeviceID[TaskIndex]; todo check
    TempEvent.sensorType = Device[DeviceIndex].VType;

    float preValue[VARS_PER_TASK]; // store values before change, in case we need it in the formula
    for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
      preValue[varNr] = UserVar[varIndex + varNr];

    if(Settings.TaskDeviceDataFeed[TaskIndex] == 0)  // only read local connected sensorsfeeds
      success = PluginCall(PLUGIN_READ, &TempEvent, dummyString);
    else
      success = true;

    if (success)
    {
      for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
      {  
        if (ExtraTaskSettings.TaskDeviceFormula[varNr][0] != 0)
        {
          String spreValue = String(preValue[varNr]);
          String formula = ExtraTaskSettings.TaskDeviceFormula[varNr];
          float value = UserVar[varIndex + varNr];
          float result = 0;
          String svalue = String(value);
          formula.replace(F("%pvalue%"), spreValue);
          formula.replace(F("%value%"), svalue);
          byte error = Calculate(formula.c_str(), &result);
          if (error == 0)
            UserVar[varIndex + varNr] = result;
        }
      }
      sendData(&TempEvent);
    }
  }
}


/*********************************************************************************************\
 * set global system timer
\*********************************************************************************************/
boolean setSystemTimer(unsigned long timer, byte plugin, byte Par1, byte Par2, byte Par3)
{
  // plugin number and par1 form a unique key that can be used to restart a timer
  // first check if a timer is not already running for this request
  boolean reUse = false;
  for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
    if (systemTimers[x].timer != 0)
    {
      if ((systemTimers[x].plugin == plugin) && (systemTimers[x].Par1 == Par1))
      {
        systemTimers[x].timer = millis() + timer;
        reUse = true;
        break;
      }
    }

  if (!reUse)
  {
    // find a new free timer slot...
    for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
      if (systemTimers[x].timer == 0)
      {
        systemTimers[x].timer = millis() + timer;
        systemTimers[x].plugin = plugin;
        systemTimers[x].Par1 = Par1;
        systemTimers[x].Par2 = Par2;
        systemTimers[x].Par3 = Par3;
        break;
      }
  }
}


/*********************************************************************************************\
 * set global system command timer
\*********************************************************************************************/
boolean setSystemCMDTimer(unsigned long timer, String& action)
{
  for (byte x = 0; x < SYSTEM_CMD_TIMER_MAX; x++)
    if (systemCMDTimers[x].timer == 0)
    {
      systemCMDTimers[x].timer = millis() + timer;
      systemCMDTimers[x].action = action;
      break;
    }
}


/*********************************************************************************************\
 * check global system timers
\*********************************************************************************************/
boolean checkSystemTimers()
{
  for (byte x = 0; x < SYSTEM_TIMER_MAX; x++)
    if (systemTimers[x].timer != 0)
    {
      if (timeOut(systemTimers[x].timer))
      {
        struct EventStruct TempEvent;
        TempEvent.Par1 = systemTimers[x].Par1;
        TempEvent.Par2 = systemTimers[x].Par2;
        TempEvent.Par3 = systemTimers[x].Par3;
        for (byte y = 0; y < PLUGIN_MAX; y++)
          if (Plugin_id[y] == systemTimers[x].plugin)
            Plugin_ptr[y](PLUGIN_TIMER_IN, &TempEvent, dummyString);
        systemTimers[x].timer = 0;
      }
    }

  for (byte x = 0; x < SYSTEM_CMD_TIMER_MAX; x++)
    if (systemCMDTimers[x].timer != 0)
      if (timeOut(systemCMDTimers[x].timer))
      {
        struct EventStruct TempEvent;
        parseCommandString(&TempEvent, systemCMDTimers[x].action);
        if (!PluginCall(PLUGIN_WRITE, &TempEvent, systemCMDTimers[x].action))
          ExecuteCommand(VALUE_SOURCE_SYSTEM, systemCMDTimers[x].action.c_str());
        systemCMDTimers[x].timer = 0;
        systemCMDTimers[x].action = "";
      }
}


/*********************************************************************************************\
 * run background tasks
\*********************************************************************************************/
void backgroundtasks()
{
  // process DNS, only used if the ESP has no valid WiFi config
#ifdef USE_WIFI
  if (wifiSetup)
    dnsServer.processNextRequest();
  WebServer.handleClient();
  MQTTclient.loop();
  checkUDP();
#endif
  statusLED(false);
  yield();
}
