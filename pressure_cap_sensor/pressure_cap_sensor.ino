#define NUM_AVG 30

#define PRESSURE_SENSOR_DELAY 10

#define OUT_CAP  27
#define IN_CAP   34

long* readings = new long[NUM_AVG] {0};
byte avgCounter = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(OUT_CAP, OUTPUT);
  pinMode(IN_CAP, INPUT);

  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(OUT_CAP, LOW);
  delay(PRESSURE_SENSOR_DELAY);

  long startTime = micros();

  digitalWrite(OUT_CAP, HIGH);
  while (digitalRead(IN_CAP) == LOW);
  long sensVal = micros()-startTime;

  readings[avgCounter] = sensVal;

  long acc = 0;
  for (int i=0; i < NUM_AVG; i++) {
    acc += readings[i];
  }
  acc = acc / NUM_AVG;  // latest average of NUM readings
  avgCounter = (avgCounter + 1) % NUM_AVG;

  Serial.println(acc);
}
