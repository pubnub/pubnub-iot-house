
#include <Servo.h>

Servo frontDoor;
Servo garageDoor;

int leftWindow = 2;
int rightWindow = 3;
int room = 4;

void setup() {

  
  frontDoor.attach(10); // servo on digital pin 10
  garageDoor.attach(11); 
  
  pinMode(leftWindow, OUTPUT);
  pinMode(rightWindow, OUTPUT);
  pinMode(room, OUTPUT);
  
}

void loop() {

   digitalWrite(leftWindow, HIGH);
  
   frontDoor.write(45);
   delay(1000);
   
   digitalWrite(rightWindow, HIGH);
   
   frontDoor.write(135);
   delay(1000);
   
   digitalWrite(room, HIGH);
   
   garageDoor.write(45);
   delay(1000);
   
   digitalWrite(leftWindow, LOW);
   digitalWrite(rightWindow, LOW);
   digitalWrite(room, LOW);
   
   garageDoor.write(135);
   delay(1000);
   
}

