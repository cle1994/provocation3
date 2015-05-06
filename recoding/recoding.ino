#include <Wtv020sd16p.h>
#include <Servo.h> 

#define RESET_PIN  2
#define CLOCK_PIN  3
#define DATA_PIN  4
#define BUSY_PIN  5

#define FSR_PIN 15
#define SERVO_PIN 9

#define N 10

float loud_voltage = 2.9;
float quiet_voltage = 2.5;
bool is_happy = true; // flag for whether buddy is currently happy or sad

// for calculating voltage
const int sampleWindow = 500; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
double prev_volts[N];
double avg_volts = 0;
double volts = 0;

Wtv020sd16p soundCtrl(RESET_PIN, CLOCK_PIN, DATA_PIN, BUSY_PIN);

Servo myservo;
int pos = 0; // servo position

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   soundCtrl.reset();
   myservo.attach(SERVO_PIN);
   myservo.write(pos);
   go_to(180);
   init_avg();
}

void loop() {
  volts = calculate_voltage();
  avg_volts = update_trailing_avg(volts);
  float fsr = analogRead(FSR_PIN);
  
   Serial.print("avg V: "); Serial.print(avg_volts);
   Serial.print(", curr V: "); Serial.print(volts);
   Serial.print(", fsr: "); Serial.println(fsr);
  
  if (avg_volts > loud_voltage && is_happy) {
     Serial.println("=======================================");
     Serial.println("   too loud! buddy is unhappy :-(  ");
     Serial.println("=======================================");
     curl_up();
     play_unhappy_noises();
     is_happy = false;
  } else if (avg_volts < quiet_voltage && !is_happy && fsr > 50) {
     Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
     Serial.println("   things just got quiet & i'm being pet :-)  ");
     Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
     uncurl();
     play_happy_noises();
     is_happy = true;
  }
}

// SOUND CONTROL ///////////////////////////////////////
void play_happy_noises() {
  soundCtrl.asyncPlayVoice(0);
  delay(4000);
}

void play_unhappy_noises() {
  soundCtrl.asyncPlayVoice(1);
  delay(4000);
}

// VOLTAGE & TRAILING AVERAGE CALCULATIONS //////////////////////////
void init_avg() {
  for (int i = 0; i < N; i++) {
     prev_volts[i] = 0;
  } 
}

double update_trailing_avg(double v) {
  for (int i = N-1; i > 0; i--) {
    prev_volts[i] = prev_volts[i-1];
  }
  prev_volts[0] = v;
  double sum = 0;
  for (int i = 0; i < N; i++) {
    sum += prev_volts[i];
  }
  return sum / N;
}

double calculate_voltage() {
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
   return volts;
}

// SERVO CONTROL /////////////////////////////////
void curl_up() {
  go_to(0);
}
void uncurl() {
  go_to(180); 
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

