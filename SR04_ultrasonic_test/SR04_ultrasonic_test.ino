/*
 * example code from https://github.com/d03n3rfr1tz3/HC-SR04
 */

#include <HCSR04.h>

#define NUM_AVERAGE 20

byte triggerPin = 15;
byte echoPin = 13;

double* distanceReadings = new double[NUM_AVERAGE] {0};
byte avgCounter = 0;

void setup () {
  Serial.begin(9600);
  HCSR04.begin(triggerPin, echoPin);
}

void loop () {
  double* distances = HCSR04.measureDistanceCm();

  distanceReadings[avgCounter] = distances[0];

  double acc = 0;
  for (int i=0; i < NUM_AVERAGE; i++) {
    acc+= distanceReadings[i];
  }
  acc = acc / NUM_AVERAGE;
  Serial.println(acc);
//  Serial.println(" cm");

  avgCounter = (avgCounter + 1) % NUM_AVERAGE;

  delay(5);
}
