#include <EEPROM.h>

/*
  Main for ButterBot
  
  README notes:
  1. ButterBot uses the Arduino Pro Mini bootloader. The Arduino pins
  actually are specified different than the actual pins on the microcontroller.
  I have documented it here so, if you reference the ButterBot schematic, it will
  make sense.
  2. The variable "freq" is the pulsing frequency of the ir led. The two ir detectors
  are so sensitive, the optimum value of 38kHz just turns them on all the time. You may have 
  to adjust the frequency down until the ir detectors work the way you want.
*/ 
const int freq = 38000;
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
//These are values that are tweaked for each robot
//The speeds are for the motors
int leftspeed = 140;
int rightspeed = 140;
int leftthreshold = 200;
int righthreshold = 200;
// the setup routine runs once when you press reset:
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
  //initialize speedy serial port  
  Serial.begin(115200);
  //Start pulsing of infrared led.
  tone(irled,freq);
  
  int whichBot = EEPROM.read(0);
  switch(whichBot)
  {
     case 1:          //Bill's robot
       rightspeed = 180;
       break;
     case 2:
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
     case 6:
       leftspeed = 140;
       rightspeed = 155; 
       break;
     default:
      break; 
   }
  //wait 3 seconds for start
  delay(3000);
  //start going forward 3/4 speed
  go();
}

// the loop routine runs over and over again forever:
void loop() {

  //check if opponent is there
  objsensorleft = digitalRead(leftobj);
  objsensorright = digitalRead(rightobj);
  if (objsensorleft == 0)
  {
     //do something 
  }
  //check line sensors
  linesensorleft = analogRead(leftline);
  linesensorright = analogRead(rightline);
 
  if (linesensorleft > leftthreshold && linesensorright > righthreshold)
  {
    digitalWrite(standby, LOW);   //hit perpindicular line, stop
    while(1) ;    
  }
  if ( linesensorleft > leftthreshold )
  {
    turnleft();
  }
  else
  {
    if (linesensorright > righthreshold)
    {
      turnright();
    }
    else
    {
      digitalWrite(ain2, HIGH);     //cancel turning, go forward
      digitalWrite(bin2, HIGH); 
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
    analogWrite(pwma, leftspeed);       //0-255 speed
    analogWrite(pwmb, rightspeed);       //right wheel needs to go a little faster
    digitalWrite(standby, HIGH);   //go
}

void turnright(void)
{
    //turn right
    digitalWrite(bin2, LOW);       //stop right wheel, left wheel keeps going 
    delay(80);                    //220ms
}
  
void turnleft(void)
{
    //turn right
    digitalWrite(ain2, LOW);       //stop left wheel, right wheel keeps going 
    delay(80);                    //220ms
}

  
  

