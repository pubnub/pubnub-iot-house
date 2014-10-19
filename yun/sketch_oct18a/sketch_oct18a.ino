#include <JsonArray.h>
#include <JsonHashTable.h>
#include <JsonObjectBase.h>
#include <JsonParser.h>
#include <Bridge.h>
#include <HttpClient.h>
#include <Servo.h>

HttpClient client;

Servo frontDoor;
Servo garageDoor;

int lightLeft = 8;
int lightRight = 7;
int lightRoom = 6;
int lightGarage = 5;

void setup() {
  // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  
  client.setTimeout(10000);

  Serial.begin(9600);

  while (!Serial); // wait for a serial connection
  
  frontDoor.attach(18);
  garageDoor.attach(19); 
  
  pinMode(lightLeft, OUTPUT);
  pinMode(lightRight, OUTPUT);
  pinMode(lightRoom, OUTPUT);
  pinMode(lightGarage, OUTPUT);
  
  blink(100, 5);
  reset();

  open();
  delay(1000);
  close();
  
}

String timetoken = "0";

void loop() {
  
  Serial.println("subscribe called");

  String sub = "demo";
  String pub = "demo";
  String chan = "pubnub_iot_house";
  String message = "hello, I am Yun";

  String url = "http://pubsub.pubnub.com/subscribe/" + sub + "/" + chan + "/0/" + timetoken;
  
  char sub_buff[200];
  char next_char;
  String thejson;

  Serial.println(url);
  client.getAsynchronously(url);
  
  // Wait for the http request to complete
  while (!client.ready()) {
    
    Serial.println("request is being made");
    delay(100); // or do other stuff
  
  }
  
  while (client.available()) {
  
    next_char = client.read();
    
    Serial.print(next_char);
    
    if(String(next_char) == '\0') {
      break;
    } else {
      thejson += next_char;
    } 
    
  }
  
  Serial.println("the json is"); 
  Serial.println(thejson);
  
  int firstParen = thejson.indexOf('(');
  int lastParen = thejson.lastIndexOf(')');

  String thejson2 = "";
  
  for(int i = 0; i < thejson.length(); i++){
    if(i == lastParen) {
      Serial.println("last paren");
      break;
    }
    if(i > firstParen) {
      thejson2 += thejson[i];
    }
  }
  
  Serial.println(thejson2);

  thejson2.toCharArray(sub_buff, 200);
  
  JsonParser<32> parser;
  JsonArray root = parser.parseArray(sub_buff);

  if (!root.success()) {
    
    Serial.println("fail");
  
  } else {
    
    timetoken = root.getString(1);
  
    JsonArray messages = root.getArray(0);
    
    Serial.print("array len ");
    Serial.print(messages.getLength());
    
    Serial.println();
    
    if(messages.getLength() < 0) {
      Serial.println("no data");
    }
    
    for(int i = 0; i < messages.getLength(); i++){  
      
      JsonHashTable message = messages.getHashTable(i);
      
      if (!message.success()) {
        Serial.println("fail");    
      }
      
      String name = message.getString("name");
      String valueString = message.getString("value");
      
      Serial.println(name + ":" + valueString);
  
      boolean value = false;
      if(valueString == "1") {
        value = true;
      }
  
      if(name == "door") {
        door(value);
      }
  
      if(name == "garage") {
        garage(value);
      }
  
      if(name == "lightLeft") {
        light(lightLeft, value);
      }
  
      if(name == "lightRight") {
        light(lightRight, value);
      }
  
      if(name == "lightRoom") {
        light(lightRoom, value);
      }
  
      if(name == "lightGarage") {
        light(lightGarage, value);
      }
      
      if(name == "blink") {
        blink(100, valueString.toInt());
      }
      
      if(name == "pingpong") {
        pingpong(valueString.toInt());
      }
    
    }
    
  }

  Serial.flush();
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
