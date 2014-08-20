#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 
int freqMin, freqMax, freqMiddle;
int delayMov; //temps nécessaire pour que le servo se déplace de min à max
 
void setup() 
{ 
  Serial.begin(9600);
  
 // A définir dans Servo.h (à remplacer dans la bibli servo) ligne 50
 #ifdef DGS
  freqMin = 500;
  freqMax = 2290;
  delayMov = 400;
 #elif defined(ALIGN)
  freqMin = 780;
  freqMax = 2180;
  delayMov = 180;
 #elif defined(FUTABA)
  freqMin = 776;
  freqMax = 2177;
  delayMov = 120;
 #endif
 
 freqMiddle = ( freqMin + freqMax ) / 2 ; 
 myservo.attach(10, freqMin, freqMax);
} 
 
 
void loop() 
{ 
  
  //Test 1 : 
  //Switch entre le minimum, le milieu et le maximum
  /*sendAFreq(freqMin);
  delay(3000);
  sendAFreq(freqMiddle);
  delay(3000);
  sendAFreq(freqMax);
  delay(3000);
  sendAFreq(freqMiddle);
  delay(3000);*/
  
  
  //Test 2 : 
  //Balaye l'ensemble des positions possibles
  /*for(pos = freqMin; pos < freqMax; pos += 10){
    myservo.writeMicroseconds(pos); 
    delay(10);
  } 
  for(pos = freqMax; pos>freqMin; pos-=10){                                
    myservo.writeMicroseconds(pos);
    delay(10);
  }*/
} 

//Attache et détache le servo à chaque fois pour éviter les soubressaux et éviter d'envoyer des ordres en continue 
//A voir si c'est plus avantageux que de laisser simplement tourner
void sendAFreq(int freq){
  myservo.attach(10, freqMin, freqMax);
  myservo.writeMicroseconds(freq);
  delay(delayMov);
  myservo.detach();
}
