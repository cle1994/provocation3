// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created  
int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo.write(pos);
  pinMode(13, OUTPUT);
} 

void loop() {
  Serial.println("about to go to 0");
  digitalWrite(13, LOW);
  myservo.write(10);
  delay(2000);

  Serial.println("about to go to 170");
  digitalWrite(13, HIGH);
  myservo.write(170);
  delay(5000);
 
}

/*
void go_to(int new_pos) {
  Serial.print("pos:");Serial.println(pos);
  if (pos == new_pos) {
     return;
   }
   int incr = 0;
   if (pos < new_pos) {
     incr = 1;
   } else {
     incr = -1;
   }
   for (pos = pos; pos != new_pos; pos = pos + incr) {
     myservo.write(pos);
     Serial.print("pos:");Serial.println(pos);
     delay(50); 
   }
}
*/

/* sweep example
void loop() 
{ 
  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }
  delay(1000);
}*/

