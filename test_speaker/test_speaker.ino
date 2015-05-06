#include <Wtv020sd16p.h>


#define RESET_PIN  2
#define CLOCK_PIN  3
#define DATA_PIN  4
#define BUSY_PIN  5

Wtv020sd16p soundCtrl(RESET_PIN, CLOCK_PIN, DATA_PIN, BUSY_PIN);
 
void setup() 
{
   Serial.begin(9600);
   soundCtrl.reset();
}


void loop() 
{
  Serial.println("about to play sound");
  soundCtrl.asyncPlayVoice(0);
  delay(4000);
}
