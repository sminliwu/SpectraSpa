#include "dmx.h"
#include <HCSR04.h>

#define DMX_RED   2
#define DMX_GREEN 3
#define DMX_BLUE  4
#define SEND  DMX::Write

#define UPDATE_TIME 25  // micros
#define NUM_AVG 20    // number of distance readings to average

#define COLOR_MIN {0, 0, 255}
#define COLOR_MAX {0, 255, 255}

byte triggerPin = 15;
byte echoPin = 13;

double* distanceReadings = new double[NUM_AVG] {0};
double prevDist = 0;
byte avgCounter = 0;

//uint16_t ch = 2; // dmx channels 1-512
// assuming ch 1 is brightness
// 2/3/4 are R/G/B respectively
uint8_t sendVal = 0;  // 0-255

unsigned long readcycle = 0;

void setup() {
  Serial.begin(115200);

  HCSR04.begin(triggerPin, echoPin);

  DMX::Initialize(output);
  SEND(1, 255);
  SEND(DMX_BLUE, 255);

  // fill avg array once to start things up
  for (byte i=0; i < NUM_AVG; i++) {
    prevDist = updateDistances();
  }
}

void loop() {
  if(millis() - readcycle > UPDATE_TIME) {
    readcycle = millis();
    
    double dist = updateDistances();
    if (prevDist - dist > 1.0 && sendVal < 245) {
      // got more than 1cm closer
      sendVal += 10;
    } else if (dist - prevDist > 1.0 && sendVal > 10) {
      // got more than 1cm farther
      sendVal -= 10;
    }
    Serial.print("d = ");
    Serial.print(dist);
    Serial.print("cm | r = ");
    Serial.println(sendVal);
    SEND(DMX_RED, sendVal);
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
