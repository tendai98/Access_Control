#include "i2c.h"
#include "client.h"
#include "sensors.h"
#include "sonar.h"
#include "pump.h"

#define TEMP_LIMIT 35
#define TARGET_DISTANCE 5

void setup() {
    initSensors();
    initPump();
    initI2CPeripherals();
    initSonar();
    initWiFi();
    delay(2000);
}

void loop() {
  
  if(TARGET_DISTANCE > getDistance()){
    runI2C();
    if(TEMP_LIMIT > getTempAvg()){
      runPump(500);
      httpGet(ACCESS_URL);
    }else{
      httpGet(ALERT_URL);
    }
  }

  checkSanitizerLevel();
}
