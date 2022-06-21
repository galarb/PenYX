#ifndef PENYX_H
#define PENYX_H

 class Penyx {
  
  private:
    int speed = 500;
  public:
    Penyx(int speed); 
    void begin();    //must be called during setup()
    void dxdy(int x, int y, bool pen); //delta X, delta Y, 
    void absxy(int x, int y, bool pen);//absolute grid location
    void display(const char*); // print to Penyx lcd
    int locx(); // returns the current X location
    int locy(); // returns the current Y location
    void checkradio(); //periodically checks the radio buffer, and executed the location command if any
    void penup(); //manual pen operation
    void pendown();  
 };

#endif 