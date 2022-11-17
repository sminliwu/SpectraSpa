#ifdef USE_PRESSURE

// capacitive pressure sensor
#define STATE_P_RESET  0
#define STATE_P_CHARGE 1

#define NUM_AVG 30
#define PRESSURE_SENSOR_DELAY 10


void setup_pressure() {
  // put your setup code here, to run once:
  pinMode(OUT_CAP, OUTPUT);
  pinMode(IN_CAP, INPUT);
}

void loop_pressure() {
  static byte state = STATE_P_RESET;
  
  static unsigned long startTime;
  static unsigned int sensVal;
  
  static unsigned int* readings = new unsigned int[NUM_AVG] {0};
  static byte avgCounter = 0;

  switch(state) {
    case STATE_P_RESET:
      if (millis() - startTime >= PRESSURE_SENSOR_DELAY) {
        startTime = micros();
        digitalWrite(OUT_CAP, HIGH);  
        state++;
      }
      break;
    case STATE_P_CHARGE:
      if (digitalRead(IN_CAP) == HIGH) {
        unsigned int sensVal = int(micros()-startTime);

        readings[avgCounter] = sensVal;

        unsigned int acc = 0;
        for (byte i=0; i < NUM_AVG; i++) {
          acc += readings[i];
        }
        pressure = acc / NUM_AVG;  // latest average of NUM readings
        avgCounter = (avgCounter + 1) % NUM_AVG;
        Console.println(pressure);

        // reset
        digitalWrite(OUT_CAP, LOW);
        startTime = millis();
        state = STATE_P_RESET;
      }
      break;
    default: break;
  }
}

#endif
