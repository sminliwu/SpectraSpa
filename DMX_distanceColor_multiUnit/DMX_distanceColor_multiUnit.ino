#include "dmx.h"
#include <HCSR04.h>

#define COLOR_MIN {255, 0, 255}
#define COLOR_MAX {0, 255, 0}

#define NUM_LIGHTS 2
#define INITCOLOR_A COLOR_MIN
#define ENDCOLOR_A  COLOR_MAX
#define INITCOLOR_B COLOR_MAX
#define ENDCOLOR_B  COLOR_MIN

#define CH_PER_UNIT 8
#define DMX_BRIGHT(n)(1 + CH_PER_UNIT*n)
#define DMX_RED(n)   (2 + CH_PER_UNIT*n)
#define DMX_GREEN(n) (3 + CH_PER_UNIT*n)
#define DMX_BLUE(n)  (4 + CH_PER_UNIT*n)
#define SEND  DMX::Write

#define UPDATE_TIME 10  // micros
#define NUM_AVG 20    // number of distance readings to average


byte triggerPin = 26;
byte echoPin = 27;

double* distanceReadings = new double[NUM_AVG] {0};
double prevDist = 0;
byte avgCounter = 0;

// dmx channels 1-512
// assuming ch 1 is brightness
// 2/3/4 are R/G/B respectively
byte colorArray[NUM_LIGHTS][3] = {
  INITCOLOR_A, 
  INITCOLOR_B
};

byte colorRanges[NUM_LIGHTS][2][3] = {
  {INITCOLOR_A, ENDCOLOR_A},
  {INITCOLOR_B, ENDCOLOR_B}
};

unsigned long readcycle = 0;

void setup() {
  Serial.begin(115200);

  HCSR04.begin(triggerPin, echoPin);

  DMX::Initialize(output);
  for (byte i=0; i<NUM_LIGHTS; i++) {
    SEND(DMX_BRIGHT(i), 255);
    sendColor(i, colorArray[i]);
  }

  // fill avg array once to start things up
  for (byte i=0; i < NUM_AVG; i++) {
    prevDist = updateDistances();
  }
}

void loop() {
  if(millis() - readcycle > UPDATE_TIME) {
    readcycle = millis();
    
    double dist = updateDistances();
//    if (prevDist - dist > 1.0 && sendVal < 245) {
      // got more than 1cm closer
//      sendVal += 10;
//    } else if (dist - prevDist > 1.0 && sendVal > 10) {
      // got more than 1cm farther
//      sendVal -= 10;
//    }
    Serial.print("d = ");
    Serial.print(dist);
    Serial.print("cm | r = ");
//    Serial.println(sendVal);

    for (int i=0; i < NUM_LIGHTS; i++) {
      distToColor(dist, colorArray[i], colorRanges[i][0], colorRanges[i][1]);
      sendColor(i, colorArray[i]);
      Serial.print("light ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(colorArray[i][0]);
      Serial.print(" ");
      Serial.print(colorArray[i][1]);
      Serial.print(" ");
      Serial.print(colorArray[i][2]);
      Serial.println(" ");
    }
    prevDist = dist;
  }
}

double updateDistances() {
  double* distances = HCSR04.measureDistanceCm();

  distanceReadings[avgCounter] = distances[0];

  double acc = 0;
  for (int i=0; i < NUM_AVG; i++) {
    acc += distanceReadings[i];
  }
  acc = acc / NUM_AVG;  // latest average of NUM readings
  avgCounter = (avgCounter + 1) % NUM_AVG;
  return acc;
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

void distToColor(double dist, byte rgb[], byte* startColor, byte* endColor) {
  for (int i=0; i < 3; i++) {
    rgb[i] = lerpValue(dist, 2, 350, 
      startColor[i], 
      endColor[i]
    );
  }
}
