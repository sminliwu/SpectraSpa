// IR sensors

#define IR_SENSOR_DELAY 10

void setup_ir() {
  pinMode(IR_SENSOR, INPUT);
}

void loop_ir() {
  static unsigned long startTime;

  static bool prev;

  if (millis() - startTime >= IR_SENSOR_DELAY) {
    bool reading = digitalRead(IR_SENSOR);
    if (reading != prev) {
      ir = true;
      prev = reading;
      Console.println("IR switched");
    }
    startTime = millis();
  }
}
