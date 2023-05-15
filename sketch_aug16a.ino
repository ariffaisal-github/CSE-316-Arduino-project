#include<SoftwareSerial.h>
SoftwareSerial rfid(2,3);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  rfid.begin(9600);
  rfid.setTimeout(2000);
  pinMode(7,INPUT);
}

void loop() {
  String person_name = auth("1234");
  if(person_name!="NOT FOUND")Serial.println(person_name);  
  delay(10000);
}


String auth(String code){
  int pass  = 3;
  String response;
  while(pass--){
    response = "";
    while(rfid.available()>0)rfid.read();
    while(digitalRead(7)!=LOW);
  
    rfid.println(code);
    rfid.listen();
    delay(100);
    while(rfid.available()<=0);
    delay(1000); 
    String t = "esp8286->"+code+"\n";
    String ack = rfid.readStringUntil(t.c_str());
    //Serial.println(ack);
    if(ack.indexOf("OK")>=0)
    {
      response = ack;
      response.trim();
      int i = response.indexOf("name->");
      response = response.substring(i+6);
      return response;
   }
  }
  return response;
}
