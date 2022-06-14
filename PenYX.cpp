#include "Arduino.h"
 #include "Penyx.h"
 #include <Adafruit_PWMServoDriver.h>
 #include <Wire.h>
 #include <Adafruit_NeoPixel.h>
 #include <LiquidCrystal_I2C.h>
 
const int LEDs = 33;
const int LED_NUM = 12;
const int stepYPin = 3; 
const int dirYPin = 4;  
const int stepXPin = 7; 
const int dirXPin = 8;  
byte speed = 500; // sets the motors delay in uS. typ 300-4000
int xlocation = 0;
int ylocation = 0;
char* dispmessage = "";
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LEDs, NEO_GRB + NEO_KHZ800);
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);//object to represent PCA9685 at default I2C address  (0x40, more extensions up to 0x60)
LiquidCrystal_I2C lcd(0x27, 16, 2); // on 0x27 i2c address, 16 by 2

Penyx::Penyx() {
 // this->pin = pin; example of var assignement from the Roboghost().. now empty
  init();
}
void Penyx::init() {
  Serial.begin(115200);       
  Serial.println("Init");

}
void Penyx::begin() {
  Serial.begin(115200);       
  Serial.println("begin");
  pinMode (LEDs,OUTPUT);
  pinMode(stepYPin,OUTPUT); 
  pinMode(dirYPin,OUTPUT);
  pinMode(stepXPin,OUTPUT); 
  pinMode(dirXPin,OUTPUT);
  //Wire.begin(); // i2c on default SCL, SDA pins: 28, 27  starts auto on defauls by LiquidCrystal_I2C.
  pca9685.begin();  
  pca9685.setPWMFreq(500);   // Set PWM Frequency(24-1526Hz). arduino PWM works at 500Hz
  strip.begin();
  strip.setBrightness(200); //(up to 255)
  strip.clear(); 
  lcd.begin(0x27, 16, 2);
  lcd.backlight();
  lcd.setCursor(5,0);
  lcd.print("PenYX");
  lcd.setCursor(4,1);
  lcd.print("Started");
}

void Penyx::xy(int x, int y, bool pen) { //steps to each side, pen up = False
 
  Serial.println("xy activated");
  Serial.print("Delta X =");
  Serial.print(x);
  Serial.print("     Delta Y =");
  Serial.print(y);
  Serial.print("     pen  =");
  Serial.println(pen);
  
 if (y > ylocation) { //new setpoint
   digitalWrite(dirYPin,HIGH); // go CW. Makes 200 pulses for making one full cycle rotation
   //Stepper has 1.8deg step angle -> 360/180=200
   for (int c = 0; c < y; c++){
    digitalWrite(stepYPin,HIGH); 
    delayMicroseconds(speed); 
    digitalWrite(stepYPin,LOW);  
    delayMicroseconds(speed); 
    ylocation++;
   } 
 }
 else if (y < ylocation){
    digitalWrite(dirYPin,LOW); // go CCW
    for(int c = 0; c < abs(y) ; c++) {
     digitalWrite(stepYPin,HIGH);
     delayMicroseconds(speed);
     digitalWrite(stepYPin,LOW);
     delayMicroseconds(speed);    
     ylocation--;
     }    
  }
  if (x > xlocation){
   digitalWrite(dirXPin,HIGH); // go CW. Makes 200 pulses for making one full cycle rotation
   for (int c = 0; c < x; c++){
    digitalWrite(stepXPin,HIGH); 
    delayMicroseconds(speed); 
    digitalWrite(stepXPin,LOW);  
    delayMicroseconds(speed); 
    xlocation++;
    Serial.println(xlocation);
   }
 }
  else if (x < xlocation){
    digitalWrite(dirXPin,LOW); // go CCW
    for(int c = 0; c < abs(x) ; c++) {
     digitalWrite(stepXPin,HIGH);
     delayMicroseconds(speed);
     digitalWrite(stepXPin,LOW);
     delayMicroseconds(speed);    
     xlocation--;  
     Serial.println(xlocation);
  }
 }
 
  delay(1000);
  Serial.println("- - - - Updated Location details- - - - ");
  Serial.print("xlocation =");
  Serial.println(xlocation);
  Serial.print("ylocation =");
  Serial.println(ylocation);
  lcd.clear();
  lcd.print(" X  |  Y  | PEN");
  lcd.setCursor(0, 1);
  lcd.print("    |     |    ");
  lcd.setCursor(0, 1);
  lcd.print(xlocation);
  lcd.setCursor(6, 1);
  lcd.print(ylocation);
  if (pen) {
    pendown();
  }
  if (!pen) {
   penup();
  }
 
}

void Penyx::pendown () {
   Serial.println("Pen Down");
   lcd.setCursor(11, 1);
   lcd.print("DRAW!");     
   }

void Penyx::penup () {
   Serial.println("Pen Up");
   lcd.setCursor(12, 1);
   lcd.print("OFF"); 
  }
void Penyx::display(const char* dispmessage){
  lcd.clear();
  lcd.backlight();
  lcd.printstr(dispmessage); 
  delay(500);
}
void stripled (int lednum, int red, int green, int blue) {
      strip.setPixelColor(lednum, strip.Color(red, green, blue));
      strip.show();
      delay(50);
}
void neopixels (int red, int green, int blue) {
  for (int i = 0; i < LED_NUM; i++) {
      strip.setPixelColor (i, strip.Color(red, green, blue));
      strip.show();
      delay(50);
  }
}


byte i2c (int port, int value) { //writes a value to the i2c port
     value = map(value, 0, 255, 0, 4095);
     pca9685.setPWM(port, 0, value);    // Write to PCA9685. 
}

