/********************************************************************************************\
* Get data from Serial Interface
\*********************************************************************************************/
#define INPUT_BUFFER_SIZE          128
#define ESPNOW_BUFFER_SIZE         250

byte SerialInByte;
int SerialInByteCounter = 0;
char InputBuffer_Serial[INPUT_BUFFER_SIZE + 2];
#ifdef USE_ESPNOW
int EspNowInByteCounter = 0;
byte EspNowBuffer_Serial[ESPNOW_BUFFER_SIZE];
#endif

void serial()
{
  byte pres = 0;
  while (Serial.available())
  {
    yield();
    SerialInByte = Serial.read();

    if (isprint(SerialInByte))
    {
      if (SerialInByteCounter < INPUT_BUFFER_SIZE) // add char to string if it still fits
        InputBuffer_Serial[SerialInByteCounter++] = SerialInByte;
    }

#ifdef USE_ESPNOW
  if (Settings.OLD_TaskDeviceID[1] == ESPNOW_SERIAL_GATEWAY) {
    EspNowBuffer_Serial[EspNowInByteCounter++] = SerialInByte;
    pres = isValidP2PStruct(EspNowBuffer_Serial,EspNowInByteCounter);
    switch(pres) {
      case 0: 
       EspNowInByteCounter = 0;
       break;
      case 2:
//       Serial.println("Pkt routed from serial to espnow");
       int err = esp_now_send(broadcastMac, (byte*) &EspNowBuffer_Serial, EspNowInByteCounter);
       EspNowInByteCounter = 0;
       SerialInByteCounter = 0;
       InputBuffer_Serial[0] = 0; // serial data processed, clear buffer
       break;
    }
  }
#endif    
    if ((SerialInByte == '\n') && (SerialInByte>1))
    {
      InputBuffer_Serial[SerialInByteCounter] = 0; // serial data completed
      Serial.write('>');
      Serial.println(InputBuffer_Serial);
      String action = InputBuffer_Serial;
      struct EventStruct TempEvent;
      parseCommandString(&TempEvent, action);
      TempEvent.Source = VALUE_SOURCE_SERIAL;
      if (!PluginCall(PLUGIN_WRITE, &TempEvent, action))
        ExecuteCommand(VALUE_SOURCE_SERIAL, InputBuffer_Serial);
      SerialInByteCounter = 0;
      InputBuffer_Serial[0] = 0; // serial data processed, clear buffer
    }
  }
}
