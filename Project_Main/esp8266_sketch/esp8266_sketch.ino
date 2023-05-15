/**
 * to burn 8266----
   (TX)8266<---->(TX)Arduino
   (RX)8266<---->(RX)Arduino
   (EN)8266<---->3.3v
   (GIO0)8266<--->GND
   (GIO1)8266<--->NOT CONNECTED
   (GND)8266<--->GND
   (3.3v)8266<--->3.3v(connect few seconds before uploading)
   (RST)8266<--->NOT CONNECTED

   (RESET)Arduino<-------->GND/**IMPORTANT**/
   
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

#define _SSID "CSE-511"
#define _PASSWORD "1stCSE@BUET"
#define _IP_ADDRESS  "172.20.58.65" //to find ip address goto command prompt and type "arp -a"
ESP8266WiFiMulti WiFiMulti;

void connectToWifi(String ssid, String password){
   for (uint8_t t = 4; t > 0; t--) {
    //Serial.printf("[SETUP] WAIT %d...\n", t);
    //Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  if(WiFiMulti.addAP(ssid.c_str(), password.c_str())){
    digitalWrite(LED_BUILTIN, LOW);  
  }
}

void makeGetRequest(String rfid){
  
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
    //Serial.print("[HTTP] begin...\n");
    //Serial.flush();
    if (http.begin(client, "http://"+_IP_ADDRESS+":3000/auth/"+rfid)) {  // HTTP
      //Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        //Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          payload.trim();
          Serial.println("name->"+payload);
          Serial.flush();
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        Serial.flush();
      }
      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
     Serial.flush(); 
    }
 }
}

void setup() {

  Serial.begin(9600);
  // Serial.setDebugOutput(true);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  connectToWifi(_SSID,_PASSWORD);
}

void loop() {
  // wait for WiFi connection
  //arduino will send rfid to rx port of esp8266
    while(Serial.available()>0)Serial.read();
    digitalWrite(LED_BUILTIN, LOW);
    while(!Serial.available()){
      //waiting for rfid code
      ;
      }
      delay(100);
      String code = "";   
      while(Serial.available()>0){
         code = code + (char)Serial.read();
         if(Serial.available()==0)delay(100); //confirming that input has ended
      }
      code.trim();
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.printf("esp8286->%s\n", code.c_str());
      Serial.println("esp8286->OK");
      Serial.flush();
      makeGetRequest(code);
      
      
    //makeGetRequest("1234");
    delay(1000);
}
