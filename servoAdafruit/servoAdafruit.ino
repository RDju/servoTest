

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Z_OSC.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

/************OSC*************/
byte myMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte myIp[]  = { 192, 168, 144, 210 };
int  myPort  = 9000;

Z_OSCServer server;
Z_OSCMessage *rcvMes;
/*****************************/

/***********Servos**********/
#define FUTABAMIN  195 
#define FUTABAMAX  534 

#define ALIGNMIN  192 
#define ALIGNMAX  535

int32_t servonum = 0;
int32_t value;
int decalage = 500;
int freqPWM = 60;

typedef struct servo{
  char name[8];
  int32_t valmin, valmax, currvalue;
} servo;

servo myservos[16];

/********************************/

#define expressionPin 0
uint16_t loopCount;
uint16_t expPedalValues[50];
boolean isListeningPedal;
int32_t exprPin = 0; //Servo controlé par la pédale
uint16_t prevExprValue = 0; //pour ne pas lire les valeurs quand la pédale ne bouge pas (mettre la pédale au max pour que ça fonctionne)
uint16_t exprMin = 0;
uint16_t exprMax = 937;
int32_t mode = 0;


void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(freqPWM);
  
  pinMode(53, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  Ethernet.begin(myMac, myIp);
  server.sockOpen(myPort);
  
  for (uint8_t i = 0; i<3; i++){
    myservos[i].valmin = 221;
    myservos[i].valmax = 516;
    myservos[i].currvalue = (myservos[i].valmin + myservos[i].valmax) / 2;
  }
}

void loop() {
  
  /*if (mode){
    updateExpressionPedal();
  }*/
  
  //Test 1 :
  // Gestion via OSC avec le programme pure data
  if(server.available()){
    
    server.availableFlush();
    rcvMes = server.getMessage();
        
    if(!strcmp(rcvMes->getZ_OSCAddress(), "/POS")){ //Déplacement d'un moteur entre le max et le min 
    
      servonum = rcvMes->getInteger32(1);
      value = rcvMes->getInteger32(0);
      myservos[servonum].currvalue = map(value, 0, 600, myservos[servonum].valmin, myservos[servonum].valmax);
      
      Serial.print("Deplacement servo ");Serial.print(servonum);Serial.print(" : ");Serial.println(myservos[servonum].currvalue);
      
      pwm.setPWM(servonum, 0, myservos[servonum].currvalue );
      
    } else if (!strcmp(rcvMes->getZ_OSCAddress(), "/REF")){ // Définition du max et du min d'une pin
      
      servonum = rcvMes->getInteger32(0);
      myservos[servonum].valmin = rcvMes->getInteger32(1);
      myservos[servonum].valmax = rcvMes->getInteger32(2);
      
      Serial.print("Fixation des extremites servo ");Serial.print(servonum);Serial.print(" : "); Serial.print(myservos[servonum].valmin);Serial.print(" "); Serial.println(myservos[servonum].valmax);
    } else if (!strcmp(rcvMes->getZ_OSCAddress(), "/EXPR")){
      exprPin = rcvMes->getInteger32(0);
    } else if (!strcmp(rcvMes->getZ_OSCAddress(), "/MIN")){
      exprMin = analogRead(expressionPin);
      Serial.print("min : ");Serial.println(exprMin);
    } else if (!strcmp(rcvMes->getZ_OSCAddress(), "/MAX")){
      exprMax = analogRead(expressionPin);
      Serial.print("max : ");Serial.println(exprMax);
    } else if (!strcmp(rcvMes->getZ_OSCAddress(), "/MODE")){
      mode = rcvMes->getInteger32(0);
    } else if (!strcmp(rcvMes->getZ_OSCAddress(), "/INIT")){
      Serial.println("youpi init");
    }
  }
  
  //Test 2 :
  // Test de délais entre les deux servos (Align sur la pin 0, Futaba sur la pin 1)
  /*pwm.setPWM(0, 0, FUTABAMIN);
  delay(45);
  pwm.setPWM(3, decalage, FUTABAMIN+decalage);
  delay(1000);
  
  pwm.setPWM(0, 0, FUTABAMAX);
  delay(45);
  pwm.setPWM(3, decalage, FUTABAMAX+decalage);
  delay(1000);*/
}


void doExpressionPedal(uint16_t expVal){
  myservos[exprPin].currvalue = map(constrain(expVal, exprMin, exprMax), exprMin, exprMax, myservos[exprPin].valmin, myservos[exprPin].valmax);
  //pwm.setPWM(servonum, 0, myservos[servonum].currvalue);
  pwm.setPWM(0, 0, myservos[servonum].currvalue);
  pwm.setPWM(1, 0, myservos[servonum].currvalue);
}

void updateExpressionPedal(){
  
  uint16_t expValOK;  
  uint16_t expVal = analogRead(expressionPin);

  if(loopCount == 29){
    expPedalValues[loopCount] = expVal;
    expValOK = abs(expPedalValues[loopCount] - expPedalValues[0]);
    loopCount = 0;
  }
  if(loopCount < 29){
    expPedalValues[loopCount] = expVal;
    expValOK = abs(expPedalValues[loopCount] - expPedalValues[loopCount + 1]);
    loopCount ++ ;
  }

  if(!isListeningPedal){
    if(expValOK > 3)
        isListeningPedal = true;
  }
  
  if(isListeningPedal  && (expVal != prevExprValue)){
    doExpressionPedal(expVal);
    Serial.print("Expr value : ");Serial.println(expVal);
  }
     
   prevExprValue = expVal;
}


