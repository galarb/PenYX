#ifndef PENYX_H
#define PENYX_H

 class Penyx {
  
  private:
    int speed = 500;
    void movefromedge(char axis, char direction);
    
  public:
    Penyx(int speed); 
    void begin();   
    void movedxdy(int x, int y); //move to delta X, delta Y, 
    void moveabsxy(int x, int y);//move to absolute grid location
    void display(const char*); // print to Penyx lcd
    int locx(); // returns the current X location
    int locy(); // returns the current Y location
    void checkradio(); //periodically checks the radio buffer, and executed the location command if any
    void penup(); //manual pen operation
    void pendown();  
    void penang(int ang); //move the servo to desired
 };

#endif 