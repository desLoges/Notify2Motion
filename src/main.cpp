//*
//*  Goto the instapush dashboard after signing up: https://instapush.im/dashboard
//*  Click "Apps" tab.
//*  Click "+Add Application" button.
//*  Application Title: GDS
//*  Click "Add Application" button.
//*  It should now show the "Events" tab.
//*  Click "+Add Events".
//*  Event Title *: GDS
//*  Trackers*: status
//*  (This confused me at first but you have to hit your keyboard TAB after typing the tracker name.)
//*  Push Message*: The garage door is {status}
//*  Click "Add Event" button.
//*  Click "Basic Info" tab.
//*  Replace the APPIDHERE with Application ID: 12345a678901b23c45d67ef8 (YOUR ID, NOT THIS ONE!)
//*  and APPSECRETHERE with Application Secret: abc12de3fa4b56c78defa9b01cd2e3f4 (YOUR SECRET, NOT THIS ONE!)
//*  in the spots identified below.
//*
//*  Replace SSIDHERE & PASSWORDHERE with your WiFi SSID and Password.
//*  Install the instapush app on your Android mobile device and login.
//*
//*  Note: Mobile app wouldn't let me log-in like many of the app complaints were
//*  saying in the Google Play Store.  I reset my password through the web
//*  interface and then it let me login with the mobile app.
//*
//*  Cross your fingers and hope for the best, power up your ESP8266 and wait for
//*  the push message in the mobile app.
//*
#include "homesec.h"

ESP8266WiFiMulti WiFiMulti;


void setup() {
  security_status = st_init;
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(PUSH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUSH_PIN), trigger_pushbutton, CHANGE);
  pinMode(SENS_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SENS_PIN), trigger_action, RISING);

  set_signalizeEvent(SIG_MCU_INIT);
  set_signalizeEvent(SIG_ALL_LED_OFF);

  Serial.begin(115200);
  delay(10);

  //if( !set_wifiConnection(WiFiMulti) ){
  //  ESP.restart();
  //}
  security_status = st_idle;
}

void loop() {

  delay(SLEEP_MAIN * 1000); //ESP.deepSleep(SLEEP_MAIN * 1000000);
  set_signalizeEvent(SIG_LED_BUILTIN_BLINK);
}
