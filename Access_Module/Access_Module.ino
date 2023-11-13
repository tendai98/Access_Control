#include "lcd.h"
#include "fps.h"
#include "rfid.h"
#include "servo.h"

#define VISITOR_TAG_1 0xFFFFCF33
#define VISITOR_TAG_2  0x00003893

void rfidAccessMode(){
  int id = -1;
  lcdClear();
  while(true){
    id = readCard();
    if(id == VISITOR_TAG_1){
      Serial.write(0x41);
      lcdClear();
      lcdPrintString("VISITOR 1 ACCESS");
      Serial.write(0x43);
      openMode();
      delay(5000);
      closeMode();
      lcdClear();
      break;
    }if(id == VISITOR_TAG_2){
      lcdClear();
      lcdPrintString("VISITOR 2 ACCESS");
      Serial.write(0x44);
      
      openMode();
      delay(5000);
      closeMode();
      lcdClear();
      break;
    }else{
      lcdClear();
      lcdPrintString("Tap your ID card");
    }
    delay(500);
  }

  lcd.print("[:] Confirmed");
  delay(1000);
  
}

void fpsAccessMode(){
  lcdClear();
  lcdPrintString("Place your finger");
  while (true){
      if(scanFingerprint() != -1){
          openMode();
          delay(5000);
          closeMode();
          Serial.write(0x41);
          lcd.clear();
          lcd.print("[:] Confirmed");
          Serial.write(0x45);
          delay(1000);
          break;
      }else{
        break;
      }
  }

}

void programMode(){
    enrollFingerprints();
}

void executeOpcode(char opcode){
  lcd.clear();
  switch(opcode){
    case 0x42:
      lcdClear();
      lcdPrintString("FPS ACCESS MODE");
      fpsAccessMode();
      delay(1000);
      lcdClear();
      break;
      
    case 0x43:
        lcdClear();
        lcdPrintString("[+] PROGRAM MODE");
        programMode();
        delay(1000);
        lcdClear();
        break;

    case 0x44:
      lcdClear();
      lcdPrintString("RFID ACCESS MODE");
      rfidAccessMode();
      delay(1000);
      lcdClear();
      break;          
  }
}

void sanitizerLEDS(){
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  analogWrite(6, 1023);
}


void setBanner(){
  lcd.setCursor(0,0);
  lcd.print("[Willard--Mhaka]");
  lcd.setCursor(0,1);
  lcd.print("[ACCESS CONTROL]");
}

void setup() {
    sanitizerLEDS();
    Serial.begin(115200);
    initServo();
    initLCD();
    initRFID();
    lcdPrintString("[ACCESS CONTROL]");
    delay(1000);
    lcdClear();
    setBanner();
    closeMode();
}

char opcode = 0;

void loop() {
    while(Serial.available()){
        opcode = Serial.read();
        executeOpcode(opcode);
        Serial.flush();
    }
    setBanner();
    delay(10);
}
