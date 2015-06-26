// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.

#include <Servo.h> 
 
Servo myservo;

void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
}

// it looks like at speed 100 for 500 ms it rotates about 90 degrees
// it looks like 3000ms at speed 100 goes the full length of the arms
void loop() {

  myservo.write(80);
  delay(2500);
  myservo.write(90);
  delay(5000);

  myservo.write(100);
  delay(2500);
  myservo.write(90);
  delay(5000);
}
