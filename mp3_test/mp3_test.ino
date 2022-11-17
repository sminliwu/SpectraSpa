// Simple MP3 player using the MD_YX5300 library.
//
// MP3 player has the following functions:
// - Start/pause track playback with switch (press)
// - Play next track with switch (long press)
// - Volume control using potentiometer
// - Run state shown using LED indicator
//
// Implemented using all synchronous calls and kept as simple as possible.
// All tracks need to be placed in folder PLAY_FOLDER (defined below)
//
// Library dependencies:
// MD_UISwitch can be found at https://github.com/MajicDesigns/MD_UISwitch
//

#ifndef USE_SOFTWARESERIAL
#define USE_SOFTWARESERIAL 0   ///< Set to 1 to use SoftwareSerial library, 0 for native serial port
#endif

#include "MD_YX5300.h"

#if USE_SOFTWARESERIAL
#include <SoftwareSerial.h>

// Connections for serial interface to the YX5300 module
const uint8_t ARDUINO_RX = 34;    // connect to TX of MP3 Player module
const uint8_t ARDUINO_TX = 35;    // connect to RX of MP3 Player module

SoftwareSerial  MP3Stream;  // MP3 player serial stream for comms
#define Console Serial           // command processor input/output stream
#else

// HARDWARE SERIAL WORKS BETTER!!!!!!!!!!!!!
#define MP3Stream Serial2  // Native serial port - change to suit the application
// ESP pins 17 and 16
#define Console   Serial   // command processor input/output stream
#endif

const uint8_t PIN_SWITCH = 33;    // play/pause toggle digital pin, active low (PULLUP)
const uint8_t PIN_LED = LED_BUILTIN;       // LED to show status
const uint8_t POT_VOLUME = 13;   // volume control pot analog pin

const uint8_t PLAY_FOLDER = 1;   // tracks are all placed in this folder

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

// Define global variables
MD_YX5300 mp3(MP3Stream);
bool playerPause = true;  // true if player is currently paused
bool prevSwitch = false;
uint8_t vol;   // current audio volume

void processVolume()
// read the volume pot and set the volume if it has changed
{
  uint8_t newVolume = map(analogRead(POT_VOLUME), 0, 1023, 0, mp3.volumeMax());
  
  if (newVolume != vol)
  {
    PRINT("\nSetting volume ", newVolume);
    vol = newVolume;
    bool b = mp3.volume(vol);
    PRINT(" result ", b);
  }
}

void processSwitch()
// read the switch and act if it has been pressed
{
  bool pressed = !digitalRead(PIN_SWITCH);
  bool b;
  // toggle switch
  if (pressed && playerPause) {
//
//    PRINTS("\nPlaying next");
//    b = mp3.playNext();
//    PRINT(" result ", b);
//  }
//  // don't use 'else' here in case this is a force
    playerPause = !playerPause;
    PRINT("\nSwitching to ", playerPause ? F("PAUSE") : F("PLAY"));
    b = mp3.playNext();
    PRINT(" result ", b);
  } if (!pressed && !playerPause) {
    playerPause = !playerPause;
    PRINT("\nSwitching to ", playerPause ? F("PAUSE") : F("PLAY"));
    b = mp3.playPause();
    PRINT(" result ", b);
  }
}

void setStatusLED(void)
// set the status led - on for running, off for paused
{
  if (playerPause)
    digitalWrite(PIN_LED, LOW);
  else
    digitalWrite(PIN_LED, HIGH);
}

void setup()
{
#if DEBUG
  Console.begin(57600);
#endif
  PRINTS("\n[MD_YX5300 Simple Player]\n");

  // set the hardware pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(POT_VOLUME, INPUT_PULLUP);

  // initialize global libraries
#if USE_SOFTWARESERIAL
  MP3Stream.begin(MD_YX5300::SERIAL_BPS, SWSERIAL_8N1, ARDUINO_RX, ARDUINO_TX);
#else
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);
#endif
  if (!MP3Stream) { // If the object did not initialize, then its configuration is invalid
    Serial.println("Invalid SoftwareSerial pin configuration, check config"); 
    while (1) { // Don't continue with invalid configuration
      delay (1000);
    }
  } else {
    Serial.println("SoftwareSerial has been set up");
  } 

  mp3.begin();

  Serial.println(mp3.getStsCode());
  
  mp3.setSynchronous(true);
  mp3.playFolderRepeat(PLAY_FOLDER);
//  processVolume();    // force these to set up the hardware
  vol = 100;//mp3.volumeMax();
  PRINT("\nSetting volume max ", vol);
  bool b = mp3.volume(vol);
  PRINT(" result ", b);
  processSwitch();

  // Set up the switches modes - only simple switching
//  sw.begin();
//  sw.enableDoublePress(false);
//  sw.enableRepeat(false);
}

void loop()
{
  mp3.check();        // run the mp3 receiver
//  processVolume();    // set the volume if required
  processSwitch();    // process the user switch
  setStatusLED();     // set the status LED to current status
}
