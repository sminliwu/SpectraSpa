/**
 * SPECTRA CONTROLLER C
 * - playing music on loop
 * - wifi interface to configure A, B, C
 */

//#define HOME_NETWORK 1  // comment out when not at home

#include "auth.h"
#include "MD_YX5300.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>

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

unsigned long currentTime;

// HTTP port for server
const uint8_t port = 80;
AsyncWebServer server(port);
// Websocket for server/client data updates
const uint8_t WSport = 81;
WebSocketsServer webSocket(WSport);
String message = "";

MD_YX5300 mp3(MP3Stream);
const uint8_t PLAY_FOLDER = 1;   // tracks are all placed in this folder
const uint8_t NUM_TRACKS = 8;

// track numbers for Spectra Spa music files
String track0 = F("Welcome 1");
String track1 = F("Welcome 2");
String track2 = F("Sound bath 1");
String track3 = F("Sound bath 2");
String track4 = F("Galactic");
String track5 = F("Vocals");
String track6 = F("Affirm");
String track7 = F("Frogs");

String* trackNames = new String[8]  {track0, track1, track2, track3, track4, track5, track6, track7};
bool* shuffleTracks = new bool[8]   {0,      0,      1,      1,      0,      0,      0,      1 };

uint8_t shuffleNum = 3;
uint8_t currentTrack = 2;
uint8_t vol;           // current audio volume

void setup() {
  Serial.begin(115200);
  PRINTS("files?");
  message.reserve(20);
  if (!SPIFFS.begin()) {
    PRINTS("SPIFF error");
  }

  PRINTS("wifi?");
  WiFi.begin(ssid, pw);
  while (WiFi.status() != WL_CONNECTED) { Serial.print(F("?")); }
  PRINT("\nIP address: ", WiFi.localIP());
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // loading page and assets
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html", false);
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });
  
  server.begin();
  
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);
  mp3.begin();
  mp3.setSynchronous(true);
  mp3.setCallback(playerCB);

  vol = mp3.volumeMax();
  bool b = mp3.volume(vol);
//  PRINT("volume is ", vol);
  mp3.playFolderShuffle(PLAY_FOLDER);
//  mp3.queryFolderFiles(PLAY_FOLDER);
  mp3.playNext();
}

void loop() {
  currentTime = millis()/1000;
  
  mp3.check();        // run the mp3 receiver
  wifiLoop();
  webSocket.loop();
}

void wifiLoop() {
  static unsigned int lastConnectAttempt;
  if (currentTime - lastConnectAttempt >= 30) {
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      WiFi.begin(ssid, pw);
      lastConnectAttempt = currentTime;
    }
  }
}
