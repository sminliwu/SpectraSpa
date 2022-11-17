/* 
 * Modified from MD_YX5300 Simple Player example code.
 * by: Shanel (S) Wu
 * 
 * Library dependencies: 
 *  - MD_YX5300 
 *  - no other dependencies, due to modifications
 *  
 * MP3 "player" has the following functions:
 *  - Start/pause track playback with switch (press)
 *  - Run state shown using LED indicator
 *  
 * All tracks need to be placed in folder PLAY_FOLDER (defined below)
 * on a micro SD card formatted with FAT32 file system.
 */

#include "MD_YX5300.h"

// Connections for hardware serial interface to the YX5300 module
// use ESP pins 17 and 16 for RX/TX2
//const uint8_t HARDWARE_RX = 16;      // connect to TX of MP3 Player module
//const uint8_t HARDWARE_TX = 17;      // connect to RX of MP3 Player module

#define Console Serial      // aliasing serial communication w/ IDE 
#define MP3Stream Serial2   // aliasing serial communication w/ module

// ESP pins config
const uint8_t PIN_LED = LED_BUILTIN;  // LED to show status
const uint8_t PIN_SWITCH = 33;        // play/pause toggle digital pin, active low (PULLUP)

const uint8_t PLAY_FOLDER = 1;        // tracks are all placed in the folder named '00'

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

// global variables
MD_YX5300 mp3(MP3Stream);

// system state
bool playerPause = true;  // true if player is currently paused
uint8_t vol;              // current audio volume

void setup() {
#if DEBUG
  Console.begin(57600);
#endif
  PRINTS("\n[MD_YX5300 Silly Simple Player]\n");

  // set the hardware pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SWITCH, INPUT_PULLUP);

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
  playerPause = false;
}

void loop() {
  mp3.check();        // run the mp3 receiver
  setStatusLED();     // set the status LED to current status
}

void setStatusLED(void) {
// set the status led - on for running, off for paused
  if (playerPause)
    digitalWrite(PIN_LED, LOW);
  else
    digitalWrite(PIN_LED, HIGH);
}
