#include <Adafruit_Fingerprint.h>
#include "SoftwareSerial.h"

SoftwareSerial fps(2,3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fps);

int getRandomID(){
  return random(1,127);
}

int runProc(){
  int code = -1;
  
  while(code != FINGERPRINT_OK){
      code = finger.getImage();
      if(code == FINGERPRINT_OK){
        lcd.clear();
        lcd.print("[+] Captured");
        break;
      }else{
        lcd.clear();
        lcd.print("Place Finger");
      }

      delay(1000);
  }

  code = finger.image2Tz(1);

  if(code == FINGERPRINT_OK){
      lcd.clear();
      lcd.print("Got biometric");
      delay(100);
  }else{
      lcd.print("Failed");
      return -1;
  }

  lcd.clear();
  lcd.print("Remove finger");
  delay(1000);
  
  lcd.clear();
  lcd.print("Place it again");
  
  code = 0;
  while (code != FINGERPRINT_NOFINGER){
      code = finger.getImage();
  }

  code = -1;
  while(1){
     if(code != FINGERPRINT_OK){
       code  = finger.getImage(); 
     }else{
        break;
          lcd.clear();
          lcd.print("Scan Complete");
     }
  }

  code = finger.image2Tz(2);

  if(code == FINGERPRINT_OK){
    lcd.clear();
    lcd.print("Got Print");
  }else{
    lcd.clear();
    lcd.print("Failed");
    return -1;
  }

  delay(1000);
  code = finger.createModel();
  
  if(code == FINGERPRINT_OK){
  
    lcd.clear();
    lcd.print("Prints Matched");
  }else{
    lcd.clear();
    lcd.print("No Match");
    return -1;
  }

  delay(1000);
  lcd.clear();
  code = finger.storeModel(getRandomID());
  if(code == FINGERPRINT_OK){
    lcd.print("Enrolled");
    return 0;
  }else{
    lcd.print("Failed to store");
    return -1;
  }
}

void enrollFingerprints(){
  finger.begin(57600);
  delay(1000);
  if(finger.verifyPassword()){
    lcd.clear();
    lcd.print("FPS Ready");
    runProc();
    
  }else{
    lcd.clear();
    lcd.print("NO FPS");
  }
  fps.end();
}

int scanFingerprint(){
  finger.begin(57600);
  delay(1000);
  
  int code = -1;
  
  while(code != FINGERPRINT_OK){

    code = finger.getImage(); 
    lcd.clear();
    lcd.print("Scanning....");
    delay(250);
     code = finger.image2Tz();
     
     if(code == FINGERPRINT_OK){
       code = finger.fingerSearch();
       
       if(code == FINGERPRINT_OK){
          lcd.clear();
          lcd.print("Access Granted");
          delay(1000);
          return finger.fingerID;
       }else{
          lcd.clear();
          lcd.print("Access Denied");
          delay(1000);
       }
     }else{
       delay(100);
     }
  }
  
  return -1;
}
