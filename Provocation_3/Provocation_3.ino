#include <Wtv020sd16p.h>

#define RESET_PIN  2
#define CLOCK_PIN  3
#define DATA_PIN  4
#define BUSY_PIN  5

const int sampleWindow = 500; // Sample window width in mS (50 mS = 20Hz)
const int loudThreshold = 4000;
unsigned int sample;

Wtv020sd16p soundCtrl(RESET_PIN, CLOCK_PIN, DATA_PIN, BUSY_PIN);
 
void setup() 
{
   Serial.begin(9600);
   soundCtrl.reset();
}

double lastReading;
unsigned long loudStart;
 
void loop() 
{
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
 
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
   
   if (lastReading > 1.5 && volts < 0.75) {
     // things just got quiet
     Serial.println("===================");
     Serial.println("   Threshold met   ");
     Serial.println("-------------------");
     Serial.println(volts);
     Serial.println("===================");
     soundCtrl.asyncPlayVoice(0);
     delay(4000);
   } else if (lastReading < 0.75 && volts > 1.5) {
     // things just got loud
     loudStart = millis();
   } else if (lastReading > 1.5 && (millis() - loudStart > loudThreshold)) {
     // things have been loud for a while
     Serial.println("===================");
     Serial.println("   Threshold met   ");
     Serial.println("-------------------");
     Serial.println(volts);
     Serial.println("===================");
     soundCtrl.asyncPlayVoice(1);
     delay(4000);
   }
   
   lastReading = volts;
}
