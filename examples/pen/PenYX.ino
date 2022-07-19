/* /////               PenYX Software       ///////////////////////
    An XY Robotic Pen  
  
  //Wire.begin(25, 27); // alternative pins for i2c: SDA, SCL

By: Gal Arbel
2022
*/

#include "Penyx.h"
#include "circle.hpp"
#include "line.hpp"

const unsigned int MAX_MESSAGE_LENGTH = 64;

Penyx penyx(1300); //create an Object. Init PenYX at a selected speed
Circle cyrcle(penyx);
Line line(penyx);

void setup() {
  penyx.begin();
  //line.Draw(200, 400, 2, 5);
}

void loop() {  
penyx.keypad();
delay(10);

 //Serial.println(" - - - Starting Loop - - - ");
 //Serial.print("X Location= ");
 //Serial.println(penyx.locx()); 
 //Serial.print("Y Location= ");
 //Serial.println(penyx.locy()); 
 //delay(1000);

 //This is the CLI Functionality
 // You can add cases with new functionalities 
 while (Serial.available() > 0) {
   char message[MAX_MESSAGE_LENGTH];
   static unsigned int message_pos = 0;
   char inByte = Serial.read();   //Read the next available byte in the serial receive buffer
    if ( inByte != '\n' && (message_pos < MAX_MESSAGE_LENGTH - 1) )
     {
     message[message_pos] = inByte;  //Add the incoming byte to our message
     message_pos++;
     }
     //Full message received...
     else
     {
      message[message_pos] = '\0';     //Add null character to string
      Serial.println(message);     //echo the message to terminal
      //penyx.display(message);
          
      int command[4];
      int argindex = 0;
      char cmd;
      char delim[] = " ";
	     char tmpmsg[MAX_MESSAGE_LENGTH];
       strcpy(tmpmsg,message);
       message_pos = 0;
       message[message_pos] = '\0';     //Add null character to string

        char *ptr = strtok(tmpmsg, delim);
	      while(ptr != NULL)
	       {
          Serial.println(ptr);

          if (argindex == 0) {
            cmd = ptr[0];
          }
          command[argindex] = atoi(ptr);   
          //Serial.println(command[argindex]);
          argindex++;  
		      ptr = strtok(NULL, delim);
	       } 

      switch (cmd) {
       case 'h': //Set port to HIGH
        pinMode(command[1],OUTPUT);
        digitalWrite(command[1],HIGH);
        Serial.print("Pin");
        Serial.print(command[1]);
        Serial.println(" = HIGH");
        penyx.display("Set Pin to HIGH");
           
        delay(1000);
        
        break;
       case 'l': // Set port to LOW
        pinMode(command[1],OUTPUT);
        digitalWrite(command[1],LOW);
        Serial.print("Pin");
        Serial.print(command[1]);
        Serial.println(" = LOW");
        penyx.display("Set Pin to LOW");
        delay(1000);
        break;
       case 'x':
        penyx.movedxdymm(command[1], command[2]);
        break; 
       case 'g':
        penyx.moveabsxy(command[1], command[2]);
        break; 
       case 'p':
        penyx.penup();
        break;
       case 'o':
        penyx.pendown();
        break;  
       case 'a':
        penyx.penang(command[1]);
        break;
         
        message_pos = 0;     //Reset for the next message
      }
   }
   delay (30);
 }
     

}
