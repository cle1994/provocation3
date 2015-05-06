#include <Wtv020sd16p.h>
#include <Servo.h> 

#define RESET_PIN  2
#define CLOCK_PIN  3
#define DATA_PIN  4
#define BUSY_PIN  5

#define FSR_PIN 15

const int sampleWindow = 500; // Sample window width in mS (50 mS = 20Hz)
const int loudThreshold = 3000;
unsigned int sample;
int lastPlayed = 0;

Wtv020sd16p soundCtrl(RESET_PIN, CLOCK_PIN, DATA_PIN, BUSY_PIN);
Servo myservo;
int pos = 0; // servo position
 
void setup() 
{
   Serial.begin(9600);
   soundCtrl.reset();
   myservo.attach(9); // servo goes in pin 9
   myservo.write(pos);
   go_to(180);
}

double lastReading;
unsigned long loudStart;

void loop() 
{
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
   
   float fsr = analogRead(FSR_PIN);
 
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
   
   if (volts < 2.9 && lastPlayed == 1 && fsr > 50) {
     // things just got quiet
     Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
     Serial.println("   things just got quiet & i'm being pet   ");
     Serial.print(lastReading); Serial.print(", "); Serial.println(volts);
     Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
     go_to(180);
     play(0);
   } else if (lastReading < 2.9 && volts > 3.0) {
     Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
     Serial.println("   things just got loud   ");
     Serial.print(lastReading); Serial.print(", "); Serial.println(volts);
     Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
     loudStart = millis();
   } else if (volts > 2.9 && (millis() - loudStart > loudThreshold)
              && lastPlayed == 0) {
     // things have been loud for a while
     Serial.println("=======================================");
     Serial.println("   things have been loud for a while   ");
     Serial.print(lastReading); Serial.print(", "); Serial.println(volts);
     Serial.println("=======================================");
     go_to(0);
     play(1);
   }
   
   Serial.print("prev V: "); Serial.print(lastReading);
   Serial.print(", curr V: "); Serial.print(volts);
   Serial.print(", fsr: "); Serial.println(fsr);
   lastReading = volts;
}

void play(int trackNumber) {
  soundCtrl.asyncPlayVoice(trackNumber);
  lastPlayed = trackNumber;
  delay(4000);
}

void go_to(int new_pos) {
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
     delay(15); 
   }
}
