/* /////               PenYX Software       ///////////////////////
             RGB Strip software using Arduino

  Wire.begin(25, 27); // alternative pins for i2c: SDA, SCL

By: Gal Arbel
2022
*/
#include "Penyx.h"

Penyx penyx; //create an Object. Init PenYX!
const unsigned int MAX_MESSAGE_LENGTH = 64;

void setup() {
  penyx.begin();

}

void loop() {  
 delay(1000);
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
      penyx.display(message);
          
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
       case 'i': // use the i2c protocol - Write to PCA9685 
        i2c(command[1], command[2]); 
        break;   
       case 's': // turn on A single Strip led (LEDnumber, R, G, B)
        stripled (command[1], command[2], command[3], command[4]);
        break;
       case 'n': // turn on all strip leds (R, G, B)
        neopixels (command[1], command[2], command[3]);
        break;
       case 'x':
        penyx.xy(command[1], command[2], command[3]);
        break; 
       
        message_pos = 0;     //Reset for the next message
      }
   }
   delay (30);
 }
     

}
