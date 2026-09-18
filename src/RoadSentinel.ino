/*
 * RoadSentinel: Anti-Sleep Driver Alert System
 * Hardware: Arduino Pro Mini, IR Sensor, Buzzer, Vibration Motor
 */

const int irSensorPin = A1;  // IR Sensor input
const int alertPin = 3;      // BC547 Transistor base for Buzzer & Motor

unsigned long eyeClosedStartTime = 0;
const unsigned long sleepThreshold = 2000; // 2 seconds threshold
bool isEyeClosed = false;

void setup() {
  pinMode(irSensorPin, INPUT);
  pinMode(alertPin, OUTPUT);
  digitalWrite(alertPin, LOW); 
  
  Serial.begin(9600); 
  Serial.println("RoadSentinel Initialized.");
}

void loop() {
  int sensorState = digitalRead(irSensorPin); 
  
  if (sensorState == HIGH) { 
    if (!isEyeClosed) {
      isEyeClosed = true;
      eyeClosedStartTime = millis(); 
    } 
    else if (millis() - eyeClosedStartTime >= sleepThreshold) {
      digitalWrite(alertPin, HIGH); 
      Serial.println("ALERT: Drowsiness Detected!");
    }
  } else {
    isEyeClosed = false;
    digitalWrite(alertPin, LOW);
  }
}