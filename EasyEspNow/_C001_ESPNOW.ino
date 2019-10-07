#ifdef USE_ESPNOW

//#######################################################################################################
//########################### Controller Plugin 001: ESPNOW ######################################
//#######################################################################################################

#define CPLUGIN_001
#define CPLUGIN_ID_001         1
#define CPLUGIN_NAME_001       "ESPNOW"

#define ESPNOW_VER 11

char c_payload[250]; // ESPNow package max size is 250bytes

struct P2P_SysInfoStruct
{
  // header field
  byte header = 255;
  byte ID = 1;
  byte sourcelUnit;
  byte destUnit=0; // default destination is to broadcast
  // start of payload
  byte mac[6];
  uint16_t build = BUILD;
  byte nodeType = NODE_TYPE_ID;
  byte cap;
  byte NameLength; // length of the following char array to send
  char Name[25];  
};

struct P2P_SensorDataStruct_Raw
{
  // header field
  byte header = 255; 
  byte ID = 5;
  byte sourcelUnit;
  byte destUnit;
  // start of payload
  uint16_t plugin_id; 
  uint16_t idx;
  byte samplesetcount=0;
  byte valuecount;  
  float Values[VARS_PER_TASK];  // use original float data
};

struct P2P_SensorDataStruct_TTN
{
  // header field
  byte header = 255; 
  byte ID = 6;
  byte sourcelUnit;
  byte destUnit;
  // start of payload
  uint16_t plugin_id; 
  uint16_t idx;
  byte samplesetcount=0;
  byte valuecount;  
  byte data[8];    // implement base16Encode thing if you like
};

struct P2P_CommandDataStruct
{
  // header field
  byte header = 255;
  byte ID = 7;
  byte sourcelUnit;
  byte destUnit;
  // start of payload
  byte CommandLength; // length of the following char array to send 
  char CommandLine[80];
};

struct P2P_TextDataStruct
{
  // header field
  byte header = 255;
  byte ID = 8;
  byte sourcelUnit;
  byte destUnit;
  // start of payload
  byte TextLength; // length of the following char array to send 
  char TextLine[80];
};

// Init ESP Now
void InitESPNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() == 0) {
    Serial.println("ESPNow Init Success");
  }
  else {
    delay(1000);
    Serial.println("ESPNow Init Failed");
    delay(5000);
    reboot();
  }
#if defined(ESP8266)
  esp_now_set_self_role(Settings.OLD_TaskDeviceID[0]); // OLD_TaskDeviceID[0] = ROLE!!!
  esp_now_add_peer(broadcastMac, ESP_NOW_ROLE_COMBO, Settings.UDPPort, NULL, 0);   // Settings.UDPPort = WIFI_CHANNEL
#elif defined(ESP32)  
  esp_now_peer_info_t slave;
  memcpy(slave.peer_addr, broadcastMac, sizeof(broadcastMac) );
  slave.channel = Settings.UDPPort;
  slave.ifidx = ESP_IF_WIFI_STA;
  slave.encrypt = 0;
  esp_err_t status = esp_now_add_peer(&slave);
#endif  
  esp_now_register_recv_cb(ESPNOW_receiver); // Attach _C001_ESPNOW.ino receiver
}

boolean CPlugin_001(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case CPLUGIN_PROTOCOL_ADD:
      {
        Protocol[++protocolCount].Number = CPLUGIN_ID_001;
        Protocol[protocolCount].usesMQTT = false;
        Protocol[protocolCount].usesTemplate = false;
        Protocol[protocolCount].usesAccount = false;
        Protocol[protocolCount].usesPassword = false;
        Protocol[protocolCount].defaultPort = WIFI_CHANNEL;
        Protocol[protocolCount].usesID = true;
        break;
      }

    case CPLUGIN_GET_DEVICENAME:
      {
        string = F(CPLUGIN_NAME_001);
        break;
      }

    case CPLUGIN_PROTOCOL_RECV:
      {
        if (event->Data[0]==255) {
/*          Serial.println("PKTOK");
          Serial.println(event->Par1); // DEBUG*/
          
          if (event->Data[1]==1) { // infopacket
            struct P2P_SysInfoStruct dataReply;          
            memcpy(&dataReply, event->Data, event->Par1);
            if (Settings.OLD_TaskDeviceID[1] == ESPNOW_SERIAL_GATEWAY) { // write to serial in bridge mode
              Serial.flush();
//              Serial.write((byte*)&dataReply,event->Par1);
              Serial.write(event->Data,event->Par1);              
            }
            break;
          }
          
          if (event->Data[1]==7) { // commandpacket
            struct P2P_CommandDataStruct dataReply;          
            memcpy(&dataReply, event->Data, event->Par1);
            if ( (byte(dataReply.destUnit) != byte(Settings.Unit)) and (byte(dataReply.destUnit) != 0) ) { // exit if not broadcasted and we are not the destination
              break;
            }
            if (Settings.OLD_TaskDeviceID[1] == ESPNOW_SERIAL_GATEWAY) { // write to serial in bridge mode
              Serial.flush();              
              Serial.write((byte*)&dataReply,event->Par1);
            }
            dataReply.CommandLine[dataReply.CommandLength] = 0; // make it zero terminated
            String request = (char*)dataReply.CommandLine;
            
            if ( (request.indexOf(",mode")>0) || (Settings.OLD_TaskDeviceID[1] != ESPNOW_SERIAL_GATEWAY) ) // if not GW try to execute commands
            {           
             struct EventStruct TempEvent;
             parseCommandString(&TempEvent, request);
             TempEvent.Source = VALUE_SOURCE_ESPNOW;
             if (!PluginCall(PLUGIN_WRITE, &TempEvent, request))            // call plugins
              ExecuteCommand(VALUE_SOURCE_ESPNOW, dataReply.CommandLine);     // or execute system command
            } 
          }    
                
          if (event->Data[1]==5) { // data comm receiving is not supported, only routing to SERIAL!!!
            struct P2P_SensorDataStruct_Raw dataReply;          
            memcpy(&dataReply, event->Data, event->Par1);
            if ( (byte(dataReply.destUnit) != byte(Settings.Unit)) && byte(dataReply.destUnit) != 0) { // exit if not broadcasted and we are not the destination
              break;
            }
            if (Settings.OLD_TaskDeviceID[1] != ESPNOW_SERIAL_GATEWAY) {
              Serial.println(F("Receiving not supported!"));
              break;
            }
            Serial.flush();
            Serial.write((byte*)&dataReply,event->Par1);
          }
                    
          if (event->Data[1]==8) { // data comm receiving is not supported, only routing to SERIAL!!!
            struct P2P_TextDataStruct dataReply;          
            memcpy(&dataReply, event->Data, event->Par1);
            if ( (byte(dataReply.destUnit) != byte(Settings.Unit)) && byte(dataReply.destUnit) != 0) { // exit if not broadcasted and we are not the destination
              break;
            }
            if (Settings.OLD_TaskDeviceID[1] != ESPNOW_SERIAL_GATEWAY) {
              Serial.println(F("Receiving not supported!"));
              break;
            }
            Serial.flush();
            Serial.write((byte*)&dataReply,event->Par1);
          }
        
        }
        
        break;
      }

    case CPLUGIN_PROTOCOL_SEND:
      {
        if ((event->idx != 0) && ((Settings.OLD_TaskDeviceID[1] == ESPNOW_SEND_ONLY_ENDPOINT) || (Settings.OLD_TaskDeviceID[1] == ESPNOW_SEND_AND_RECEIVE_ENDPOINT)))             
        {
          struct P2P_SensorDataStruct_Raw dataReply;

          dataReply.sourcelUnit = byte(Settings.Unit);
          dataReply.destUnit = byte(Settings.OLD_TaskDeviceID[2]);

          dataReply.plugin_id = (word)(Settings.TaskDeviceNumber[event->TaskIndex]);
          dataReply.idx = (word)(event->idx);
          dataReply.valuecount = byte(getValueCountFromSensorType(event->sensorType));
  
          for (byte x = 0; x < dataReply.valuecount; x++)
            dataReply.Values[x] = (float)UserVar[event->BaseVarIndex + x];
  
          byte psize = sizeof(P2P_SensorDataStruct_Raw);
          if (dataReply.valuecount<4) { // do not send zeroes
            psize = psize - ((4-dataReply.valuecount)*sizeof(float));
          }
/*          Serial.print(dataReply.idx);
          Serial.print(",");  
          Serial.println(dataReply.Values[0]);
          Serial.print(",");  
          Serial.println(psize); // DEBUG

           for (byte x = 0; x < psize; x++) {
            Serial.println();
            Serial.print(((byte*)&dataReply)[x]);
            Serial.println();            
          }*/

          esp_now_send(broadcastMac, (byte*) &dataReply, psize);
        }
        break;
      }

  }
  return success;
}

#if defined(ESP8266)
void ESPNOW_receiver(unsigned char* macaddr, unsigned char* data, unsigned char data_len) // reroute incoming data
#endif
#if defined(ESP32)
void ESPNOW_receiver(const uint8_t* macaddr, const uint8_t* data, int data_len) // reroute incoming data
#endif
{
/*  Serial.print("recv_cb "); // debug
  Serial.println(data_len); // debug */
  if (data_len<1) {
    return;
  }
  memcpy(&c_payload,data,data_len);
  c_payload[data_len] = 0;
//  Serial.println(c_payload[0]); 
  if (c_payload[0] == 255) { // valid data arrived
#if defined(ESP8266)
   if (!esp_now_is_peer_exist(macaddr))
   {
    esp_now_add_peer(macaddr, ESP_NOW_ROLE_COMBO, Settings.UDPPort, NULL, 0);
   }
#endif   
   struct EventStruct TempEvent;
   TempEvent.Data = (byte*)&c_payload;         // Payload
   TempEvent.Par1    = data_len;               // data length
   TempEvent.String1 = String((char*)macaddr); // Source
   TempEvent.String2 = "";                     // Destination
   byte ProtocolIndex = getProtocolIndex(Settings.Protocol[0]);
   CPlugin_ptr[ProtocolIndex](CPLUGIN_PROTOCOL_RECV, &TempEvent, dummyString);
  }
}

boolean ESPNOW_sendsysinfo() {
  if (Settings.OLD_TaskDeviceID[1] != ESPNOW_SERIAL_GATEWAY) {Serial.println("Sending info packet: ");}
  struct P2P_SysInfoStruct dataReply;

  byte mac[6];
  WiFi.macAddress(mac);    
  memcpy(dataReply.mac, mac, 6);
  memset(dataReply.Name,0,24);
  strncpy(dataReply.Name, Settings.Name,24);
  dataReply.NameLength = strlen(Settings.Name);

  dataReply.sourcelUnit = byte(Settings.Unit);
  dataReply.build = byte(BUILD);
  dataReply.nodeType = byte(NODE_TYPE_ID);
  dataReply.cap = getCapability();
  
/*  Serial.print(dataReply.sourcelUnit);
  Serial.print(",");
  Serial.println(dataReply.Name); */
  // send packet  
  byte psize = sizeof(dataReply);
  if (dataReply.NameLength<24) { // do not send zeroes
    psize = psize-(24-dataReply.NameLength);
  }
/*  Serial.print(psize);
  Serial.print(",");
  Serial.println(dataReply.NameLength);
  Serial.print(",");  
  Serial.println(psize); // DEBUG*/
  
  delay(random(10,200));
  esp_now_send(broadcastMac, (byte*) &dataReply, psize);
}

boolean ESPNOW_sendreply(String line) {
  if (Settings.OLD_TaskDeviceID[1] != ESPNOW_SERIAL_GATEWAY) {Serial.println("Sending reply");}
  struct P2P_TextDataStruct dataReply;

  dataReply.TextLength = byte(line.length());
  dataReply.TextLine[dataReply.TextLength] = 0;
  strcpy(dataReply.TextLine, line.c_str());  
  dataReply.sourcelUnit = byte(Settings.Unit);
  dataReply.destUnit = byte(Settings.OLD_TaskDeviceID[2]);

  // send packet  
  byte psize = sizeof(P2P_TextDataStruct);
  if (dataReply.TextLength<80) { // do not send zeroes
    psize = psize-(80-dataReply.TextLength)+1;
  }
/*  Serial.println(dataReply.TextLine);
  Serial.println(dataReply.TextLength);
  Serial.println(psize);  */

  esp_now_send(broadcastMac, (byte*) &dataReply, psize);
}

boolean ESPNOW_sendcmd(const char *line) {
  if (Settings.OLD_TaskDeviceID[1] != ESPNOW_SERIAL_GATEWAY) {Serial.println("Sending cmd");}
  struct P2P_CommandDataStruct dataReply;

  dataReply.CommandLength = strlen(line);
  strcpy(dataReply.CommandLine, line);

  dataReply.sourcelUnit = byte(Settings.Unit);
  dataReply.destUnit = byte(Settings.OLD_TaskDeviceID[2]);

  // send packet  
  byte psize = sizeof(P2P_CommandDataStruct);
  if (dataReply.CommandLength<80) { // do not send zeroes
    psize = psize-(80-dataReply.CommandLength);
  }
  esp_now_send(broadcastMac, (byte*) &dataReply, psize);
}

boolean ESPNOW_commands(byte source, const char *Line) {
  char TmpStr1[80];
  TmpStr1[0] = 0;
  char Command[80];
  Command[0] = 0;
  int Par1 = 0;
  int Par2 = 0;
  int Par3 = 0;
  boolean success = false;

  GetArgv(Line, Command, 2);
  if (GetArgv(Line, TmpStr1, 3)) Par1 = str2int(TmpStr1);
  if (GetArgv(Line, TmpStr1, 4)) Par2 = str2int(TmpStr1);
  if (GetArgv(Line, TmpStr1, 5)) Par3 = str2int(TmpStr1);

  if (strcasecmp_P(Command, PSTR("ver")) == 0)  // get Version
  {
    success = true;
    SendStatus(source, String(ESPNOW_VER)); //    Serial.println(ESPNOW_VER);
  }
  if (strcasecmp_P(Command, PSTR("chan")) == 0) // get-set Wifi Channel
  {
    success = true;
    if (Par1) {
      Settings.UDPPort = Par1;
    }
    SendStatus(source, String(Settings.UDPPort));
  }
  if (strcasecmp_P(Command, PSTR("deepsleep")) == 0) // get-set DeepSleep setting
  {
    success = true;
    GetArgv(Line, TmpStr1, 3);    
    if (strlen(TmpStr1)>0) {
      if (Par1>0) {
        Settings.Delay = Par1;
        Settings.deepSleep = 1;
      } else {
        Settings.deepSleep = 0; // if Par1 is zero, disable deep-sleep
      }
    }
    if (Settings.deepSleep<1) {
      Par1=0;
    } else {
      Par1=Settings.Delay;
    }
    SendStatus(source, String(Par1));
  }
  if (strcasecmp_P(Command, PSTR("mode")) == 0) // get-set Endpoint work mode
  {
    success = true;
    GetArgv(Line, TmpStr1, 3);    
    if (strlen(TmpStr1)>0) {
      switch (Par1)
      {
        case ESPNOW_SERIAL_GATEWAY:
           Settings.OLD_TaskDeviceID[0] = ESP_NOW_ROLE_COMBO;
           Settings.OLD_TaskDeviceID[1] = ESPNOW_SERIAL_GATEWAY;
          break;
        case ESPNOW_SEND_ONLY_ENDPOINT:
           Settings.OLD_TaskDeviceID[0] = ESP_NOW_ROLE_CONTROLLER;
           Settings.OLD_TaskDeviceID[1] = ESPNOW_SEND_ONLY_ENDPOINT;
          break;
        case ESPNOW_RECEIVE_ONLY_ENDPOINT:
           Settings.OLD_TaskDeviceID[0] = ESP_NOW_ROLE_SLAVE;
           Settings.OLD_TaskDeviceID[1] = ESPNOW_RECEIVE_ONLY_ENDPOINT;
          break;
        default:
           Settings.OLD_TaskDeviceID[0] = ESP_NOW_ROLE_COMBO;
           Settings.OLD_TaskDeviceID[1] = ESPNOW_SEND_AND_RECEIVE_ENDPOINT;
          break;
      }
    }
    SendStatus(source, String(Settings.OLD_TaskDeviceID[1]));
  }
  if (strcasecmp_P(Command, PSTR("dest")) == 0) // get-set DestinationNode setting
  {
    success = true;
    GetArgv(Line, TmpStr1, 3);    
    if (strlen(TmpStr1)>0) {
      Settings.OLD_TaskDeviceID[2] = Par1;
    }
    SendStatus(source, String(Settings.OLD_TaskDeviceID[2])); // store default Destination
  }
  if (strcasecmp_P(Command, PSTR("name")) == 0) // get-set UnitName setting
  {
    success = true;
    memset(TmpStr1,0,25);
    GetArgv(Line, TmpStr1, 3);
    byte slen = strlen(TmpStr1);
    if (slen>0) {
      if (sizeof(Settings.Name)<slen){
        slen= sizeof(Settings.Name);
      }
      memset(Settings.Name,0,slen);
      memcpy(Settings.Name,TmpStr1,slen);
      Settings.Name[slen]=0;
    }
    SendStatus(source, String(Settings.Name));
  }
  if (strcasecmp_P(Command, PSTR("sendinfo")) == 0) // send Info packet
  {
    success = true;
    ESPNOW_sendsysinfo();
  }
  if (strcasecmp_P(Command, PSTR("sendcmd")) == 0) // send Command packet
  {
    success = true;
    String event = Line;
    ESPNOW_sendcmd(event.substring(15).c_str()); // tasknum?? MISSING!
  }
  if (strcasecmp_P(Command, PSTR("tasklist")) == 0) // get tasklist
  {
    success = true;
    byte tfound = 0;
    for (byte x = 0; x < TASKS_MAX; x++)
    {      
      if (Settings.TaskDeviceNumber[x]>0) {
       SendStatus(source, "Task,"+String(x+1)+","+String(Settings.TaskDeviceNumber[x])+","+String(Settings.TaskDevicePin1[x])+","+String(Settings.TaskDevicePin2[x])+","+String(Settings.TaskDevicePin3[x])+","+String(Settings.TaskDevicePort[x])+","+String(Settings.TaskDeviceTimer[x])+","+String(Settings.TaskDeviceID[0][x]));
       // taskIndex, plugin_id, pin1, pin2, pin3, port, interval, idx
       SendStatus(source, "Conf,"+String(x+1)+","+String(Settings.TaskDevicePin1PullUp[x])+","+String(Settings.TaskDevicePin1Inversed[x])+","+String(Settings.TaskDevicePluginConfig[x][0])+","+String(Settings.TaskDevicePluginConfig[x][1])+","+String(Settings.TaskDevicePluginConfig[x][2])+","+String(Settings.TaskDevicePluginConfig[x][3])+","+String(Settings.TaskDevicePluginConfig[x][4])+","+String(Settings.TaskDevicePluginConfig[x][5])+","+String(Settings.TaskDevicePluginConfig[x][6])+","+String(Settings.TaskDevicePluginConfig[x][7]));      
       // taskIndex, pullup, inverted, taskdevicepluginconfig[0],[1],[2],[3],[4],[5],[6],[7]
       tfound++;
      }
    }
    if (tfound<1) {
      SendStatus(source, F("No task found"));
    }
  }  
  if (strcasecmp_P(Command, PSTR("taskconf")) == 0) // modify task config
  {
    success = true;     // (byte taskIndex, boolean pullup, boolean inv, int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
    if (Par1) {
     int Par4 = 0; int Par5 = 0; int Par6 = 0; int Par7 = 0; int Par8 = 0;
     int Par9 = 0; int Par10 = 0; int Par11 = 0;
     if (GetArgv(Line, TmpStr1, 6)) Par4 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 7)) Par5 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 8)) Par6 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 9)) Par7 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 10)) Par8 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 11)) Par9 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 12)) Par10 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 13)) Par11 = str2int(TmpStr1);               
     taskConf(Par1, Par2, Par3, Par4, Par5, Par6, Par7, Par8, Par9, Par10, Par11);
     Serial.println(F("OK. Task config modified")); // INCOMPLETE!!!
    }
  }
  if (strcasecmp_P(Command, PSTR("taskadd")) == 0) // add a new task
  {
    success = true;   // (byte taskIndex, byte taskdevicenumber, int pin1, int pin2, int pin3, int port, int interval, int idx)
    if (Par1) {
     int Par4 = 0; int Par5 = 0; int Par6 = 0; int Par7 = 0; int Par8 = 0;
     if (GetArgv(Line, TmpStr1, 6)) Par4 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 7)) Par5 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 8)) Par6 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 9)) Par7 = str2int(TmpStr1);
     if (GetArgv(Line, TmpStr1, 10)) Par8 = str2int(TmpStr1);
     taskAdd(Par1, Par2, Par3, Par4, Par5, Par6, Par7, Par8, true);
     Serial.println(F("OK. Task added"));     
    }    
  }

  if (strcasecmp_P(Command, PSTR("setdate")) == 0) { // set time
    success = true;
          int shour;
          int smin;
          int ssec;
          if (GetArgv(Line, TmpStr1, 6)) shour = str2int(TmpStr1);
          if (GetArgv(Line, TmpStr1, 7)) smin = str2int(TmpStr1);
          if (GetArgv(Line, TmpStr1, 8)) ssec = str2int(TmpStr1);
          tm.Year = Par1;
          if ((tm.Year < 2019) || (tm.Year > 3000)) {
            tm.Year = 2019;
          }
          tm.Year = tm.Year - 1970;
          tm.Month = Par2;
          if ((tm.Month < 1) || (tm.Month > 12)) {
            tm.Month = 1;
          }
          tm.Day = Par3;
          if ((tm.Day < 1) || (tm.Day > 31)) {
            tm.Day = 1;
          }
          tm.Hour = shour;
          if ((tm.Hour < 0) || (tm.Hour > 23)) {
            tm.Hour = 0;
          }
          tm.Minute = smin;
          if ((tm.Minute < 0) || (tm.Minute > 59)) {
            tm.Minute = 0;
          }
          tm.Second = ssec;
          if ((tm.Second < 0) || (tm.Second > 59)) {
            tm.Second = 0;
          }
          setTime(makeTime(tm));
          Settings.UseNTP = true;
        }
  

  return success;
}

/********************************************************************************************\
  Modify task settings, BEFORE adding!
  \*********************************************************************************************/
void taskConf(byte taskIndex, boolean pullup, boolean inv, int c0, int c1, int c2, int c3, int c4, int c5, int c6, int c7)
{
  if (taskIndex>0) {
    taskIndex = taskIndex -1;
  }
  Settings.TaskDevicePin1PullUp[taskIndex] = pullup;
  Settings.TaskDevicePin1Inversed[taskIndex] = inv;

  Settings.TaskDevicePluginConfig[taskIndex][0] = c0;
  Settings.TaskDevicePluginConfig[taskIndex][1] = c1;
  Settings.TaskDevicePluginConfig[taskIndex][2] = c2;
  Settings.TaskDevicePluginConfig[taskIndex][3] = c3;
  Settings.TaskDevicePluginConfig[taskIndex][4] = c4;
  Settings.TaskDevicePluginConfig[taskIndex][5] = c5;
  Settings.TaskDevicePluginConfig[taskIndex][6] = c6;
  Settings.TaskDevicePluginConfig[taskIndex][7] = c7;
}

/********************************************************************************************\
  Add new task
  \*********************************************************************************************/
void taskAdd(byte taskIndex, byte plugin_id, int pin1, int pin2, int pin3, int port, int interval, int idx, boolean save)
{
  if (taskIndex>0) {
    taskIndex = taskIndex -1;
  }
  LoadTaskSettings(taskIndex);
  struct EventStruct TempEvent;
  TempEvent.TaskIndex = taskIndex;
  Settings.TaskDeviceNumber[taskIndex] = plugin_id;
  Settings.TaskDeviceDataFeed[taskIndex] = 0;
  Settings.TaskDevicePin1[taskIndex] = pin1;
  Settings.TaskDevicePin2[taskIndex] = pin2;
  Settings.TaskDevicePin3[taskIndex] = pin3;
  Settings.TaskDevicePort[taskIndex] = port;
  Settings.TaskDeviceTimer[taskIndex] = interval;
  Settings.TaskDeviceEnabled[taskIndex] = true;
  if (idx>0){
    Settings.TaskDeviceID[0][taskIndex] = idx;
    Settings.TaskDeviceSendData[0][taskIndex] = true;   
  }
  String deviceName = "";
  byte DeviceIndex = getDeviceIndex(plugin_id);
  Plugin_ptr[DeviceIndex](PLUGIN_GET_DEVICENAME, 0, deviceName);
  deviceName += String(taskIndex);
  deviceName.toCharArray(ExtraTaskSettings.TaskDeviceName,deviceName.length());
  PluginCall(PLUGIN_GET_DEVICEVALUENAMES, &TempEvent, dummyString);
  PluginCall(PLUGIN_INIT, &TempEvent, dummyString);

  if (save)
  {
    SaveTaskSettings(taskIndex);
    SaveSettings();
  }
}

byte getCapability() {
 if (Settings.OLD_TaskDeviceID[0]==ESP_NOW_ROLE_IDLE) {
  return 0;  
 }
 if ( (Settings.OLD_TaskDeviceID[0]==ESP_NOW_ROLE_CONTROLLER) || (Settings.OLD_TaskDeviceID[1]==ESPNOW_SEND_ONLY_ENDPOINT) ){
  return 1;
 }
 if ( Settings.OLD_TaskDeviceID[0]==ESP_NOW_ROLE_SLAVE){
  return 2;
 } 
 if ( Settings.OLD_TaskDeviceID[0]==ESP_NOW_ROLE_COMBO){
  return 3;
 } 
}

byte isValidP2PStruct(byte* pbuffer, int psize) { //0=never,1=unsure,2=OK
  if (psize<1) {
    return 1;
  }
  if (pbuffer[0]!=255) {
    return 0;
  }
  if (psize<2) {
    return 1;
  }
  if ((pbuffer[1]!=1) && (pbuffer[1]!=5) && (pbuffer[1]!=7) && (pbuffer[1]!=8)) {
    return 0;
  }
  if (psize<6) {
    return 1;
  }
 switch (pbuffer[1]) {
  case 1:
    // statements
    if (psize<11) { return 1;}
    if ((byte(pbuffer[9])+10)<=psize) {
      return 2;
    }
    break;
  case 5:
    // statements
    if (psize<14) { return 1;}
    if (((byte(pbuffer[9])*4)+10)<=psize) {
      return 2;
    }
    break;
  case 7:
  case 8:  
    // statements
    if ((byte(pbuffer[4])+5)<=psize) {
      return 2;
    }
    break;
 }
 return 1;
}


uint32_t makeTime(const timeStruct &tm) {
  // assemble time elements into uint32_t
  // note year argument is offset from 1970 (see macros in time.h to convert to other formats)
  // previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
  const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int i;
  uint32_t seconds;

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds= tm.Year*(SECS_PER_DAY * 365);
  for (i = 0; i < tm.Year; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }

  // add days for this year, months start from 1
  for (i = 1; i < tm.Month; i++) {
    if ( (i == 2) && LEAP_YEAR(tm.Year)) {
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
    }
  }
  seconds+= (tm.Day-1) * SECS_PER_DAY;
  seconds+= tm.Hour * SECS_PER_HOUR;
  seconds+= tm.Minute * SECS_PER_MIN;
  seconds+= tm.Second;
  return (uint32_t)seconds;
}

#endif
