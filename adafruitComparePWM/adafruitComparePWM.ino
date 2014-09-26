#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);

#define SERVOMIN  185 // 900°
#define SERVOMIDDLE  307 // 1500°
#define SERVOMAX  430 // 2100°

void setup() {
  Serial.begin(9600);
  Serial.println("16 channel Servo test!");

  pwm.begin();
  pwm1.begin();
  
  pwm.setPWMFreq(50);  // Analog servos run at ~60 Hz updates
  pwm1.setPWMFreq(50);
  
  for (uint8_t i = 0; i < 8; i++){
    pwm1.setPWM(i, 0, SERVOMIN);
  }
  
}

void loop() {

  //pwm.setPWM(0, 0, SERVOMIN);
  
  /*pwm1.setPWM(0, 0, SERVOMIN);
  delay(1000);
  //pwm.setPWM(0, 0, SERVOMAX);
  pwm1.setPWM(0, 0, SERVOMAX);
  delay(1000);*/

}
