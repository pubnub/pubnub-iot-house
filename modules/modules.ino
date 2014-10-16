
#include <Servo.h>

Servo frontDoor;
Servo garageDoor;

int leftWindow = 2;
int rightWindow = 3;
int room = 4;

void setup() {
  
  frontDoor.attach(11); // servo on digital pin 10
  garageDoor.attach(12); 
  
  pinMode(leftWindow, OUTPUT);
  pinMode(rightWindow, OUTPUT);
  pinMode(room, OUTPUT);
  
}

void garage(boolean open){
  
  if(open) {
     garageDoor.write(80);
  } else {
     garageDoor.write(140);
  }
  
}

void door(boolean open){
  
  if(open) {
     frontDoor.write(90);
  } else {
     frontDoor.write(0);
  }
  
}

void loop() {
  
   garage(false);

   digitalWrite(leftWindow, HIGH);
  
   delay(600);
   
   door(false);
   
   delay(600);
   
   digitalWrite(rightWindow, HIGH);
  
   garage(true);
  
   delay(600);
   
   door(true);
   
   
   delay(600);
   digitalWrite(room, HIGH);   
   digitalWrite(leftWindow, LOW);
   digitalWrite(rightWindow, LOW);
   digitalWrite(room, LOW);
}

