// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.

#include <Servo.h> 
 
Servo myservo;

void setup() 
{ 
  //Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo.write(10);
  pinMode(13, OUTPUT);
} 

void loop() {
  digitalWrite(13, HIGH);
  myservo.write(10);
  delay(1000);
  digitalWrite(13, LOW);
  myservo.write(170);
  delay(1000);
}
