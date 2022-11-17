#define SENSOR 13

int sensorReading;
const int threshhold = 500;

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  sensorReading = analogRead(SENSOR);
  Serial.println(sensorReading);
  if (sensorReading >= threshhold) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
