
//#include <ESP8266WiFi.h>
#include "ESP8266WiFiMulti.h"

#include "../credentials.h"

#define LED_RED   5
#define SENS_PIN  14
#define PUSH_PIN  12

#define SIG_ALL_LED_OFF           0
#define SIG_ALL_LED_ON            1
#define SIG_MCU_INIT              2
#define SIG_WIFI_CONNECT_ERROR    3
#define SIG_WIFI_CONNECT_SUCCESS  4
#define SIG_LED_BUILTIN_BLINK     5
#define SIG_LED_RED_BLINK         6

#define WIFI_CONNECT_RETRY_LIMIT 12   //asdas
#define LONG_PUSH_LIMIT          2000 //ms
#define SLEEP_MAIN               5 //s

#define LED_BUILTIN_ON (digitalWrite(LED_BUILTIN, LOW))
#define LED_BUILTIN_OFF (digitalWrite(LED_BUILTIN, HIGH))
#define LED_RED_ON (digitalWrite(LED_RED, HIGH))
#define LED_RED_OFF (digitalWrite(LED_RED, LOW))

enum statuses { st_init,st_idle,st_armed,st_triggered,alarm };
extern statuses security_status;

//EEPROM
bool set_eepromSettings(void);

bool get_eepromSettings(void);

//WIFI
bool set_wifiConnection(ESP8266WiFiMulti wifi_c);

bool get_wifiConnection(void);

//Internet
bool get_internetAvailable(void);

//Application
bool send_applicationAlert(String PushStatus);

//call
bool send_callAlarm(void);

//signalizations
void set_signalizeEvent(uint8_t eventcode);

//callbacks
void trigger_action(void);
void trigger_pushbutton(void);

//button
void pushbutton_longPress(void);
void pushbutton_shortPress(void);
void pushbutton_release(void);
