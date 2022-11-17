/* 
 * Library dependencies: 
 *  - MD_YX5300 
 *  - HRSR04
 *  
 * Music source: https://archive.org/details/SynthesizedPianoNotes/
 *  
 * Function:
 *  
 * All tracks need to be placed in folder PLAY_FOLDER (defined below)
 * on a micro SD card formatted with FAT32 file system.
 * 
 * Just play the piano notes in order and display distance sensor readings
 * to check I can get both working in parallel
 */

#include <HCSR04.h>
#include <MD_YX5300.h>


// Making serial communications more human-readable
#define Console Serial      // aliasing serial communication w/ IDE 
#define MP3Stream Serial2   // aliasing serial communication w/ YX5300 module

// Debug switch for debugging output - set to non-zero to enable
#define DEBUG 1

#ifdef DEBUG
#define PRINT(s,v)    { Console.print(F(s)); Console.print(v); }
#define PRINTX(s,v)   { Console.print(F(s)); Console.print(v, HEX); }
#define PRINTS(s)     { Console.print(F(s)); }
#else
#define PRINT(s,v)    
#define PRINTX(s,v)   
#define PRINTS(s)     
#endif


// ESP pins config
// HRS04 pins
const byte triggerPin = 15;
const byte echoPin = 13;

// pins for MP3 don't need to be defined, use ESP pins 17 and 16 for RX/TX2
//const uint8_t HARDWARE_RX = 16;      // connect to TX of MP3 Player module
//const uint8_t HARDWARE_TX = 17;      // connect to RX of MP3 Player module

// other pins
const uint8_t PIN_LED = LED_BUILTIN;  // LED to show status
const uint8_t PIN_SWITCH = 33;        // play/pause toggle digital pin, active low (PULLUP)


// YX5300 mp3 player object + settings
MD_YX5300 mp3(MP3Stream);
const uint8_t PLAY_FOLDER = 1;        // tracks are all placed in the folder named '00'

/*
 * PLAYER CONTROLS (via serial)
 *   'p' - toggles player pause
 *   '.' - speeds up (switchSpeed -1)
 *   '>' - speeds up more (switchSpeed -10)
 *   ',' - slows down (switchSpeed +1)
 *   '<' - slows down more (switchSpeed +10)
 *   't##' - ## is number 1-25, plays that track num
 */
#define PLAYER_TOGGLE 'p'
#define DECREMENT_SWITCH_SPEED '.'
#define DECREASE_SWITCH_SPEED '>'
#define INCREMENT_SWITCH_SPEED ','
#define INCREASE_SWITCH_SPEED '<'

#define MAX_SWITCH_SPEED 2000

// HRS04 settings
#define NUM_AVERAGE 20
#define SENSOR_DELAY 5

// system state
// HRS04 distance sensing state
unsigned long prevSensorMillis;
double* distanceReadings = new double[NUM_AVERAGE] {0};
uint8_t avgCounter = 0;

// YX5300 playback state
unsigned long prevSwitchMillis;
// how fast to cycle between files: wait time in millis
// the piano sample notes are 2 seconds, so max 2000ms
unsigned long switchSpeed = MAX_SWITCH_SPEED; 

bool playing = false;  // true if player is currently paused
uint8_t vol;           // current audio volume

void setup() {
  Console.begin(57600);
  PRINTS("\n[MD_YX5300 Switching Notes Player]\n");

  // set the hardware pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SWITCH, INPUT_PULLUP);

  HCSR04.begin(triggerPin, echoPin);

  setStatusLED();   // LED should start off, because player starts paused

  // initialize global libraries
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);

  if (!MP3Stream) { // If the object did not initialize, then its configuration is invalid
    PRINTS("Invalid serial pin configuration, check config"); 
    while (1) { // Don't continue with invalid configuration
      delay (1000);
    }
  } else {
    PRINTS("Serial2 has been set up");
  } 

  // send handshake to MP3 player
  PRINTS("\nBeginning MP3 player...");
  mp3.begin();
  PRINT("\nStatus code: ", mp3.getStsCode());
  
  mp3.setSynchronous(true);
  
  vol = mp3.volumeMax(); // max seems to be 30, and higher values does not affect it
  PRINT("\nSetting volume to max: ", vol);
  bool b = mp3.volume(vol);
  PRINT(" result ", b);

  mp3.playFolderRepeat(PLAY_FOLDER); // this WILL start playing music
  playing = true;
  prevSwitchMillis = millis();
}

void loop() {
  mp3.check();        // run the mp3 receiver
  setStatusLED();     // set the status LED to current status
  processSerial();

  unsigned long currentMillis = millis();

  if (playing && currentMillis - prevSwitchMillis >= switchSpeed) {
    mp3.playNext();
    prevSwitchMillis = currentMillis;
  }

  if (currentMillis - prevSensorMillis >= SENSOR_DELAY) {
    double* distances = HCSR04.measureDistanceCm();
  
    distanceReadings[avgCounter] = distances[0];
  
    double acc = 0;
    for (int i=0; i < NUM_AVERAGE; i++) {
      acc += distanceReadings[i];
    }
    acc = acc / NUM_AVERAGE;
    Console.println(acc);
  
    avgCounter = (avgCounter + 1) % NUM_AVERAGE;
  }
}

void setStatusLED() {
// set the status led - on for running, off for paused
  if (!playing)
    digitalWrite(PIN_LED, LOW);
  else
    digitalWrite(PIN_LED, HIGH);
}

void togglePlayerState() {
  bool b;
  playing = !playing;
  
  PRINT("\nSwitching to ", !playing ? F("PAUSE") : F("PLAY"));
  
  if (!playing) b = mp3.playPause(); else {
    b = mp3.playStart();
    prevSwitchMillis = millis();
  }
  PRINT(" result ", b);
}

void updateSwitchSpeed(char c) {
  if (c == INCREMENT_SWITCH_SPEED) {
    if (switchSpeed < MAX_SWITCH_SPEED){
      switchSpeed++;
      PRINT("\nswitch speed: ", switchSpeed);
    } else {
      PRINTS("switch speed is already max");
    }
  } else if (c == DECREMENT_SWITCH_SPEED) {
    if (switchSpeed > 1) {
      switchSpeed--;
      PRINT("\nswitch speed: ", switchSpeed);
    } else {
      PRINTS("switch speed is at minimum")
    }
  }
}

void processSerial() {
  if (Serial.available()) {
    char inChar = (char) Serial.read();
    if (inChar == 'p') {
      Serial.flush();
      togglePlayerState(); 
    } else if (inChar == 't'){
      processTrackNumber();
    } else { updateSwitchSpeed(inChar); }
  }
}

void processTrackNumber() {
  String inString = "";
  while (Serial.available()) {
    char inChar = (char) Serial.read();
    if (inChar == '\n') {
      uint8_t trackNum = (uint8_t) inString.toInt();
      PRINT("track number ", trackNum);
      mp3.playTrack(trackNum);
      return;
    }
    inString += inChar;
  }
}
