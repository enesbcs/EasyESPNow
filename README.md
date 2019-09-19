# EasyESPNow
Heavily modified ESPEasy which serves as an ESPNow sensor for RPIEasy receiver.

Generic settings can be modified in EasyEspNow-Globals.h file before compilation or it can be controlled with serial commands. Data sending works using ESPNow protocol.
Only tested on ESP8266 with Arduino Core for ESP8266 v2.5.2.

## EasyEspNow-Globals.h settings

```
// This is the default name, set it if you wish (max 24 char)
#define DEFAULT_NAME        "espnowdevice"

// This is the default (send) delay time, used for DeepSleep (DeepSleep is disabled by default)
#define DEFAULT_DELAY             60

// This is the default unit number of the node
#define UNIT                     254

// Default destination node number (0=broadcast to every unit, set it to the RPIEasy node number for correct working)
#define DEFAULT_DESTINATION_NODE   0

// ESPNow WiFi Channel, has to be the same on ALL nodes! US:1-11, Other places on the world:1-13
#define WIFI_CHANNEL 1

// Period in milliseconds for sending alive signals to all node (default is 5minute=300.000ms)
#define ALIVE_PERIOD 300000

// Role setting possible values: (not used in ESP32 espnow)
// ESP_NOW_ROLE_CONTROLLER : only for sending (can be handy for deepsleep use-cases)
// ESP_NOW_ROLE_SLAVE      : only for receiving
// ESP_NOW_ROLE_COMBO      : capable of sending and receiving (DEFAULT!!!)  
#define ESPNOW_ROLE ESP_NOW_ROLE_COMBO


// Sketch working mode possible values:
// ESPNOW_SERIAL_GATEWAY             : serial to ESPNow bridge mode (no direct data sending or receiving)
// ESPNOW_SEND_ONLY_ENDPOINT         : only for sending (can be handy for deepsleep use-cases)
// ESPNOW_RECEIVE_ONLY_ENDPOINT      : only for receiving
// ESPNOW_SEND_AND_RECEIVE_ENDPOINT  : capable of sending and receiving (DEFAULT!!!)

#define ESPNOW_PURPOSE   ESPNOW_SEND_AND_RECEIVE_ENDPOINT
```

All the settings above can be managed by either by RPIEasy webGUI under http://IPADDRESS/espnow or by serial commands.


## Important serial command list:

Change unit number to 3:
> unit,3

Scan for I2C devices:
> i2cscanner

Delete Task 1:
> taskclear,1

Reboot device:
> reboot

Save settings:
> save

Show settings:
> settings

Get currently used wifi channel number:
> espnow,chan

Set wifi channel to 2:
> espnow,chan,2

Enable DeepSleep interval and set delay to 180 seconds:
> espnow,deepsleep,180

Get current working mode number:
> espnow,mode

Set working mode to serial Gateway mode:
> espnow,mode,0

Set working mode to Send only mode:
> espnow,mode,1

Set working mode to Send&Receive mode:
> espnow,mode,3

Set destination node address to 1:
> espnow,dest,1

Set unit name to "remote":
> espnow,name,remote

Force unit to send an alive package immediately to everyone:
> espnow,sendinfo
