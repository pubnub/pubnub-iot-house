#include <SPI.h>
#include <EthernetV2_0.h>
#include <PubNub.h>
#include <string.h>
#include <Servo.h>

Servo frontDoor;
Servo garageDoor;

// Some Ethernet shields have a MAC address printed on a sticker on the shield;
// fill in that address here, or choose your own at random:
byte mac[] = {0x68, 0x5b, 0x35A, 0xCF, 0xA6, 0x4E};

int lightLeft =8;
int lightRight = 7;
int lightRoom = 6;
int lightGarage = 5;

int i;

EthernetClient *client;

#define W5200_CS  3
#define SDCARD_CS 4

char pubkey[] = "demo";
char subkey[] = "demo";
char channel[] = "pubnub_iot_house";

void setup()
{
  
  pinMode(SDCARD_CS,OUTPUT);
  digitalWrite(SDCARD_CS,HIGH);//Deselect the SD card
  
  /*  
	Serial.begin(9600);
	Serial.println("Serial set up");

	while (!Ethernet.begin(mac)) {
		Serial.println("Ethernet setup error");
                blink(1000, 999999);
		delay(1000);
	}
	Serial.println("Ethernet set up");

	PubNub.begin(pubkey, subkey);
	Serial.println("PubNub set up");
  */

  frontDoor.attach(18);
  garageDoor.attach(19); 
  
  pinMode(lightLeft, OUTPUT);
  pinMode(lightRight, OUTPUT);
  pinMode(lightRoom, OUTPUT);
  pinMode(lightGarage, OUTPUT);
  
  blink(100, 999);
  reset();

  open();
  delay(1000);
  close();
  
}

void flash(int ledPin)
{
	/* Flash LED three times. */
	for (int i = 0; i < 3; i++) {
		digitalWrite(ledPin, HIGH);
		delay(100);
		digitalWrite(ledPin, LOW);
		delay(100);
	}
}

void loop()
{
  
  
  
	Ethernet.maintain();

  	PubSubClient *client;
	
	Serial.println("waiting for a message (subscribe)");
	client = PubNub.subscribe(channel);

	if (!client) {
		Serial.println("subscription error");
		return;
	}

  String messages[10];

  boolean inside_command = false; 
  int num_commands = 0;
  
  String message = "";
  char c;
  
  while (client->wait_for_data()) {
    
    c = client->read();
    
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
  client->stop();
  
  for (i = 0; i < num_commands; i++){
    
    int colonIndex = messages[i].indexOf(':');
    
    String subject = messages[i].substring(0, colonIndex);
    String valueString = messages[i].substring(colonIndex + 1, messages[i].length());
    boolean value = false;
    
    if(valueString == "1") {
      value = true;
    }

    if(subject == "door") {
      door(value);
    }

    if(subject == "garage") {
      garage(value);
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
      blink(100, valueString.toInt());
    }
    
    if(subject == "pingpong") {
      pingpong(valueString.toInt());
    }

    Serial.println(subject);
    Serial.println(value);
    
  }
  
  Serial.print(message);
  
  Serial.println();

  delay(2000);

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
     garageDoor.write(30);
  } else {
     garageDoor.write(80);
  }
  
}

void door(boolean open){
  
  Serial.println("door");
  
  if(open) {
     frontDoor.write(80);
  } else {
     frontDoor.write(20);
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

