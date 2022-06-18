#ifndef PENYX_H
#define PENYX_H


  void stripled(int lednum, int red, int green, int blue);
  void neopixels(int red, int green, int blue);
  byte i2c (int port, int value);

 class Penyx {
  
  private:
   // byte pin;
  
  int speed = 500;
  public:
    Penyx(int speed); //Calls init()
    void init();
    void begin();    
    void xy(int x, int y, bool pen); //command to move Penyx to a grid location
    void gotogrid(int x, int y, bool pen);
    void display(const char*); // print to lcd
    int locx(); // returns the current X location
    int locy(); // returns the current Y location
    void checkradio(); //periodically checks the radio buffer, and executed the location command if any
    void penup();
    void pendown();  
 };

#endif 