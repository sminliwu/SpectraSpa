// DMX lights
// lights control w/ DMX/RS-485 will need two pins for serial RX/TX
//#define DMX_SERIAL_INPUT_PIN    GPIO_NUM_9 // pin for dmx rx
//#define DMX_SERIAL_OUTPUT_PIN   GPIO_NUM_10 // pin for dmx tx
// use Serial1

// dmx channels 1-512
// assuming ch 1 is brightness
// 2/3/4 are R/G/B respectively


#define COLOR_MIN {255, 20, 255}
#define COLOR_MAX {20, 20, 255}

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

#define LIGHTS_UPDATE_TIME 10  // millis

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
  
  if (millis()-prevTime >= LIGHTS_UPDATE_TIME) {
    for (int i=0; i < NUM_LIGHTS; i++) {
      distToColor(dist, colorArray[i], colorRanges[i][0], colorRanges[i][1]);
      sendColor(i, colorArray[i]);
//      Serial.print("light ");
//      Serial.print(i);
//      Serial.print(": ");
//      Serial.print(colorArray[i][0]);
//      Serial.print(" ");
//      Serial.print(colorArray[i][1]);
//      Serial.print(" ");
//      Serial.print(colorArray[i][2]);
//      Serial.println(" ");
    }
  }
}

void sendColor(byte num, byte* rgb) {
  SEND(DMX_RED(num), rgb[0]);
  SEND(DMX_GREEN(num), rgb[1]);
  SEND(DMX_BLUE(num), rgb[2]);
}

byte lerpValue(double x, double x_min, double x_max,
  byte y_min, byte y_max) {
  return byte((x / (x_max - x_min)) * (y_max - y_min));
}

void distToColor(double d, byte rgb[], byte* startColor, byte* endColor) {
  for (int i=0; i < 3; i++) {
    rgb[i] = lerpValue(d, 2, 350, 
      startColor[i], 
      endColor[i]
    );
  }
}
