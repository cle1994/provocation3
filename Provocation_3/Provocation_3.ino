#include <Wtv020sd16p.h>
#include <Servo.h>
#include "CapacitiveSensor.h"

#define RESET_PIN  2
#define CLOCK_PIN  3
#define DATA_PIN  4
#define BUSY_PIN  5

#define CAPACITIVE_PIN_SENSOR 8
#define CAPACITIVE_PIN_OTHER 6

#define SERVO_PIN 9

// SETTINGS: NUMBERS YOU CAN TWEAK //////////////////////////////////////

// making N smaller will make it more responsive 
#define N 3 // the number of previous loudness values to maintain

float quiet_volts = 1.75; // this or lower means it's quiet
float chatty_volts_min = 1.75; // between chatty_volts_min and chatty_volts_max
float chatty_volts_max = 2.5; // means normal talking loudness
float loud_volts = 2.7; // when it's this loud or above, it means the kid is upset

float petting_level = 1500; // when the capacitive sensor reaches this threshold,
                            // it means Buddy is getting petted

unsigned long purr_limit = 15 * 1000; // only allowed to purr at most once every this often (ms)
unsigned long cry_limit = 15 * 1000;  // only allowed to cry at most once every this often (ms)
unsigned long chat_duration_min = 500; // the minimum amount of time that can count as being chatty

// how we organized the sound files on the SD card
int sd_chat_start = 1;
int sd_chat_end = 23;
int sd_unhappy_start = 24;
int sd_unhappy_end = 54;
int sd_happy_start = 1;
int sd_happy_end = 23;

int sd_sample_length = 700; // a lot of the samples are about this long (ms)
                            // except for the happy/unhappy noises

// END OF SETTINGS //////////////////////////////////////////////////////

// remembering internal state about whether to be purring or chatty or crying
unsigned long last_purred = 0; // the time at which Buddy last purred
unsigned long last_cried = 0;  // the time at which Buddy last cried
unsigned long chat_duration = 0; // how long we have been in chatty levels

unsigned long timestamp = 0; // the timestamp of the current time through the loop
unsigned long prev_timestamp = 0; // the timestamp of the previous time through the loop

// for calculating voltage
const int sampleWindow = 500; // Sample window width in mS
unsigned int sample;
double prev_volts[N];
double avg_volts = 0;
double volts = 0;

// for playing sounds
Wtv020sd16p soundCtrl(RESET_PIN, CLOCK_PIN, DATA_PIN, BUSY_PIN);

// for sensing touch
CapacitiveSensor capacitive_sensor = CapacitiveSensor(6,8); // 10M resistor between pins 6 & 8, pin 8 is sensor pin, add a wire and or foil

// for curling up
Servo myservo;
int pos = 0; // servo position

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   soundCtrl.reset();
   myservo.attach(SERVO_PIN);
   myservo.write(pos);
   go_to(0);
   init_avg();
}

void loop() {
  volts = calculate_voltage();
  avg_volts = update_trailing_avg(volts);
  float touch = capacitive_sensor.capacitiveSensor(30);
  bool being_pet = touch > petting_level;
  prev_timestamp = timestamp;
  timestamp = millis();
  
  Serial.println();
  Serial.print(", avg V: "); Serial.print(avg_volts);
  Serial.print(", touch: "); Serial.print(touch);
  Serial.println();

  // QUIET PURRING HAPPY TIME  
  if (
        //avg_volts <= quiet_volts && // it's quiet
        being_pet && // buddy is currently being pet
        chat_duration < chat_duration_min && // the kid wasn't just chatting
        time_since(last_purred) > purr_limit // Buddy didn't just purr
  ) {
    Serial.println("QUIET PURRING HAPPY TIME");
    uncurl();
    play_happy_noises(); // make purring/happy noises
    last_purred = timestamp; // remember that this is the most recent purr time

  // BRIEF PAUSE IN KID TALKING - CHAT BACK
  } else if (
        avg_volts <= quiet_volts && // it's quiet
        chat_duration > chat_duration_min // the kid has been chatting for a little bit
  ) {
    Serial.println("CHAT BACK");
    play_chatty_noises(chat_duration); // chat back
    chat_duration = 0; // reset chat_duration

  // KID IS TALKING - LISTEN/WAIT
  } else if (
        avg_volts >= chatty_volts_min && avg_volts <= chatty_volts_max // we are in chatty range
  ) {
    Serial.println("LISTEN/WAIT");
    chat_duration += time_since(prev_timestamp); // update chat_duration

  // LOUD UPSET UNHAPPY TIME
  } else if (
        avg_volts >= loud_volts && // it's loud
        time_since(last_cried) > cry_limit // Buddy didn't just cry
  ) {
    Serial.println("LOUD UPSET UNHAPPY TIME");
    curl_up();
    delay(500);
    play_unhappy_noises();
    play_unhappy_noises();
    last_cried = timestamp;
  }
}



// HELPER FUNCTIONS ////////////////////////////////////
unsigned long time_since(unsigned long t) {
  return millis() - t; 
}

// SOUND CONTROL ///////////////////////////////////////
void play_unhappy_noises() {
  play_track_number(random(sd_unhappy_start,sd_unhappy_end + 1));
  delay(4000);
}

void play_happy_noises() {
  play_track_number(random(sd_happy_start,sd_happy_end + 1));
  delay(4000);
}

void play_chatty_noises(unsigned long duration) {
  play_track_number(random(sd_chat_start, sd_chat_end + 1));
  delay(sd_sample_length);       
}

void play_track_number(int track) {
  Serial.print("Playing track number: ");Serial.println(track);
  soundCtrl.playVoice(track);
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
  //go_to(170);
  myservo.write(160);
}
void uncurl() {
  //go_to(0); 
  myservo.write(10);
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

