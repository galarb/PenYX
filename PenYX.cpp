/* cpp code for PenYX library

speed sets the motors control pulses width in uS. typ 300-4000.
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
const int stepXPin = 4; 
const int stepYPin = 3; 
const int dirXPin = 7; 
const int dirYPin = 6;  
const int servoPin = 10;  

int xlocation = 0;
int ylocation = 0;
int pos = 0;    // stores the servo position
const byte address[6] = "00001"; //for radio
int control[4]; // radio message. x, y, btn1, btn2
char* dispmessage = ""; // to display on LCD

//LiquidCrystal_I2C lcd(0x27, 16, 2); // on 0x27 i2c address, 16 by 2
Servo myservo;  
RF24 radio(8, 9);  // create an RF24 object. CE, CSN


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
  //Wire.begin(); // i2c on default SCL, SDA pins: 28, 27  starts auto on defauls by LiquidCrystal_I2C.
  myservo.attach(servoPin);  // attaches the servo to the pin
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
 /* lcd.begin(0x27, 16, 2);
  lcd.backlight();
  lcd.setCursor(5,0);
  lcd.print("PenYX");
  lcd.setCursor(3,1);
  lcd.print("Initiated");*/
  

}


void Penyx::dxdy(int x, int y, bool pen) { //steps to each side, pen up = False
 
  Serial.println("dxdy activated");
  Serial.print("Delta X =");
  Serial.print(x);
  Serial.print("  |  Delta Y =");
  Serial.print(y);
  Serial.print("  |  pen  =");
  Serial.println(pen);
  
 if (y > 0) { //new delta setpoint
   digitalWrite(dirYPin,HIGH); // go CW. Makes 200 pulses for one revolution
   //Stepper has 1.8deg step angle -> 360/1.8=200
   //Stepper in Full steps
   for (int c = 0; c < y; c++){
    digitalWrite(stepYPin,HIGH); 
    delayMicroseconds(speed); 
    digitalWrite(stepYPin,LOW);  
    delayMicroseconds(speed); 
    ylocation++;
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
     }    
  }

  if (x > 0){
   digitalWrite(dirXPin,HIGH); // go CW. Makes 200 pulses for making one full cycle rotation
    Serial.println("dx ++");

   for (int c = 0; c < x; c++){
    digitalWrite(stepXPin,HIGH); 
    delayMicroseconds(speed); 
    digitalWrite(stepXPin,LOW);  
    delayMicroseconds(speed); 
    xlocation++;
   // Serial.println(xlocation);
   }
 }
   if (x < 0){
    digitalWrite(dirXPin,LOW); // go CCW
        Serial.println("dx --");

    for(int c = 0; c < abs(x) ; c++) {
     digitalWrite(stepXPin,HIGH);
     delayMicroseconds(speed);
     digitalWrite(stepXPin,LOW);
     delayMicroseconds(speed);    
     xlocation--;  
   //  Serial.println(xlocation);
  }
 }
 
  delay(1000);
  Serial.println("- - - - Updated Location details- - - - ");
  Serial.print("xlocation =");
  Serial.print(xlocation);
  Serial.print("  |  ylocation =");
  Serial.println(ylocation);
  /*lcd.clear();
  lcd.print(" X  |  Y  | PEN");
  lcd.setCursor(0, 1);
  lcd.print("    |     |    ");
  lcd.setCursor(0, 1);
  lcd.print(xlocation);
  lcd.setCursor(6, 1);
  lcd.print(ylocation);*/
  if (pen) {
    pendown();
  }
  if (!pen) {
   penup();
  }
  delay(1000);
   
}

void Penyx::absxy(int x, int y, bool pen) { //setpoint to grid location X, Y. pen up = False
 
  Serial.println("gotogrid activated!");
  Serial.print("Absolute X =");
  Serial.print(x);
  Serial.print("     Absolute Y =");
  Serial.print(y);
  Serial.print("     pen  =");
  Serial.println(pen);
  
 if (y > ylocation) { //new setpoint
   digitalWrite(dirYPin,HIGH); // go CW. Makes 200 pulses for making one full cycle rotation
   //Stepper has 1.8deg step angle -> 360/1.8=200
   for (int c = ylocation; c < y; c++){ // a loop to send pulses to the Stepper.
    digitalWrite(stepYPin,HIGH); 
    delayMicroseconds(speed); 
    digitalWrite(stepYPin,LOW);  
    delayMicroseconds(speed); 
    ylocation++;
   } 
 }
 else if (y < ylocation){
    digitalWrite(dirYPin,LOW); // go CCW
    int tempy = ylocation;    
    for(int c = y; c < tempy; c++) {
     digitalWrite(stepYPin,HIGH);
     delayMicroseconds(speed);
     digitalWrite(stepYPin,LOW);
     delayMicroseconds(speed);    
     ylocation--;
     }    
  }
  if (x > xlocation){
   digitalWrite(dirXPin,HIGH); // go CW. Makes 200 pulses for making one full cycle rotation
   for (int c = xlocation; c < x; c++){
    digitalWrite(stepXPin,HIGH); 
    delayMicroseconds(speed); 
    digitalWrite(stepXPin,LOW);  
    delayMicroseconds(speed); 
    xlocation++;
   // Serial.println(xlocation);
   }
 }
  else if (x < xlocation){
    digitalWrite(dirXPin,LOW); // go CCW
    int tempx = xlocation;
    for(int c = x; c < tempx ; c++) {
     digitalWrite(stepXPin,HIGH);
     delayMicroseconds(speed);
     digitalWrite(stepXPin,LOW);
     delayMicroseconds(speed);    
     xlocation--;  
   //  Serial.println(xlocation);
  }
 }
 
  delay(1000);
  Serial.println("- - - - Updated Location details- - - - ");
  Serial.print("xlocation =");
  Serial.print(xlocation);
  Serial.print("  |  ylocation =");
  Serial.println(ylocation);
 /* lcd.clear();
  lcd.print(" X  |  Y  | PEN");
  lcd.setCursor(0, 1);
  lcd.print("    |     |    ");
  lcd.setCursor(0, 1);
  lcd.print(xlocation);
  lcd.setCursor(6, 1);
  lcd.print(ylocation);*/
  if (pen) {
    pendown();
  }
  if (!pen) {
   penup();
  }
  delay(1000);
   
}

void Penyx::pendown () {
   Serial.println("Pen Down");
  // lcd.setCursor(11, 1);
   //lcd.print("DRAW!"); 
   myservo.write(150);    
   }

void Penyx::penup () {
   Serial.println("Pen Up");
   //lcd.setCursor(12, 1);
   //lcd.print("OFF "); 
   myservo.write(10);    

  }
void Penyx::display(const char* dispmessage){
  /*lcd.clear();
  lcd.backlight();
  lcd.printstr(dispmessage); */
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
    absxy(control[0], control[1], control[2] or control[3]);
  }
  else {Serial.println("no radio available");}  
}     