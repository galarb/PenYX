/* cpp code for PenYX library
v2.2.3

speed sets the motors control pulses width in uS. typ 700 (fast) - 1700 (slow).
the Radio is connected to Default SPI Pins: CLK, MOSI, MISO = 13, 11, 12
and  CE, CSN = 8, 9
and Default I2C pins : SCL, SDA = 28, 27 

Using the CNC Shield:
pin 8 = En (below the reset button) must be jumped to GND
pin 9 = X- = X+
pin 10 = Y- = Y+
pin 11 = Z+ = Z-
pin 12 = SpnEn
pin 13 = SpnDir

Radio control: the buttons are used as "pen down"

nema 17 Stepper has 1.8deg step angle -> 360/1.8=200
Stepper in Full steps

by gal arbel
2022
*/
 #include "Arduino.h"
 #include "Penyx.h"
 #include <Adafruit_PWMServoDriver.h>
 #include <Wire.h>
 #include <Adafruit_NeoPixel.h>
 #include <LiquidCrystal_I2C.h>
 #include <SPI.h>
 #include <nRF24L01.h>
 #include <RF24.h>
 #include <Servo.h>
static const int stepXPin = 2; 
static const int stepYPin = 3; 
static const int dirXPin = 5; 
static const int dirYPin = 6; 
static const int ylimPin = 10; 
static const int xlimPin = 9; 
static const int servoPin = 11;  

static int xlocation = 0;
static int ylocation = 0;
static int pos = 0;    // stores the servo position
static const byte address[6] = "00001"; //for radio
static int control[4]; // radio message. x, y, btn1, btn2
static char* dispmessage = ""; // to display on LCD

LiquidCrystal_I2C lcd(0x27, 16, 2); // on 0x27 i2c address, 16 by 2
Servo myservo;  
RF24 radio(12, 13);  // create an RF24 object. CE, CSN


Penyx::Penyx(int speed) {
  this->speed = speed; //assign motors speed or - delay
}

void Penyx::begin() {
  Serial.begin(115200);       
  Serial.println("Penyx Initiated");
  pinMode(stepYPin,OUTPUT); 
  pinMode(dirYPin,OUTPUT);
  pinMode(stepXPin,OUTPUT); 
  pinMode(dirXPin,OUTPUT);
  pinMode(ylimPin,INPUT_PULLUP); 
  pinMode(xlimPin,INPUT_PULLUP);

  myservo.attach(servoPin);  
  Serial.println("Receiver Srarted");
  
  /*
  if (!radio.begin()) {
   Serial.println(F("radio hardware not responding!"));
   //while (1) {} // hold program in infinite loop to prevent subsequent errors
   
   }
  else {
    radio.setPALevel(RF24_PA_MIN);
    radio.openReadingPipe(0, address);
    radio.startListening();
    Serial.println("Radio Details:");
    radio.printDetails();
  }
*/
  lcd.init();  
  lcd.begin(0x27, 16, 2);
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("PenYX Initiated!");
  lcd.setCursor(1,1);
  lcd.print("By: Gal Arbel");
  
}


void Penyx::movedxdy(int x, int y) { //steps to each side
  Serial.println("dxdy activated");
  Serial.print("Delta X =");
  Serial.print(x);
  Serial.print("  |  Delta Y =");
  Serial.println(y);
   
 if (y > 0) { //new delta setpoint
   digitalWrite(dirYPin,HIGH); // go CW. Makes 200 pulses for one revolution
   
   for (int c = 0; c < y; c++){
    digitalWrite(stepYPin,HIGH); 
    delayMicroseconds(speed); 
    digitalWrite(stepYPin,LOW);  
    delayMicroseconds(speed); 
    ylocation++;

     if (!digitalRead(ylimPin)) {
       movefromedge('y', 'd');
       Serial.println("limit Y reached"); 
       break;
       }
   } 
 }
  if (y < 0){
    digitalWrite(dirYPin,LOW); // go CCW
    for(int c = 0; c < abs(y) ; c++) {
     digitalWrite(stepYPin,HIGH);
     delayMicroseconds(speed);
     digitalWrite(stepYPin,LOW);
     delayMicroseconds(speed);    
     ylocation--;

     if (!digitalRead(ylimPin)) {
       Serial.println("limit Y reached"); 
       movefromedge('y', 'u');
       break;
       }
     }    
  }

  if (x > 0){
   digitalWrite(dirXPin,HIGH); // go CW. Makes 200 pulses for making one full cycle rotation
   for (int c = 0; c < x; c++){
    digitalWrite(stepXPin,HIGH); 
    delayMicroseconds(speed); 
    digitalWrite(stepXPin,LOW);  
    delayMicroseconds(speed); 
    xlocation++;
    if (!digitalRead(xlimPin)) {
       Serial.println("limit X reached"); 
       movefromedge('x', 'l'); 
        break;
       }
   }
 }
   if (x < 0){
    digitalWrite(dirXPin,LOW); // go CCW
    for(int c = 0; c < abs(x) ; c++) {
     digitalWrite(stepXPin,HIGH);
     delayMicroseconds(speed);
     digitalWrite(stepXPin,LOW);
     delayMicroseconds(speed);    
     xlocation--;  
     if (!digitalRead(xlimPin)) {
       Serial.println("limit X reached"); 
       movefromedge('x', 'r'); 
        break;
       }
  }
 }
 
/*  Serial.println("- - - - Updated Location details- - - - ");
  Serial.print("xlocation =");
  Serial.print(xlocation);
  Serial.print("  |  ylocation =");
  Serial.println(ylocation);*/
  lcd.clear();
  lcd.print(" X  |  Y  | PEN");
  lcd.setCursor(0, 1);
  lcd.print("    |     |    ");
  lcd.setCursor(0, 1);
  lcd.print(xlocation);
  lcd.setCursor(6, 1);
  lcd.print(ylocation);  
}

void Penyx::moveabsxy(int x, int y) { //setpoint to grid location X, Y. pen up = False
  Serial.println("goto grid activated!");
  Serial.print("Absolute X =");
  Serial.print(x);
  Serial.print("     Absolute Y =");
  Serial.println(y);
  int modx = x - xlocation;
  int mody = y-ylocation;
  movedxdy(modx, mody);
 
  Serial.println("- - - - Updated Location details- - - - ");
  Serial.print("xlocation =");
  Serial.print(xlocation);
  Serial.print("  |  ylocation =");
  Serial.println(ylocation);
  lcd.print(" X  |  Y  | PEN");
  lcd.setCursor(0, 1);
  lcd.print("    |     |    ");
  lcd.setCursor(0, 1);
  lcd.print(xlocation);
  lcd.setCursor(6, 1);
  lcd.print(ylocation);
     
}

void Penyx::pendown () {
   Serial.println("Pen Down");
   lcd.setCursor(11, 1);
   lcd.print("DRAW!"); 
   myservo.write(100);    
   }

void Penyx::penup () {
   Serial.println("Pen Up");
   lcd.setCursor(12, 1);
   lcd.print("OFF "); 
   myservo.write(120);    
  }

void Penyx::penang (int ang) {
   Serial.println("angle chosen");
   myservo.write(ang);    
  }

void Penyx::display(const char* dispmessage){
  lcd.clear();
  lcd.backlight();
  lcd.printstr(dispmessage); 
  delay(500);
}
int Penyx::locx(){
 int x = xlocation ;  
 return(x);
}
int Penyx::locy(){
  int y = ylocation;
  return(y);
}

void Penyx::checkradio() {
  
 // the data if available in buffer
 if (radio.available()) {   // If the NRF240L01 module received data
    radio.read(&control, sizeof(control));
    Serial.print("control[]=");
    Serial.print(control[0]);
    Serial.print("  ");
    Serial.print(control[1]);
    Serial.print("  ");    
    Serial.print(control[2]);  
    Serial.print("  ");    
    Serial.println(control[3]);  
    moveabsxy(control[0], control[1]);
  }
  else {Serial.println("no radio available");}  
}     
void Penyx::movefromedge(char axis, char direction) {
 if (axis == 'y') {
   if (direction == 'u'){
    digitalWrite(dirYPin,HIGH); // go CW. Makes 200 pulses for one revolution
    ylocation =+ 35;
    Serial.println("hitting upper edge!");
   }
   if (direction == 'd'){
    digitalWrite(dirYPin,LOW); // go CCW. Makes 200 pulses for one revolution
    ylocation =- 35;
    Serial.println("hitting lower edge!");

   }
   for (int c = 0; c < 35; c++){// measured value to move from edge
    digitalWrite(stepYPin,HIGH); 
    delayMicroseconds(1500); //slowing down
    digitalWrite(stepYPin,LOW);  
    delayMicroseconds(1500); 
   } 
 }
 if (axis == 'x') {
   if (direction == 'l'){
    digitalWrite(dirXPin,LOW); 
    xlocation =- 35;
    Serial.println("hitting right edge!");
   }
   if (direction == 'r'){
    digitalWrite(dirXPin,HIGH); 
    xlocation =+ 35;
    Serial.println("hitting left edge!");

   }
   for (int c = 0; c < 35; c++){// measured value to move from edge
    digitalWrite(stepXPin,HIGH); 
    delayMicroseconds(1500); 
    digitalWrite(stepXPin,LOW);  
    delayMicroseconds(1500); 
   } 
 }
}