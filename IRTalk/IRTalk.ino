/*
  Main for ButterBot
 
 README notes:
 1. ButterBot uses the 3.3V Arduino Pro Mini bootloader. 
 2. The pin numbers used by the Arduino IDE are different than the pins numbers 
 on the actual chip. I have documented it here so, if you reference the ButterBot schematic, it will
 make sense.
 2. The variable called "freq" is the pulsing frequency of the ir led. It runs at 38kHz by using the tone
 library function. To make it more or less sensitive, you can either adjust the trimpot (max 5k ohms) on the 
 front of the robot (clockwise is higher) or you can adjust the frequency in the tone command
 down from 38kHz until it works the way you want.
 3. The Sparkfun Arduino programmer only supplies 50mA and this isn't enough to drive the irled or the motors, so
 they won't run when it is connected. This also means the Serial Monitor won't work if the motors or irled are
 on, i.e. Serial.print(s) won't work if the motors or irled are on.
 
 Motors:    left        right
 ain1  ain2  bin1  bin2
 forward    0    1      0    1
 backward   1    0      1    0
 
 pwma - left  motor speed 0-255
 pwmb - right motor speed 0-255
 
 standby - motors on or off (high is on)
 
 The motors and drive for each wheel are not perfectly the same. If you want the robot to go straight, you have to
 experiment to see what pwm values work. For example, if the robot turns toward the right, the right wheel is going
 slower. Try increasing the pwm for the right wheel, 10 at a time, until the robot goes straight.
 
 Linesensors:
 They return a value between 0-1024. 1024 is no reflection (black or table edge), 0 is purely reflective (white)
 
 objectsensors:
 when they return 0, something is there
 when they return 1, nothing is there
 A trick here is the vary the frequency in steps to get a distance measurement, for example.
 38kHz get a 0, see it
 35kHz get a 0, see it
 30kHz get a 1, aha, must be at a distance (tested by you)
 Also, since there is a left and right sensor. If one side detects an object and the other side doesn't, you can turn to avoid, or
 turn to approach, the object detected.
 
 */
#include <EEPROM.h>

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
//the bottom of your Butterbot has it's serial number. The eeprom is used to save a ButterBots serial number. That way you can have one source file for multiple robots.
//You read the bots number from eeprom and execute code based on the unique Bot number.
int whichBot;      
//object sensor variables: 0 = object there, 1 = no object
int objsensorleft = 0;
int objsensorright = 0;
//line sensor variables, < 200 white line, > 200 black
int linesensorleft = 0;
int linesensorright = 0;
//bot specific values
int freq = 38000;
int leftmotorspeed = 200;
int rightmotorspeed = 200;
int leftthreshold = 200;
int righthreshold = 200;
//ir communication
int state_for_bot_2;
int state_for_bot_4;
const byte SYNC1 = 0x82;    //start command bytes with the first bit set to 1
const byte SYNC2 = 0x83;
const byte GO = 0x84;
byte result;

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
  pinMode(pin4,OUTPUT);
  
  //EEPROM.write(0, 4);
  whichBot = EEPROM.read(0);
  //override default values for particular robot
  switch(whichBot)
  {
  case 1 :
    freq = 27000;            
    leftmotorspeed = 200;
    rightmotorspeed = 250; 
    break;
  case 2 :
    leftmotorspeed = 140;
    rightmotorspeed = 150; 
    break;

  default :        //nothing overridden from defaults
    break;
  }
  Serial.begin(115200);
  //Start pulsing infrared led, this is continuous.
  //tone(irled,freq);
  //wait 3 seconds for start
  delay(3000);
  //start going forward 3/4 speed
  //go();
}

// the loop routine runs over and over again forever:
void loop() {

  state_for_bot_2 = 0;
  state_for_bot_4 = 0;

  if (whichBot == 4)
  {
    while(1)
    {
      switch(state_for_bot_4)
      {
      case 0 :
        send_byte(SYNC1);    //480ms
        delay(100);
        if (polling(500))
        {
          result = recv_byte();
          if (result == SYNC2)
          {
            state_for_bot_4 = 1;
          }
        }
        break;
      case 1 :
        send_byte(GO);    //480ms
        delay(100);
        break;
      default:
        break;
      }  
    }
  }
  if (whichBot == 2)
  {
    while(1)
    {
      switch(state_for_bot_2)
      {
      case 0 :
        if (polling(0))
        {
          result = recv_byte();
          if (result == SYNC1)
          {
            state_for_bot_2 = 1;
          }
        }
        break;
      case 1 :
        delay(200);
        send_byte(SYNC2);
        if (polling(400))
        {
          result = recv_byte();
          if (result == GO)
            state_for_bot_2 = 2;
        }
        break;
      case 2 :
        go();
        delay(100);
        backupandturnright();
        begin_roaming();
        break;
      default :
        break;        
      }
    }
  }
}

void begin_roaming(void)
{
  //Serial.println("Hello Bill");
  //delay(1000);
  //check if something is there
  while(1)
  {  
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
}

boolean polling(int time)
{

  if (time == 0)
  {
    while(1)
    {
      objsensorleft = digitalRead(leftobj);
      if (objsensorleft == 0)
      {
        return true;
      }
      delayMicroseconds(50);

    }
  }
  else
  {
    while(1)
    {
      objsensorleft = digitalRead(leftobj);
      if (objsensorleft == 0)
      {
        return true;
      }
      else
      {
        time -=2;
        if (time <= 0)
          return false;
        delay(2);
      }
    }
  }    
}

void send_byte(byte bits) 
{
  for (int i = 7; i >= 0; i--)
  {
    if (((1 << i) & bits) == (1 << i))
    {
      tone(irled,freq);
    }
    else
    {
      noTone(irled);
    }
    delay(60);
  }
  noTone(irled);
}

byte recv_byte(void)
{
  byte gatherbits;
  int i;

  gatherbits = 0;

  digitalWrite(pin4,HIGH);
  for (i=7;(i>=0);i--)
  {
    objsensorleft = digitalRead(leftobj);
    if (objsensorleft == 0)
    {
      digitalWrite(pin4,LOW);
      gatherbits |= 1 << i; 
    }
    delay(60);
    digitalWrite(pin4,HIGH);
  }
  return gatherbits;
}

void go(void)
{
  digitalWrite(standby, LOW);   //stop
  digitalWrite(ain1, LOW);       //left wheel forward
  digitalWrite(ain2, HIGH); 
  digitalWrite(bin1, LOW);       //right wheel forward
  digitalWrite(bin2, HIGH); 
  analogWrite(pwma, leftmotorspeed);       //0-255 for speed
  analogWrite(pwmb, rightmotorspeed);      //0-255 for speed  
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
  delay(200);                    //160ms
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




