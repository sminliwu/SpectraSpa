#ifdef USE_LIGHTS

// DMX lights
// lights control w/ DMX/RS-485 will need two pins for serial RX/TX
//#define DMX_SERIAL_INPUT_PIN    GPIO_NUM_9 // pin for dmx rx
//#define DMX_SERIAL_OUTPUT_PIN   GPIO_NUM_10 // pin for dmx tx
// use Serial1

// dmx channels 1-512
// assuming ch 1 is brightness
// 2/3/4 are R/G/B respectively


#define COLOR_MIN {200,0,0}
#define COLOR_MAX {0, 255, 255}

#define NUM_LIGHTS 2
#define INITCOLOR_A COLOR_MIN
#define ENDCOLOR_A  COLOR_MAX
#define INITCOLOR_B COLOR_MIN
#define ENDCOLOR_B  COLOR_MAX

#define CH_PER_UNIT 8
#define DMX_BRIGHT(n)(1 + CH_PER_UNIT*n)
#define DMX_RED(n)   (2 + CH_PER_UNIT*n)
#define DMX_GREEN(n) (3 + CH_PER_UNIT*n)
#define DMX_BLUE(n)  (4 + CH_PER_UNIT*n)
#define SEND  DMX::Write

#define LIGHTS_UPDATE_TIME 10  // micros

byte colorArray[NUM_LIGHTS][3] = {
  INITCOLOR_A, 
  INITCOLOR_B
};

byte colorRanges[NUM_LIGHTS][2][3] = {
  {INITCOLOR_A, ENDCOLOR_A},
  {INITCOLOR_B, ENDCOLOR_B}
};

void setup_lights() {
  DMX::Initialize(output);
  for (byte i=0; i<NUM_LIGHTS; i++) {
    SEND(DMX_BRIGHT(i), 255);
    sendColor(i, colorArray[i]);
  }
}

void loop_lights() {
  static unsigned long prevTime;
#ifdef USE_PRESSURE
#else
  static unsigned long lastColorChange;
  static unsigned int lastSwitch;
  static byte cycleDir[3] = {0, 1, 1};
#endif
  
  if (millis()-prevTime >= LIGHTS_UPDATE_TIME) {
    for (int i=0; i < NUM_LIGHTS; i++) {
#ifdef USE_PRESSURE
      pressureToColor(pressure, colorArray[i], colorRanges[i][0], colorRanges[i][1]);
#else
      if (millis()-lastColorChange >= 20) {
        for (byte j=0; j < 3; j++) {
          if (colorArray[i][j] >= 245) {
            cycleDir[j] = 0;
          } else if (colorArray[i][j] <= 10) {
            cycleDir[j] = 1;
          }
          colorArray[i][j] = cycleDir[j] ? colorArray[i][j]+1 : colorArray[i][j]-1;
          if (millis()-lastSwitch >= 5000 && ir) {
            colorArray[i][j] = cycleDir[j] ? colorArray[i][j]+5 : colorArray[i][j]-5;
            lastSwitch = millis();
           }
        }
        lastColorChange = millis();
      }
#endif
      sendColor(i, colorArray[i]);
#ifdef DEBUG_LIGHTS
      Serial.print("light ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(colorArray[i][0]);
      Serial.print(" ");
      Serial.print(colorArray[i][1]);
      Serial.print(" ");
      Serial.print(colorArray[i][2]);
      Serial.println(" ");
#endif
    }
  }
}

void sendColor(byte num, byte* rgb) {
  SEND(DMX_RED(num), rgb[0]);
  SEND(DMX_GREEN(num), rgb[1]);
  SEND(DMX_BLUE(num), rgb[2]);
}

byte lerpValue(uint8_t x, uint8_t x_min, uint8_t x_max,
  byte y_min, byte y_max) {
  return byte((float(x) / (x_max - x_min)) * (y_max - y_min));
}

void pressureToColor(unsigned int p, byte rgb[], byte* startColor, byte* endColor) {
  for (int i=0; i < 3; i++) {
    rgb[i] = lerpValue(p, 10, 350, 
      startColor[i], 
      endColor[i]
    );
  }
}

#endif
