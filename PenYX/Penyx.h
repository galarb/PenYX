#ifndef PENYX_H
#define PENYX_H

 #include <Arduino.h>
 #include <config.h>
 #include <Adafruit_PWMServoDriver.h>
 #include <Wire.h>
 #include <Adafruit_NeoPixel.h>
 #include <LiquidCrystal_I2C.h>
 #include <SPI.h>
 #include <nRF24L01.h>
 #include <RF24.h>

  void stripled(int lednum, int red, int green, int blue);
  void neopixels(int red, int green, int blue);
  byte i2c (int port, int value);

 class Penyx {
  
  private:
   // byte pin;
  void penup();
  void pendown();  

  public:
    Penyx(); //Calls init()
    void init();
    void begin();    
    void xy(int x, int y, bool dir);
    void display(const char*);
      
 };

#endif 