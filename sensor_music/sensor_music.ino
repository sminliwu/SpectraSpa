/* PUTTING IT ALL TOGETHER
 * 
 * Library dependencies: 
 *  - MD_YX5300 
 *  - HRSR04
 *  
 * Function:
 * Plays Spectra Spa music files in response to 
 * various sensors: 
 *  - SR04 distance reading (analog)
 *  - infrared distance reading (digital)
 *  
 * All tracks need to be placed in folder PLAY_FOLDER (defined below)
 * on a micro SD card formatted with FAT32 file system.
 */

#include <HCSR04.h>
#include <MD_YX5300.h>

// track numbers for Spectra Spa music files
#define NUM_TRACKS      8
#define TRK_WELCOME_1     0
#define TRK_WELCOME_2     1
#define TRK_SOUNDBATH_1   2
#define TRK_SOUNDBATH_2   3
#define TRK_GALACTIC      4
#define TRK_LUNAVOCALS    5
#define TRK_AFFIRM        6
#define TRK_JUNGLE        7

// different playback modes
#define MODE_IDLE         0   // waits for commsnds
#define MODE_DRONE        1   // plays tracks end-to-end
#define MODE_SENS_DIGITAL 2   // plays a specific track in response to sensor
#define MODE_SENS_ANALOG  3   // plays something depending on an analog signal

// Making serial communications more human-readable
#define Console Serial      // aliasing serial communication w/ IDE 
#define MP3Stream Serial2   // aliasing serial communication w/ YX5300 module

// Debug switch for debugging output - set to non-zero to enable
#define DEBUG 1

#ifdef DEBUG
#define PRINT(s,v)    { Console.print(F(s)); Console.println(v); }
#define PRINTX(s,v)   { Console.print(F(s)); Console.println(v, HEX); }
#define PRINTS(s)     { Console.println(F(s)); }
#else
#define PRINT(s,v)    
#define PRINTX(s,v)   
#define PRINTS(s)     
#endif


/* ********************
 * ESP config: pins, global libraries
 */

// HRS04 pins
const byte triggerPin = 15;
const byte echoPin = 13;

// digital sensor pins
#define NUM_DIGI_SENSORS  7
#define SENSOR_PIN_ARRAY {34, 35, 32, 33, 25, 26, 27}
const uint8_t* sensorPins = new uint8_t[NUM_DIGI_SENSORS] SENSOR_PIN_ARRAY;

// pins for MP3 don't need to be defined, use ESP pins 17 and 16 for RX/TX2
//const uint8_t HARDWARE_RX = 16;      // connect to TX of MP3 Player module
//const uint8_t HARDWARE_TX = 17;      // connect to RX of MP3 Player module

// YX5300 mp3 player object + config
MD_YX5300 mp3(MP3Stream);
const uint8_t PLAY_FOLDER = 1;        // tracks are all placed in the folder named '00'

// lights control w/ DMX/RS-485 will need two pins for serial RX/TX

// built-in LED for status
const uint8_t PIN_LED = LED_BUILTIN; 


/* ********************
 * SYSTEM STATE/SETTINGS
 */

// SR04 settings
#define NUM_AVERAGE 40
#define SENS_ANALOG_DELAY 5  // millis between sensor readings

// digital sensors
#define SENS_DIGITAL_DELAY 5
bool* prevSensorReadings = new bool[NUM_DIGI_SENSORS] {true};
byte prevSensorIndex = 0;

// YX5300 playback state
#define SWITCH_INTERVAL 5000  // minimum time for switching tracks
unsigned long prevSwitchMillis; // the most recent track switch
uint8_t playMode = MODE_SENS_DIGITAL;

bool playing = false;  // true if player is currently playing a track
bool interruptable = false;   // true if the player will switch tracks while in the middle of playing
uint8_t vol;           // current audio volume

void setup() {
  Console.begin(57600);
  pinMode(PIN_LED, OUTPUT);

  // set up the infrared sensors
  for (byte i=0; i < NUM_DIGI_SENSORS; i++) {
    pinMode(sensorPins[i], INPUT);
    SYNC_LED_FLASH(100);
  }
  // set up the distance sensor
  HCSR04.begin(triggerPin, echoPin);
  // set up mp3 communications
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);
  mp3.begin();

  mp3.setSynchronous(true);
  vol = mp3.volumeMax(); // max seems to be 30, and higher values does not affect it
  PRINT("\nSetting volume to max: ", vol);
  bool b = mp3.volume(vol);
  PRINT(" result ", b);
}

void loop() {
  mp3.check();        // run the mp3 receiver
  processSerial();    // check for any serial input

  // proceed with playback mode
  switch (playMode) {
    case MODE_IDLE:
      break;
    case MODE_DRONE:
      break;
    case MODE_SENS_DIGITAL:
      playDigitalSensors();
      delay(100);
      break;
    case MODE_SENS_ANALOG:
      break;
    default:
      break;
  }
}

void processSerial() {
  if (Serial.available()) {
    char inChar = (char) Serial.read();
    if (inChar == 'p') {
      Serial.flush();
      togglePlayerState(); 
    } else {
      char* payload = new char[3];
      Serial.readBytesUntil('\n', payload, 3);
      switch (inChar) {
        case 't':      
          processTrackNumber(payload);
          break;
        case 'v':
//          updateVolume(payload);
          break;
        case 'm':
//          updateMode(payload);
          break;
        default:
          break;
      }
    }
  }
}

void playDigitalSensors() {
//  PRINTS("reading IR sensors");
  // read each IR sensor
  bool* sensorReadings = new bool[NUM_DIGI_SENSORS];
  uint8_t switched = 255;
  for (byte i=0; i < NUM_DIGI_SENSORS; i++) {
    sensorReadings[i] = digitalRead(sensorPins[i]);
    if (sensorReadings[i] != prevSensorReadings[i]) {
      prevSensorReadings[i] = sensorReadings[i];
      switched = i;
    }
  }

  if (switched != 255) {
    PRINT("sensor ", switched);
    PRINT("sensor is ", sensorReadings[switched]);
    if (millis() - prevSwitchMillis >= SWITCH_INTERVAL) {
      mp3.playTrack(switched);
      prevSwitchMillis = millis();
      PRINT("playing track ", switched);
    } else {
      PRINTS("too soon to switch");
    }
  }
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

void processTrackNumber(char* payload) {
  uint8_t trackNum = (uint8_t) payload[0] - '0';
  PRINT("track number ", trackNum);
  mp3.playTrack(trackNum);
  return;
}

void SYNC_LED_FLASH(unsigned int t) {
  digitalWrite(PIN_LED, LOW);
  delay(100);
  digitalWrite(PIN_LED, HIGH);
  delay(t);
  digitalWrite(PIN_LED, LOW);
}
