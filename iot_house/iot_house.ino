#include <SPI.h>
#include <EthernetV2_0.h>
#include <PubNub.h>
#include <string.h>
#include <Servo.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  

Servo frontDoor;
Servo garageDoor;

int lightLeft =7;
int lightRight = 6;
int lightRoom = 5;
int lightGarage = 4;

char pubkey[] = "demo"; 
char subkey[] = "demo";
char channel[] = "pubnub_iot_house";

int i;

EthernetClient *client;

#define W5200_CS  10
#define SDCARD_CS 4

void setup() {
  
  reset();

  pinMode(SDCARD_CS,OUTPUT);
  digitalWrite(SDCARD_CS,HIGH);//Deselect the SD card
  
  Serial.begin(9600);
  
  frontDoor.attach(14);
  garageDoor.attach(15); 
  
  pinMode(lightLeft, OUTPUT);
  pinMode(lightRight, OUTPUT);
  pinMode(lightRoom, OUTPUT);
  pinMode(lightGarage, OUTPUT);

  delay(5000);
  
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    error();
    for(;;)
      ;
  } else {
    Serial.print("success: ");  
    Serial.println(Ethernet.localIP());
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);

  Serial.println("Ethernet set up");

  PubNub.begin(pubkey, subkey);
  Serial.println("PubNub set up");
  
  demo();
  
}

void light(int ledPin, boolean on) {
  
  Serial.println("led");

  if(on) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  
}

void garage(boolean open){

  Serial.println("garage");
  
  if(open) {
     garageDoor.write(0);
  } else {
     garageDoor.write(90);
  }
  
}

void door(boolean open){
  
  Serial.println("door");
  
  if(open) {
     frontDoor.write(90);
  } else {
     frontDoor.write(0);
  }
  
}

void reset() {
  garage(false);
  door(false);
  light(lightLeft, false);
  light(lightRight, false);
  light(lightRoom, false);
  light(lightGarage, false);
}

void on() {
  light(lightLeft, true);
  light(lightRight, true);
  light(lightRoom, true);
  light(lightGarage, true);
}

void off() {
  light(lightLeft, false);
  light(lightRight, false);
  light(lightRoom, false);
  light(lightGarage, false);
}

void blink(int delayn, int count) {
  
  for (int j=0; j <= count; j++){
    on();
    delay(delayn);
    off();
    delay(delayn);
  } 
  
}

void error() {
  blink(1000, 100);
}

void pingpong(int count) {
  
  for (int j=0; j <= count; j++){
    ping(100);
    pong(100);
  }
  
}

void demo() {
  
  blink(100, 5);

  delay(1000);

  open();
  delay(1000);
  close();
}

void open() {
  door(1);
  garage(1);
}

void close() {
  door(0);
  garage(0);
}

void ping(int delayn) {
  
  off();
  
  light(lightLeft, true);
  delay(delayn);

  light(lightRight, true);
  light(lightLeft, false);
  delay(delayn);

  light(lightRight, false);
  light(lightRoom, true);
  delay(delayn);

  light(lightRoom, false);
  light(lightGarage, true);
  delay(delayn);

  delay(delayn);
  
}

void pong(int delayn) {
  
  off();

  light(lightGarage, true);
  delay(delayn);

  light(lightGarage, false);
  light(lightRoom, true);
  delay(delayn);

  light(lightRoom, false);
  light(lightRight, true);
  delay(delayn);

  light(lightRight, false);
  light(lightLeft, true);
  delay(delayn);

  delay(delayn);
  
}

void publish() {
  
  Serial.println("publishing a message");
  
  client = PubNub.publish(channel, "\"garage:1\"");
  
  if (!client) {
    Serial.println("publishing error");
    delay(1000);
    return;
  }
  
  while (client->connected()) {
    while (client->connected() && !client->available()) ;
    char c = client->read();
    Serial.print(c);
  }
  
  client->stop();
  Serial.println();
  
}

void loop() {
  
  Serial.println("waiting for a message (subscribe)");
  PubSubClient *pclient = PubNub.subscribe(channel);
  
  if (!pclient) {
    Serial.println("subscription error");
    delay(1000);
    return;
  }
  
  String messages[10];
  boolean inside_command = false; 
  int num_commands = 0;
  
  String message = "";
  char c;
  
  while (pclient->wait_for_data()) {
    
    c = pclient->read();
    
    if(inside_command && c != '"') {
      messages[num_commands] += c;
    }

    if(c == '"') {
  
      if(inside_command) {         
        
        num_commands = num_commands + 1;
        inside_command = false;
        
      } else {     
        inside_command = true;
      }
      
    }
  
    message += c;

  }
  
  for (i = 0; i < num_commands; i++){
    
    int colonIndex = messages[i].indexOf(':');
    
    String subject = messages[i].substring(0, colonIndex);
    String valueString = messages[i].substring(colonIndex + 1, messages[i].length());
    boolean value = false;
    
    if(valueString == "1") {
      value = true;
    }

    if(subject == "garage") {
      garage(value);
    }

    if(subject == "door") {
      door(value);
    }

    if(subject == "lightLeft") {
      light(lightLeft, value);
    }

    if(subject == "lightRight") {
      light(lightRight, value);
    }

    if(subject == "lightRoom") {
      light(lightRoom, value);
    }

    if(subject == "lightGarage") {
      light(lightGarage, value);
    }
    
    if(subject == "blink") {
      blink(100, value);
    }
    
    if(subject == "pingpong") {
      pingpong(value);
    }

    Serial.println(subject);
    Serial.println(value);
    
  }
  
  Serial.print(message);
  
  pclient->stop();
  Serial.println();

}

