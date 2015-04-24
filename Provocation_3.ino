#include <Wtv020sd16p.h>

#define RESET_PIN  2
#define CLOCK_PIN  3
#define DATA_PIN  4
#define BUSY_PIN  5

const int sampleWindow = 500; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

Wtv020sd16p soundCtrl(RESET_PIN, CLOCK_PIN, DATA_PIN, BUSY_PIN);
 
void setup() 
{
   Serial.begin(9600);
   soundCtrl.reset();
}

double lastReading; 
 
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
   
   if (lastReading > 1.5 && volts < 0.75)
   {
     Serial.println("===================");
     Serial.println("   Threshold met   ");
     Serial.println("-------------------");
     Serial.println(volts);
     Serial.println("===================");
     soundCtrl.asyncPlayVoice(0);
     delay(2000);
   }
   
   lastReading = volts;
}
