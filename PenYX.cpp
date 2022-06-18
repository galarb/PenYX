/* cpp code for PenYX library

speed sets the motors delay in uS. typ 300-4000
using Default SPI Pins: CLK, MOSI, MISO = 13, 11, 12
and Default I2C pins : SCL, SDA = 28, 27 

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

const int LEDs = 33;
const int LED_NUM = 12;
const int stepYPin = 2; 
const int dirYPin = 3;  
const int stepXPin = 7; 
const int dirXPin = 5;
const int servoPin = 6;  
int xlocation = 0;
int ylocation = 0;
int pos = 0;    // stores the servo position
const byte address[6] = "00001"; //for radio
int control[4]; // radio message. x, y, btn1, btn2

char* dispmessage = ""; // to display on LCD

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LEDs, NEO_GRB + NEO_KHZ800);
Adafruit_PWMServoDriver pca9685 = Adafruit_PWMServoDriver(0x40);//object to represent PCA9685 at default I2C address  (0x40, more extensions up to 0x60)
LiquidCrystal_I2C lcd(0x27, 16, 2); // on 0x27 i2c address, 16 by 2
Servo myservo;  
RF24 radio(9,10);  // create an RF24 object. CE, CSN


Penyx::Penyx(int speed) {
  this->speed = speed; //assign motors speed
}

void Penyx::begin() {
  Serial.begin(115200);       
  Serial.println("Penyx Initiated");
  pinMode (LEDs,OUTPUT);
  pinMode(stepYPin,OUTPUT); 
  pinMode(dirYPin,OUTPUT);
  pinMode(stepXPin,OUTPUT); 
  pinMode(dirXPin,OUTPUT);
  //Wire.begin(); // i2c on default SCL, SDA pins: 28, 27  starts auto on defauls by LiquidCrystal_I2C.
  myservo.attach(servoPin);  // attaches the servo to the pin
  pca9685.begin();  
  pca9685.setPWMFreq(500);   // Set PWM Frequency(24-1526Hz). arduino PWM works at 500Hz
  strip.begin();
  strip.setBrightness(200); //(up to 255)
  strip.clear(); 
  Serial.println("Receiver Srarted");
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

  lcd.begin(0x27, 16, 2);
  lcd.backlight();
  lcd.setCursor(5,0);
  lcd.print("PenYX");
  lcd.setCursor(3,1);
  lcd.print("Initiated");


}


void Penyx::dxdy(int x, int y, bool pen) { //steps to each side, pen up = False
 
  Serial.println("xy activated");
  Serial.print("Delta X =");
  Serial.print(x);
  Serial.print("     Delta Y =");
  Serial.print(y);
  Serial.print("     pen  =");
  Serial.println(pen);
  
 if (y > ylocation) { //new setpoint
   digitalWrite(dirYPin,HIGH); // go CW. Makes 200 pulses for making one full cycle rotation
   //Stepper has 1.8deg step angle -> 360/1.8=200
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
  Serial.print(xlocation);
  Serial.print("  |  ylocation =");
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
    Serial.println(xlocation);
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
     Serial.println(xlocation);
  }
 }
 
  delay(1000);
  Serial.println("- - - - Updated Location details- - - - ");
  Serial.print("xlocation =");
  Serial.print(xlocation);
  Serial.print("  |  ylocation =");
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
  delay(1000);
   
}

void Penyx::pendown () {
   Serial.println("Pen Down");
   lcd.setCursor(11, 1);
   lcd.print("DRAW!"); 
   myservo.write(150);    
   }

void Penyx::penup () {
   Serial.println("Pen Up");
   lcd.setCursor(12, 1);
   lcd.print("OFF "); 
   myservo.write(10);    

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
    absxy(control[0], control[1], control[2] or control[3]);
  }
 // else {Serial.println("no radio available");}  
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

