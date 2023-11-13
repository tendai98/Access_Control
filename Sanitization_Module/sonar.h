const int pingPin = D6; // Trigger Pin of Ultrasonic Sensor
const int echoPin = D5; // Echo Pin of Ultrasonic Sensor

void initSonar() {
   pinMode(pingPin, OUTPUT);
   pinMode(echoPin, INPUT);
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}

int getDistance() {
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   long duration = pulseIn(echoPin, HIGH);
   return microsecondsToCentimeters(duration);
}
