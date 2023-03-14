#include "ESP8266WiFi.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include<EEPROM.h>
#include "time.h"

#include "config.h"
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

#define RX_PIN 5 // RX GPIO-Pin of your Microcontroller
#define TX_PIN 4 // TX GPIO-Pin of your Microcontroller 
#define BUZZER_PIN 14
#define BUTTON_PIN 12

uint32_t lastTimeItHappened = millis() + papercheck_milliseconds; 
const int LONG_PRESS_TIME  = 3000; // 1000 milliseconds

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 28800;   //Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 0;  //Replace with your daylight offset (seconds)

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

WiFiClientSecure client;
PubSubClient mqtt(client);

SoftwareSerial mySerial(RX_PIN, TX_PIN, true); 
Adafruit_Thermal printer(&mySerial);


void(* resetFunc) (void) = 0; 
  
void blink(unsigned int times,unsigned int interval){
  for (int i=0;i<times;i++) {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(interval);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(interval);  
  }
}

void buzz(unsigned int times,unsigned int buzzTime,unsigned int interval){
    for (int i=0;i<times;i++) {
      digitalWrite(BUZZER_PIN,HIGH) ;
      delay(buzzTime); 
      digitalWrite(BUZZER_PIN, LOW);   
      delay(interval);  // wait for a second
    }
}
void buttonEvent(){
   currentState = digitalRead(BUTTON_PIN);

  if(lastState == HIGH && currentState == LOW)        // button is pressed
    pressedTime = millis();
  else if(lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if (pressedTime != 0){
      if( pressDuration > LONG_PRESS_TIME){
        WiFiManager wifiManager;
        wifiManager.resetSettings();
        buzz(1,70,50);
        delay(500);
        resetFunc();
      }else{
        printer.println(F("WiFi connected to "));
        printer.println(WiFi.SSID());// Print the IP address;
        printer.println(F("long press to reset the wifi setting, use your phone to connect to Quick Order AP to connect the device to your Wifi"));// Print the IP address;
        printer.feed(3);
      }
    }

  } 

  // save the the last state
  lastState = currentState;
}
void callback(char* topic, byte* payload, unsigned int length) {
 String orderDetail = "";
// topic to print text
// printer.setCharset(CHARSET_CHINA);
// printer.setCodePage(CODEPAGE_CP850);
//  
    printer.wake();
    orderDetail.concat(F("\n"));
    orderDetail.concat(F("Order Detail\n"));
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    orderDetail.concat(asctime(timeinfo));
    orderDetail.concat(F("\n"));
 if (strcmp(topic,mqtt_listen_topic_text2print)==0){
    for (int i=0;i<length;i++) {      
      orderDetail.concat((char)payload[i]);
    }
    printer.print(orderDetail);
//    printer.print(F("\n"));
    printer.feed(3);
    buzz(2,300,50);
    blink(3,500);
    printer.sleep();
 }

} 
void setup() {
  // put your setup code here, to run once:
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  mySerial.begin(baud);
  client.setInsecure();

  // WiFiManager
  WiFiManager wifiManager;

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("Quick Order AP");
  
//  printer.wake();
  printer.begin();
  printer.setDefault();
  
  printer.setSize(mqtt_text_size);
  printer.setLineHeight(mqtt_row_spacing);
  unsigned long dotPrintTime = 1*30L;
  unsigned long dotFeedTime = 1*30L;
  printer.setTimes(dotPrintTime, dotFeedTime);
  printer.sleep();
//  printer.setHeatConfig(11,360,40);
//  printer.feed(1);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // Connect to WiFi
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);
}

void loop() {

  buttonEvent();
  // put your main code here, to run repeatedly:

//  if (WiFi.status() != WL_CONNECTED) {
////    printer.println(F("Connectin                                                           g to WiFi..."));
//    WiFi.begin(ssid, password);
//
//    unsigned long begin_started = millis();
//    while (WiFi.status() != WL_CONNECTED) {
//      delay(10);
//      if (millis() - begin_started > 60000) {
//        ESP.restart();
//      }
//    }
////    printer.println(F("WiFi connected!"));
////    printer.println("The IP Address of ESP8266 Module is: ");
////    printer.println(WiFi.localIP());// Print the IP address;
//    blink(4,250);
//  }


  
  if (!mqtt.connected()) {
    if (mqtt.connect(mqtt_id, mqtt_user, mqtt_pass)) {
      Serial.println(F("MQTT connected"));
//      printer.feed(1);
      mqtt.subscribe(mqtt_listen_topic_text2print);
      blink(2,250);
    } else {
//      Serial.println(F("MQTT connection failed"));
      Serial.print("failed, rc=");
      Serial.println(mqtt.state()); // I'm getting -1
//      printer.feed(1);
      delay(2000);
      return;
    }
  }

  //check the paperload
//  if ((millis() - lastTimeItHappened >= papercheck_milliseconds)){
//    bool bPaperCheck = printer.hasPaper();
//    delay(100);
//    if (!bPaperCheck) {
//      buzz(1,2000,50);
//    } 
//    lastTimeItHappened = millis();
//  }

  mqtt.loop();
}
