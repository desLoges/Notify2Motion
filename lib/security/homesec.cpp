#include "homesec.h"

statuses security_status;

//connect to WiFi
bool set_wifiConnection(ESP8266WiFiMulti wifi_c){
  bool ret = false;

  wifi_c.addAP(WIFI_SSID, WIFI_PASS);

  Serial.print("\nWaiting for WiFi");

  uint8_t retry=0;
  while((wifi_c.run() != WL_CONNECTED) && (retry <= WIFI_CONNECT_RETRY_LIMIT)) {
    Serial.print(".");
    LED_BUILTIN_OFF;
    delay(500);
    LED_BUILTIN_ON;
    retry++;
    delay(50);
  }
  LED_BUILTIN_OFF;

  if (retry >= WIFI_CONNECT_RETRY_LIMIT){
    //Wifi connection failed
    Serial.println("\nWiFi connecion Failed!");
    set_signalizeEvent(SIG_WIFI_CONNECT_ERROR);
    ret = false;
  }else{
    //Wifi connection success
    Serial.println("\nWiFi connected!");
    Serial.print("Assigned IP address: ");
    Serial.println(WiFi.localIP());
    set_signalizeEvent(SIG_WIFI_CONNECT_SUCCESS);
    ret = true;
  }
  return ret;
}

//call function
bool send_callAlarm(void) {
  bool ret = false;
  // Make a TCP connection to remote host
  WiFiClient client;
  if (client.connect(THINGSPEAK_SERVER_URL, 80))
  {

    //should look like this...
    //api.thingspeak.com/apps/thinghttp/send_request?api_key={api key}&number={send to number}&message={text body}
    client.print("GET /apps/thinghttp/send_request?api_key=");
    client.print(THINGSPEAK_API_KEY);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(THINGSPEAK_SERVER_URL);
    client.println("Connection: close");
    client.println();

    Serial.println("call sent");
    ret = true;
  }
  else
  {
    Serial.println(F("Connection failed"));
    ret = false;
  }

  // Check for a response from the server, and route it
  // out the serial port.
  while (client.connected())
  {
    if ( client.available() )
    {
      char c = client.read();
      Serial.print(c);
    }
  }
  Serial.println();
  client.stop();

  return ret;
}

bool get_internetAvailable(void){
  bool ret = false;
  const uint16_t port = 80; // Web port.
  const char * host = "api.instapush.im"; // Instapush API website.

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Connection failed");
    ret = false;
  }else{
    ret = true;
  }
  return ret;
}

//app notification
bool send_applicationAlert(String PushStatus) {
  bool ret = false;
  const uint16_t port = 80; // Web port.
  const char * host = "api.instapush.im"; // Instapush API website.

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Connection failed");
    ret = false;
  }

  // Send a request to the server
  // Make JSON request:
  String json1 = String("{\"event\":\"NodeMCU_test\",\"trackers\":{\"status\":\"");
  String json2 = PushStatus;
  String json3 = String("\"}}");

  // Make a HTTP request:
  client.print("POST /v1/post HTTP/1.1\n");
  client.print("User-Agent: ESP8266\n");
  client.print("Host: api.instapush.im\n");
  client.print("Accept: *\\*\n");
  client.print("x-instapush-appid: 58612690a4c48a25a09430e9\n");
  client.print("x-instapush-appsecret: 6e703628771638804d0c4af47a346ff2\n");
  client.print("Content-Type: application/json\n");
  client.print("Content-Length: ");
  client.print(json1.length() + json2.length() + json3.length());
  client.print("\n\n");
  client.print(json1);
  client.print(json2);
  client.print(json3);
  client.print("\n\n");
  Serial.println("Sent push, closing connection.");
  client.stop();
  ret = true;

  return ret;
}

void trigger_action(void){
  delayMicroseconds(2000);
  Serial.print("Action IRQ ");
}

void trigger_pushbutton(void){
  detachInterrupt(digitalPinToInterrupt(PUSH_PIN));
  delayMicroseconds(3000);
  attachInterrupt(digitalPinToInterrupt(PUSH_PIN), trigger_pushbutton, CHANGE);

  Serial.print("Push button IRQ  ");

  if ( digitalRead(PUSH_PIN) == HIGH ) {
    pushbutton_release();
  } else {
    int longPushStart = millis();
    int longPushElapsed = 0;
    while ( (digitalRead(PUSH_PIN) == LOW) && (longPushElapsed <= LONG_PUSH_LIMIT) ) {
      longPushElapsed = millis() - longPushStart;
      delayMicroseconds(1000);
    }

    if ( longPushElapsed >= LONG_PUSH_LIMIT ) {
      pushbutton_longPress();
    }else{
      pushbutton_shortPress();
    }
  }
}

void pushbutton_longPress(void){
  Serial.println("Push >> long action");
  //state init
  if (security_status == st_init) {
    Serial.println("state: INIT");
  }else{
    //state not init
    if (security_status != st_armed) {
      security_status = st_armed; //if the state is not INIT and not ARMED, then to long press activate ARMED
      Serial.println("state: ARMED");
    }else{
      security_status = st_idle; //if the state is not INIT and in ARMED, then to long press set to IDLE
      Serial.println("state: IDLE");
    }
  }
}

void pushbutton_shortPress(void){
  Serial.println("Push >> short action");
}

void pushbutton_release(void){
  Serial.println("Push >> release action");
}

//event signalizations by LEDS
void set_signalizeEvent(uint8_t eventcode){
  switch (eventcode) {
    case SIG_ALL_LED_OFF:
    LED_RED_OFF;
    LED_BUILTIN_OFF;
    break;

    case SIG_ALL_LED_ON:
    LED_RED_ON;
    LED_BUILTIN_ON;
    break;

    case SIG_WIFI_CONNECT_ERROR:
    //red - .. -
    LED_RED_ON;
    delay(2000);
    LED_RED_OFF;
    delay(200);
    LED_RED_ON;
    delay(200);
    LED_RED_OFF;
    delay(200);
    LED_RED_ON;
    delay(200);
    LED_RED_OFF;
    delay(200);
    LED_RED_ON;
    delay(2000);
    LED_RED_OFF;
    break;
    case SIG_WIFI_CONNECT_SUCCESS:
    //blue - .. -
    LED_BUILTIN_ON;
    delay(2000);
    LED_BUILTIN_OFF;
    delay(100);
    LED_BUILTIN_ON;
    delay(100);
    LED_BUILTIN_OFF;
    delay(100);
    LED_BUILTIN_ON;
    delay(100);
    LED_BUILTIN_OFF;
    delay(100);
    LED_BUILTIN_ON;
    delay(2000);
    LED_BUILTIN_OFF;
    break;

    case SIG_MCU_INIT:
    //blue ...
    LED_BUILTIN_ON;
    delay(100);
    LED_BUILTIN_OFF;
    delay(100);
    LED_BUILTIN_ON;
    delay(100);
    LED_BUILTIN_OFF;
    delay(100);
    LED_BUILTIN_ON;
    delay(100);
    LED_BUILTIN_OFF;

    break;

    case SIG_LED_BUILTIN_BLINK:
    LED_BUILTIN_ON;
    delay(100);
    LED_BUILTIN_OFF;
    break;

    case SIG_LED_RED_BLINK:
    LED_RED_ON;
    delay(100);
    LED_RED_OFF;
    break;

    default:
    break;
  }
}
