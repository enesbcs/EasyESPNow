#ifdef USES_P102
#ifdef ESP32

//#######################################################################################################
//################################# Plugin 102: Analog ESP32 ############################################
//#######################################################################################################

#define PLUGIN_102
#define PLUGIN_ID_102         102
#define PLUGIN_NAME_102       "Analog input - ESP32 Touch/Hall sensor"
#define PLUGIN_VALUENAME1_102 "Analog"

boolean Plugin_102(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_102;
        Device[deviceCount].Type = DEVICE_TYPE_ANALOG;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].TimerOptional = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_102);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_102));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
/*        addHtml(F("<TR><TD>Analog Pin:<TD>"));
        addPinSelect(false, "taskdevicepin1", Settings.TaskDevicePin1[event->TaskIndex]);

        byte mode = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
#define P102_MODE_OPTION 3
        String Options[P102_MODE_OPTION] = {
          F("Touch Sensor"),
          F("Hall Sensor"),
          F("Analog input"),
        };
        addFormSelector(F("Input Source"), F("plugin_102_mode"), P102_MODE_OPTION, Options, NULL, mode);

        addFormCheckBox(F("Digital Output Enabled"), F("plugin_102_bin"), Settings.TaskDevicePluginConfig[event->TaskIndex][1]);
        addFormNumericBox(F("Digital 0 if larger"), F("plugin_102_adc0l"), Settings.TaskDevicePluginConfig[event->TaskIndex][2], -4095, 4095);
        addFormNumericBox(F("Digital 0 if below"), F("plugin_102_adc0b"), Settings.TaskDevicePluginConfig[event->TaskIndex][3], -4095, 4095);
        addFormNote(F("For example: larger than -10 and smaller than 10 = 0"));

        addFormNumericBox(F("Digital 1 if below"), F("plugin_102_adc1b"), Settings.TaskDevicePluginConfig[event->TaskIndex][4], -4095, 4095);
        addFormNumericBox(F("Digital 1 if larger"), F("plugin_102_adc1l"), Settings.TaskDevicePluginConfig[event->TaskIndex][5], -4095, 4095);
        addFormNote(F("For example: smaller than -500 or larger than 500 = 1"));
*/
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
/*        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = getFormItemInt(F("plugin_102_mode"));
        Settings.TaskDevicePluginConfig[event->TaskIndex][1] = isFormItemChecked(F("plugin_102_bin"));

        Settings.TaskDevicePluginConfig[event->TaskIndex][2] = getFormItemInt(F("plugin_102_adc0l"));
        Settings.TaskDevicePluginConfig[event->TaskIndex][3] = getFormItemInt(F("plugin_102_adc0b"));
        Settings.TaskDevicePluginConfig[event->TaskIndex][4] = getFormItemInt(F("plugin_102_adc1b"));
        Settings.TaskDevicePluginConfig[event->TaskIndex][5] = getFormItemInt(F("plugin_102_adc1l"));
*/
        success = true;
        break;
      }

    case PLUGIN_TEN_PER_SECOND:
      {
        if ((Settings.TaskDevicePluginConfig[event->TaskIndex][1]==1) && (Settings.TaskDeviceTimer[event->TaskIndex] == 0)) //Binary sensor
        {
          int16_t value = Plugin_102_getvalue(Settings.TaskDevicePin1[event->TaskIndex],Settings.TaskDevicePluginConfig[event->TaskIndex][0]);
          float newbinvalue = -1;
          if (value > Settings.TaskDevicePluginConfig[event->TaskIndex][2] && value < Settings.TaskDevicePluginConfig[event->TaskIndex][3]) {
            newbinvalue = 0;
          }
          if (value < Settings.TaskDevicePluginConfig[event->TaskIndex][4] || value > Settings.TaskDevicePluginConfig[event->TaskIndex][5]) {
            newbinvalue = 1;
          }
          if ((newbinvalue > -1) && (int(newbinvalue) != int(UserVar[event->BaseVarIndex]))) {
            UserVar[event->BaseVarIndex] = newbinvalue;
            sendData(event);
          }
        }
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        String log = F("Analog32 value: ");

        int16_t value = Plugin_102_getvalue(Settings.TaskDevicePin1[event->TaskIndex],Settings.TaskDevicePluginConfig[event->TaskIndex][0]);

        if (Settings.TaskDevicePluginConfig[event->TaskIndex][1]==1)   //Binary sensor
        {
          if (value > Settings.TaskDevicePluginConfig[event->TaskIndex][2] && value < Settings.TaskDevicePluginConfig[event->TaskIndex][3]) {
            UserVar[event->BaseVarIndex] = 0;
          }
          if (value < Settings.TaskDevicePluginConfig[event->TaskIndex][4] || value > Settings.TaskDevicePluginConfig[event->TaskIndex][5]) {
            UserVar[event->BaseVarIndex] = 1;
          }
        } else {
          UserVar[event->BaseVarIndex] = (float)value;
        }
        log += String(UserVar[event->BaseVarIndex]);
        addLog(LOG_LEVEL_INFO, log);
        success = true;
        break;
      }
  }
  return success;
}

int16_t Plugin_102_getvalue(uint8_t pin, int vmode)
{
  int16_t value = 0;
  switch (vmode)
  {
    case 0:
      value = touchRead(pin);
      break;

    case 1:
      value = hallRead();
      break;
      
    case 2:
      value = analogRead(pin);
      break;

  }
  return value;
}
#endif
#endif // USES_P102
