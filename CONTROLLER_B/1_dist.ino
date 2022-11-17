// SR04 distance sensor
#define UPDATE_TIME 10  // micros
#define NUM_AVG 20    // number of distance readings to average

void setup_dist() {
   HCSR04.begin(triggerPin, echoPin);
}

void loop_dist() {
  static unsigned long startTime;
  static double* distanceReadings = new double[NUM_AVG] {0};
  static double prevDist = 0;
  static byte avgCounter = 0;

  if(millis() - startTime > UPDATE_TIME) {    
    double* distances = HCSR04.measureDistanceCm();

    distanceReadings[avgCounter] = distances[0];

    double acc = 0;
    for (int i=0; i < NUM_AVG; i++) {
      acc += distanceReadings[i];
    }
    dist = acc / NUM_AVG;  // latest average of NUM readings
    avgCounter = (avgCounter + 1) % NUM_AVG;
    
    Serial.print("d = ");
    Serial.print(dist);
    Serial.println("cm");
    startTime = millis();
  }
}
