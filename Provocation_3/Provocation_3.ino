#include <Wtv020sd16p.h>
#include <Servo.h> 

#define RESET_PIN  2
#define CLOCK_PIN  3
#define DATA_PIN  4
#define BUSY_PIN  5

#define FSR_PIN 15
#define SERVO_PIN 9

#define N 15

float loud_voltage = 2.9;
float quiet_voltage = 2.5;
bool is_happy = true; // flag for whether buddy is currently happy or sad
unsigned long started_happy_at_time = 0; // record millis() when it started being happy
unsigned long be_happy_for_at_least = 10000; // can't go to sad state until at least this
                                  // much time has been spent in happy state

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
   started_happy_at_time = millis();
}

void loop() {
  volts = calculate_voltage();
  avg_volts = update_trailing_avg(volts);
  float fsr = analogRead(FSR_PIN);
  
  Serial.print("avg V: "); Serial.print(avg_volts);
  Serial.print(", curr V: "); Serial.print(volts);
  Serial.print(", fsr: "); Serial.print(fsr);
  
  unsigned long current_time = millis();
  unsigned long time_elapsed = current_time - started_happy_at_time;
  Serial.print(", t: "); Serial.print(current_time);
  Serial.print(", happy start t: "); Serial.print(started_happy_at_time);
  Serial.print(", dt: "); Serial.println(time_elapsed);
  
  if (avg_volts > loud_voltage && is_happy
     && time_elapsed > be_happy_for_at_least ) {
     Serial.println("=======================================");
     Serial.println("   too loud! buddy is unhappy :-(  ");
     Serial.println("=======================================");
     curl_up();
     play_unhappy_noises();
     is_happy = false;
  } else if (avg_volts < quiet_voltage && !is_happy && fsr > 50) {
     Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
     Serial.println("   things just got quiet & i'm being pet :-)  ");
     Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
     uncurl();
     play_happy_noises();
     is_happy = true;
     started_happy_at_time = millis();
     init_avg();
  }
}

// SOUND CONTROL ///////////////////////////////////////
void play_happy_noises() {
  play_track_number(random(0,3));
}

void play_unhappy_noises() {
  play_track_number(random(3,6));
}

void play_track_number(int track) {
  Serial.print("Playing track number: ");Serial.println(track);
  soundCtrl.playVoice(track);
  delay(5000);
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
      if (sample < 950)  // toss out spurious readings
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

