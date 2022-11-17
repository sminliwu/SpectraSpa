/**
 * SPECTRA CONTROLLER B
 * - distance sensor -> DMX lights
 * - object sensor -> sound cue
 */
#include "auth.h"
#include "dmx.h"
#include <MD_YX5300.h>
#include <HCSR04.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

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

#define IR_SENSOR 34
// pins for MP3 don't need to be defined, use ESP pins 17 and 16 for RX/TX2
//const uint8_t HARDWARE_RX = 16;      // connect to TX of MP3 Player module
//const uint8_t HARDWARE_TX = 17;      // connect to RX of MP3 Player module

// YX5300 mp3 player object + config
MD_YX5300 mp3(MP3Stream);
const uint8_t PLAY_FOLDER = 1;        // tracks are all placed in the folder named '00'

byte triggerPin = 15;
byte echoPin = 14;

//#define DMX_SERIAL_INPUT_PIN    GPIO_NUM_9 // pin for dmx rx
//#define DMX_SERIAL_OUTPUT_PIN   GPIO_NUM_10 // pin for dmx tx

/* ********************
 * SYSTEM STATE/SETTINGS GLOBALS
 */

// IR sensors
bool ir;
bool i_ready = false;

// pressure sensor
unsigned int dist;
bool d_ready = false;

// YX5300 playback state
uint8_t trackNum = 4;
uint8_t vol;           // current audio volume

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pw);
//  while (WiFi.status() != WL_CONNECTED) { Serial.print(F("?")); }
//  PRINT("\nIP address: ", WiFi.localIP());
  setup_ir();
  setup_dist();
  setup_sound();
  setup_lights();
}

void loop() {
  loop_ir();
  loop_dist();
  loop_sound();
  loop_lights();
}
