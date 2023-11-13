#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void lcdPrintString(char * buff){
  lcd.print(buff);
}

void lcdPrintObject(String * obj){
  lcd.print(*obj);
}

void lcdClear(){
  lcd.clear();
}

void initLCD(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
}
