#define PROXIMITY D7

void initSensors(){
  pinMode(PROXIMITY, INPUT);
}

int readFluidLevel(){
  return analogRead(A0);
}

int readProximitySensor(){
  return !digitalRead(PROXIMITY);
}
