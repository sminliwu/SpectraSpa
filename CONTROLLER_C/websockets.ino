/**
 * WEBSOCKET MESSAGE FORMAT -- RECEIVED
 * 
 * - [0]: <?| > - whether message originates from user req
 * - [1]: <a|b|c> - target device
 * - [2] - device data field
 *    - if [1] = a: <p|i|s|l>
 *    - if [1] = b: <d|i|s|l>
 *    - if [1] = c: <s>
 * - [3] - data value OR data subfield
 *    - if [2] = p: <####> - pressure val, 4-digit int
 *    - if [2] = d: <###.#> - distance val, 4-digit float w/ 1 decimal place
 *    - if [2] = i: <0|1> - IR sensor val, binary 0/1
 *    - if [2] = s: <t|n|s|l> - sound subfield
 *    - if [2] = l: <1|2> - lights subfield (unit #)
 * - [4]: data subfield value OR data subsubfield
 *    - sound ([2] = s)
 *        - if [3] = t: <#> - track number of currently playing
 *        - if [3] = n: <*...> - track name of current
 *        - if [3] = s: <#> - number of tracks in shuffle list
 *        - if [3] = l: <0-7> - track num to toggle in shuffle
 *    - lights ([2] = l): <c|s|e> - lights color
 * - [5]: data subsubfield value
 *    - sound shuffle list ([2:3] = sl): <0|1> - bool, whether or not track is in shuffle
 *    - lights color ([2:3] = l<c|s|e>): <HEXHEX> - 6-char hex color
 */

#define A_DATA a
#define B_DATA b
#define C_DATA c
#define USER_MESSAGE ?

#define PRESSURE  p   // only from controller A
#define DISTANCE  d   // only from controller B
#define INFRA     i   // only controllers A+B
#define SOUND     s   // all controllers: A, B, C
#define LIGHTS    l   // only controllers A+B

// lights data fields
// lighting messages from A will start with 'al'
// lighting messages from B will start with 'bl'
#define L_CURRENT   c
#define L_START     s
#define L_END       e

// sound data fields
#define S_CURRENT_TRACK t
#define S_TRACK_NAME    n
#define S_SHUFFLE_NUM   s
#define S_SHUFFLE_LIST  l

#define WS_NEW       message = ""
#define WS_FIELD(x)  message += F("x")
#define WS_ADDN(x)   message += String(x)
#define WS_ADDS(x)   message += x
#define WS_SEND      webSocket.broadcastTXT(message)

#define A_UPDATE      WS_FIELD(A_DATA)
#define B_UPDATE      WS_FIELD(B_DATA)
#define C_UPDATE      WS_FIELD(C_DATA)
#define SELF_UPDATE   WS_NEW; C_UPDATE

// c data fields
#define CURRENT_TRACK   SELF_UPDATE; message += F("t"); message += String(currentTrack); WS_SEND
#define TRACK_NAME      SELF_UPDATE; message += F("n"); message += trackNames[currentTrack]; WS_SEND
#define SHUFFLE_NUM     SELF_UPDATE; message += F("s"); message += String(shuffleNum); WS_SEND
#define SHUFFLE_LIST    SELF_UPDATE; message += F("l"); message += shuffleListString(); WS_SEND

#define SEND_COMPLETE_STATUS_C CURRENT_TRACK; TRACK_NAME; SHUFFLE_NUM; SHUFFLE_LIST

/* Callback function for websocket events. */
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED: // if a client disconnects
//      Serial.printf("[%u] Client disconnected :(\n", num);
      if (webSocket.connectedClients() == 0) { // if no more clients
//        clientConnected = false;
      }
      break;
    case WStype_CONNECTED: { // if a new websocket client connects
//        if (!clientConnected) { // update clientConnected
//          clientConnected = true;
//        }
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        SEND_COMPLETE_STATUS_C;
        break;
      }
    case WStype_TEXT: { // if new text data is received
      Serial.printf("[%u] sent: %s\n", num, payload);
      if (payload[0] == '?') { // message from client (user device) configuring data
        switch(payload[1]) {
          case 'a': // forward to controller a
            forwardMessage('a', (char*) payload);
            break;
          case 'b': // forward to controller b
            forwardMessage('b', (char*) payload);
            break;
          case 'c': // handle request
            handleUserConfig(length, (char*) payload);
            break;
          default: break;
        }
      } else {
        switch(payload[0]) {
          case 'a': // message from control A
            // update stored data
            // forward to client interface
            break;
          case 'b': // message from control B
            // update stored data
            // forward to client interface
            break;
          default: break;
        } 
      } break; 
    }  default:  break;
  }
}

void forwardMessage(char targ, char* message) {
  
}

void handleUserConfig(uint8_t num, char* payload) {
  if (payload[0] != '?' || payload[1] != 'c' || payload[2] != 's') { return; }
  switch (payload[3]) {
    case 't':
      break;
    case 'n':
      break;
    case 's':
      break;
    case 'l':
      break;
    default: break;
  }
}

// reference MD_YX5300_Test example
void playerCB(const MD_YX5300::cbData *status) {
    switch (status->code)
  {
  case MD_YX5300::STS_OK:         Console.print(F("STS_OK"));         break;
  case MD_YX5300::STS_TIMEOUT:    Console.print(F("STS_TIMEOUT"));    break;
  case MD_YX5300::STS_VERSION:    Console.print(F("STS_VERSION"));    break;
  case MD_YX5300::STS_TF_INSERT:  Console.print(F("STS_TF_INSERT"));  break;
  case MD_YX5300::STS_TF_REMOVE:  Console.print(F("STS_TF_REMOVE"));  break;
  case MD_YX5300::STS_ERR_FILE:   Console.print(F("STS_ERR_FILE"));   break;
  case MD_YX5300::STS_ACK_OK:     Console.print(F("STS_ACK_OK"));     break;
  case MD_YX5300::STS_FILE_END:   Console.print(F("STS_FILE_END"));   break;
  case MD_YX5300::STS_INIT:       Console.print(F("STS_INIT"));       break;
  case MD_YX5300::STS_STATUS:     Console.print(F("STS_STATUS"));     break;
  case MD_YX5300::STS_EQUALIZER:  Console.print(F("STS_EQUALIZER"));  break;
  case MD_YX5300::STS_VOLUME:     Console.print(F("STS_VOLUME"));     break;
  case MD_YX5300::STS_TOT_FILES:  Console.print(F("STS_TOT_FILES"));  break;
  case MD_YX5300::STS_PLAYING:
    CURRENT_TRACK;    
    Console.print(F("STS_PLAYING")); 
  case MD_YX5300::STS_FLDR_FILES: Console.print(F("STS_FLDR_FILES")); break;
  case MD_YX5300::STS_TOT_FLDR:   Console.print(F("STS_TOT_FLDR"));   break;
  default: Console.print(F("STS_??? 0x")); Console.print(status->code, HEX); break;
  }
}

String shuffleListString() {
  String res = "";
  for (byte i=0; i<shuffleNum; i++) {
    res += shuffleTracks[i];
  }
  return res;
}
