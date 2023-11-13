#include <Servo.h>

#define SERVO_PIN 8
Servo servo;

void initServo(){
  servo.attach(SERVO_PIN);
}

void closeMode(){
  servo.write(145);
}

void openMode(){
  servo.write(45);
}
