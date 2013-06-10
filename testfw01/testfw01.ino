#include <EEPROM.h>
/* ButterBot source
   testfw01.ino - File for testing new features
   Created by Bill Weiler, May 3, 2013.
   Released into the public domain.
*/
//h-bridge (motors) control
const int standby = 2;    //PD2,actual pin 32
const int ain1 = 9;       //PB1,actual pin 13   
const int ain2 = 10;       //PB2,actual pin 14  
const int pwma = 5;        //PD5,actual pin 9   
const int bin1 = 8;       //PB0,actual pin 12
const int bin2 = 7;       //PD7,actual pin 11
const int pwmb = 6;       //PD6,actual pin 10
//Object sensors
const int irled = 3;       //PD3,actual pin 1
const int leftobj = A0;    //ADC0,actual pin 23 
const int rightobj = A1;    //ADC1,actual pin 24
//Line sensors
const int leftline = A2;    //ADC2,actual pin 25
const int rightline = A3;    //ADC3,actual pin 26
//breakout
const int pin1 = 3;        //PD3,actual pin 1
const int pin2 = A4;        //PC4,actual pin 27
const int pin3 = A5;        //PC5,actual pin 28
const int pin4 = 4;        //PD4,actual pin 2
//object sensor variables: 0 = object there, 1 = no object
int objsensorleft = 0;
int objsensorright = 0;
//line sensor variables, < 200 white line, > 200 black
int linesensorleft = 0;
int linesensorright = 0;
//bot specific values
int freq = 38000;
int leftspeed = 140;
int rightspeed = 140;
int leftthreshold = 200;
int righthreshold = 200;
//whichBot is the serial number in location 0 of the EEPROM
//the bottom of your Butterbot has it's serial number
int whichBot;    
// the setup function runs once when you press reset:
void setup() {                
  // This one line below is vital. Don't ever delete it or the robot could be damaged.
  analogReference(EXTERNAL);
  //initialialize h-bridge pins
  pinMode(standby, OUTPUT);     
  pinMode(ain1, OUTPUT);     
  pinMode(ain2, OUTPUT);     
  pinMode(pwma, OUTPUT);     
  pinMode(bin1, OUTPUT);     
  pinMode(bin2, OUTPUT);     
  pinMode(pwmb, OUTPUT); 
  //initialize object detection pins  
  pinMode(leftobj, INPUT);     
  pinMode(rightobj, INPUT);
  
  //EEPROM.write(0,6);
  whichBot = EEPROM.read(0);
  //override default values for particular robot
  switch(whichBot)
  {
  case 1 :          //Bill's robot
    freq = 27000;            
    leftspeed = 140;
    rightspeed = 180; 
    break;
  case 2 :
    leftspeed = 140;
    rightspeed = 150;
    leftthreshold = 600; 
    break;
   case 3:          //Brij's robot
    break;
   case 4:
    break;
   case 5:
    break;
   case 6 :
    leftspeed = 140;
    rightspeed = 155; 
    break;
 
  default :        //nothing overridden from defaults
     break;
  }
  Serial.begin(115200);
  //Start pulsing infrared led, this is continuous.
  tone(irled,freq);
  //wait 3 seconds for start
  delay(3000);
  //start going forward 3/4 speed
  go();
}

// the loop routine runs over and over again forever:
void loop() {

  //Serial.println("Hello Bill");
  //delay(1000);
  //check if something is there
 
  //check line sensors
  linesensorleft = analogRead(leftline);
  if ( linesensorleft < leftthreshold )
  {
    backupandturnright();
  }
  else
  {
    linesensorright = analogRead(rightline);
    if (linesensorright < righthreshold)
    {
      backupandturnleft();
    }
  }
}
 
void go(void)
{
    digitalWrite(standby, LOW);   //stop
    digitalWrite(ain1, LOW);       //left wheel forward
    digitalWrite(ain2, HIGH); 
    digitalWrite(bin1, LOW);       //right wheel forward
    digitalWrite(bin2, HIGH); 
    analogWrite(pwma, leftspeed);       //0-255 for speed
    analogWrite(pwmb, rightspeed);      //0-255 for speed  
    digitalWrite(standby, HIGH);   //go
}

void backupandturnleft(void)
{
    backup();
    //turn right
    digitalWrite(bin1, LOW);       //right wheel forward
    digitalWrite(bin2, HIGH); 
    delay(200);                    //200ms
    //go forward
    go();
}
  
void backupandturnright(void)
{
    backup();
    //turn right
    digitalWrite(ain1, LOW);       //left wheel forward
    digitalWrite(ain2, HIGH); 
    delay(200);                    //200ms
    //go forward
    go();
}

void backup(void)
{
      //go backwards
    digitalWrite(standby, LOW);    //stop 
    digitalWrite(ain1, HIGH);       //left wheel back
    digitalWrite(ain2, LOW); 
    digitalWrite(bin1, HIGH);       //right wheel back
    digitalWrite(bin2, LOW); 
    digitalWrite(standby, HIGH);   //go
    delay(800);                    //160ms
}
  
void spinleft(void)
{
    //turn right
    digitalWrite(ain1, HIGH);       //left wheel back
    digitalWrite(ain2, LOW); 
    digitalWrite(bin1, LOW);       //right wheel forward
    digitalWrite(bin2, HIGH); 
    delay(200);                    //220ms
}
  
void spinright(void)
{
    //turn right
    digitalWrite(ain1, LOW);       //left wheel forward
    digitalWrite(ain2, HIGH); 
    digitalWrite(bin1, HIGH);       //right wheel back
    digitalWrite(bin2, LOW); 
    delay(200);                    //220ms
}


