#include <JsonArray.h>
#include <JsonHashTable.h>
#include <JsonObjectBase.h>
#include <JsonParser.h>

/*

 This example code is in the public domain.

 http://arduino.cc/en/Tutorial/HttpClient

 */
#include <Bridge.h>
#include <HttpClient.h>

void setup() {
  // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);

  Serial.begin(9600);

  while (!Serial); // wait for a serial connection
}

int timetoken = 0;
void loop() {

  // Initialize the client library
  HttpClient client;

  String sub = "demo";
  String pub = "demo";
  String chan = "pubnub_iot_house";
  String message = "hello, I am Yun";

//  client.getAsynchronously("http://pubsub.pubnub.com/publish/" + pub + "/" + sub + "/0/" + chan + "/0/" + message);
  client.getAsynchronously("http://pubsub.pubnub.com/subscribe/" + pub + "/" + sub + "/" + chan + "/0/" + timetoken + "/");

  // Wait for the http request to complete

  while (!client.ready()) {
    Serial.println("request is being made");
    delay(100); // or do other stuff
  }
  
  char sub_buff[200];
  char next_char;
  String thejson;
  
  while (client.available()) {
  
    next_char = client.read();
    
    if(String(next_char) == '\0') {
      break;
    } else {
      thejson += next_char;
    } 
    
  }
   
  Serial.println(thejson);
  
  int firstParen = thejson.indexOf('(');
  int lastParen = thejson.indexOf(')');

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
  
  Serial.println("break!!!");
  Serial.println(thejson2);

  thejson2.toCharArray(sub_buff, 200);
  
  JsonParser<3> parser;
  JsonArray root = parser.parseArray(sub_buff);

  if (!root.success()) {
    Serial.println("fail");
  }

  String a = root.getString(1);
  Serial.println(a);
 
  delay(5000);

}


