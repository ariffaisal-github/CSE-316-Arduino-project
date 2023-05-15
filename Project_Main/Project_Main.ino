#include <Servo.h>             //Servo library
 
Servo servo_motor1;        //initialize a servo object for the connected servo  
int LED = 12; // Use the onboard Uno LED
int gateObstaclePin1 = 7;  // This is our input pin
int hasObstacle = HIGH;  // HIGH MEANS NO OBSTACLE
int overFlowStatusPin = 13;
int sevroMotorPin = 9;
int angle = 0;
 
void setup() 
{
  servo_motor1.attach(sevroMotorPin);      // attach the signal pin of servo to pin9 of arduino
  servo_motor1.write(0);
  pinMode(LED, OUTPUT);
  pinMode(gateObstaclePin1, INPUT);
  pinMode(overFlowStatusPin, INPUT);
  digitalWrite(LED, LOW);
  Serial.begin(9600); 
} 
  
void loop() 
{ 
  while(digitalRead(overFlowStatusPin) == HIGH) delay(1000);
  hasObstacle = digitalRead(gateObstaclePin1); //Reads the output of the obstacle sensor from the 7th PIN of the Digital section of the arduino
  if (hasObstacle == LOW) 
  {
    delay(1000);
    //Serial.println("Person at Entry");
    digitalWrite(LED, HIGH);
    openGate();
    digitalWrite(LED, LOW);
  }
  else
  {
    //Serial.println("No person to enters");
    digitalWrite(LED, LOW);
  }
  delay(400);

  
}

void openGate() {
  angle = 0;  
  
  for(angle = 0; angle <= 90; angle += 1)    // command to move from 0 degrees to 180 degrees 
  {                                  
    servo_motor1.write(angle);                 //command to rotate the servo to the specified angle
    delay(15);                       
  } 
 
  delay(3000);
  
  for(angle = 90; angle>=0; angle-=1)     // command to move from 180 degrees to 0 degrees 
  {                                
    servo_motor1.write(angle);
    delay(5);                       
  }

  delay(500);
}
