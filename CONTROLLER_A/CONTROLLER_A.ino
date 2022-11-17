/**
 * SPECTRA CONTROLLER A
 * - pressure sensor -> DMX lights
 * - object sensor -> sound cue
 */

#define USE_IR        1
//#define USE_DIST      1
//#define USE_PRESSURE  1
#define USE_SOUND     1
#define USE_LIGHTS    1

// uncomment each part that needs to print to console
#ifdef USE_IR
  #define DEBUG_IR 1
#endif

#ifdef USE_PRESSURE
  #define DEBUG_PRESSURE 1
#endif

#ifdef USE_SOUND
  #define DEBUG_SOUND 1
  #include "MD_YX5300.h"
  #define MP3Stream Serial2   // aliasing serial communication w/ YX5300 module
#endif

#ifdef USE_LIGHTS
  #define DEBUG_LIGHTS 1
  #include "dmx.h"
#endif


// Making serial communications more human-readable
#define Console Serial      // aliasing serial communication w/ IDE 
#define MP3Stream Serial2

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

/** 
 * Pins all here for connection convenience
 */
#ifdef USE_IR
#define IR_SENSOR 34
#endif

#ifdef USE_SOUND
// use ESP pins 17 and 16 for RX/TX2
#define SOUND_RX  18      // connect to TX of MP3 Player module
#define SOUND_TX  19      // connect to RX of MP3 Player module

// YX5300 mp3 player object + config
MD_YX5300 mp3(MP3Stream);
const uint8_t PLAY_FOLDER = 1;        // tracks are all placed in the folder named '00'
#endif

#ifdef USE_PRESSURE
#define OUT_CAP  25
#define IN_CAP   26
#endif

#ifdef USE_LIGHTS
#define LIGHTS_RX   12 // pin for dmx rx, unused
#define LIGHTS_TX   13 // pin for dmx tx
#endif

/* ********************
 * SYSTEM STATE/SETTINGS GLOBALS
 */

unsigned long currentTime;

// IR sensors
bool ir;
//bool i_ready = false;

#ifdef USE_PRESSURE
// pressure sensor
unsigned int pressure = 100;
//bool p_ready = false;
#endif

#ifdef USE_SOUND
// YX5300 playback state
uint8_t trackNum = 4;

uint8_t vol;           // current audio volume
#endif

void setup() {
  Serial.begin(115200);
  setup_ir();
  #ifdef USE_PRESSURE
    setup_pressure();
  #endif
  setup_sound();
  #ifdef USE_LIGHTS
    setup_lights();
  #endif
}

void loop() {
  currentTime = millis()/1000;
  
  loop_ir();
  #ifdef USE_PRESSURE
    loop_pressure();
  #endif
  loop_sound();
  #ifdef USE_LIGHTS
    loop_lights();
  #endif
}
