#include <SoftwareSerial.h>

#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

#include <Servo.h>
int LED2 = 4;
int LED3 = 5;
int WIFIENABLE = 8;
int MAX_COUNT = 3;

SoftwareSerial wifiModule(10,11);

//For IR Sensors
int gateObstaclePin2 = 2;
int gateObstaclePin3 = 3;
int hasObstacle2 = HIGH; 
int hasObstacle3 = HIGH;
SoftwareSerial wifi(10,11);
int ALARM_LED = 35;
int RESET_ALARM = 36;

//For LCD
const int rs = 22, en = 23, d4 = 24, d5 = 25, d6 = 26, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//For RFID Reader
const int ss = 53, rst = 6;
MFRC522 mfrc522(ss, rst);

//servomotrs
Servo servo2;
Servo servo3;


int personCount;
bool gateThreeOpened;
bool gateTwoOpened;
bool personEntered;
bool personExited;


volatile int counter;
int UNO_INT_PIN = 18; 
int overFlow = 12;

void overFlowCheck() {
  counter+=1;
  if(counter >= MAX_COUNT) digitalWrite(overFlow, HIGH);
}


void setup() {
  counter = 0;
  gateThreeOpened = false;
  gateTwoOpened = false;
  personEntered = false;
  personExited = false;
  
  lcd.begin(16, 2);
  lcd.print("Welcome");
  wifiModule.begin(9600);
  wifiModule.setTimeout(2000);

  servo2.attach(14);
  servo3.attach(15);
  servo2.write(90);
  servo3.write(90);

  pinMode(WIFIENABLE, INPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(gateObstaclePin2, INPUT);
  pinMode(gateObstaclePin3, INPUT);
  pinMode(ALARM_LED, OUTPUT);
  pinMode(RESET_ALARM, INPUT);

  pinMode(overFlow, OUTPUT);
  digitalWrite(overFlow, LOW);
  attachInterrupt(digitalPinToInterrupt(UNO_INT_PIN), overFlowCheck, RISING);
  
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(4);
  
}

String RFIDreader() {
  String rfidCard = "";
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return "";
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return "";
  }

  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     rfidCard.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  /*lcd.print(rfidCard);
  openGate2();
  openGate3();*/
  return rfidCard;
}

bool ir2() {
  hasObstacle2 = digitalRead(gateObstaclePin2);
  if (hasObstacle2 == LOW)
  {
    Serial.println("2 Stop something is ahead!!");
    digitalWrite(LED2, HIGH);
    return true;
  }
  else
  {
    Serial.println("2 Path is clear");
    digitalWrite(LED2, LOW);
    return false;
  }
  return false;
}

bool ir3() {
  hasObstacle3 = digitalRead(gateObstaclePin3);
  if (hasObstacle3 == LOW)
  {
    Serial.println("3 Stop something is ahead!!");
    digitalWrite(LED3, HIGH);
    return true;
  }
  else
  {
    Serial.println("3 Path is clear");
    digitalWrite(LED3, LOW);
    return false;
  }
  return false;
}

void openGate2() {
  int angle = 0; 
  bool opened2 = false;
  
  for(angle = 0; angle <= 90; angle += 1)
  {                                  
    servo2.write(angle);
    delay(15);                      
  }
  opened2 = true;

  delay(4000);
  
  if(ir2() && opened2 == true) {
    for(angle = 90; angle>=0; angle-=1)
    {                                
      servo2.write(angle);
      personCount++;
      delay(5);                       
    }
    opened2 = false;
  }
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Next Person");
}


void openGate3() {
  int angle = 0; 
  bool opened3 = false;
  
  for(angle = 0; angle <= 90; angle += 1)
  {                                  
    servo3.write(angle);
    delay(15);                      
  }
  opened3 = true;

  delay(4000);
  
  if(ir3() && opened3 == true) {
    for(angle = 90; angle>=0; angle-=1)
    {                                
      servo3.write(angle);
      personCount++;
      delay(5);                       
    }
    opened3 = false;
  }
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Next Person");
  
}

 
void loop() {
  
  //For LCD
  /*lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);*/
  

  //For RFID Card Reader
  lcd.setCursor(0, 0);
  String rfidCard = "";
  while(rfidCard == "") rfidCard = RFIDreader();
  String gateStatus = auth(rfidCard);
  Serial.println(gateStatus);
  Serial.flush();
  if(gateStatus == "NOT FOUND" || gateStatus == "not found") {
    gateThreeOpened = false;
    personExited = false;

    lcd.clear();
    lcd.print("Invalid Card");
    
    openGate3();
    gateThreeOpened = true;

    if(ir2() == true) {
      delay(300);
      if(ir2() == true) {
        
        digitalWrite(ALARM_LED, HIGH);
        digitalWrite(overFlow, HIGH);
        while(true){
          if(digitalRead(RESET_ALARM) == HIGH) {
            delay(200);
            if(digitalRead(RESET_ALARM) == HIGH){
              digitalWrite(ALARM_LED, LOW);
              digitalWrite(overFlow, LOW);
              digitalWrite(4, LOW);
              
              break;
            }
          }
        }
      }
    }
    
    if(ir3() == true) {
      personExited = true;
    }
  }
  else {
    lcd.print(gateStatus + " Entered");
    gateThreeOpened = false;
    personEntered = false;

    openGate2();
    gateTwoOpened = true;

    if(ir2() == true) {
      /*lcd.setCursor(0, 1);
      lcd.print(personCount);*/
    }
  }

  counter--;
  
  if(counter<MAX_COUNT) digitalWrite(overFlow, LOW);

  rfidCard = "";
  gateStatus = "";

  delay(1000);
  //lcd.clear();

}


String auth(String code){
  int pass  = 4;
  String response;
  while(pass--){
    response = "";
    while(wifiModule.available()>0)wifiModule.read();
    while(digitalRead(WIFIENABLE)!=LOW);
  
    wifiModule.println(code);
    wifiModule.listen();
    delay(100);
    while(wifiModule.available()<=0);
    delay(1000); 
    String t = "esp8286->"+code+"\n";
    String ack = wifiModule.readStringUntil(t.c_str());
    //Serial.println(ack);
    if(ack.indexOf("OK")>=0)
    {
      response = ack;
      response.trim();
      int i = response.indexOf("name->");
      if(i==-1)continue;
      response = response.substring(i+6);
      if(response!="")return response;
   }
  }
  return response;
}
