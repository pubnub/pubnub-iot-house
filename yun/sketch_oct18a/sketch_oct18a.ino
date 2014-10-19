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

String timetoken = "0";
void loop() {

  // Initialize the client library
  HttpClient client;

  String sub = "demo";
  String pub = "demo";
  String chan = "pubnub_iot_house";
  String message = "hello, I am Yun";

  String url = "http://pubsub.pubnub.com/subscribe/" + sub + "/" + chan + "/0/" + timetoken;

  Serial.println(url);
  client.getAsynchronously(url);

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
    
    Serial.print(next_char);
    
    if(String(next_char) == '\0') {
      break;
    } else {
      thejson += next_char;
    } 
    
  }
   
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
  
  Serial.println("break!!!");
  Serial.println(thejson2);

  thejson2.toCharArray(sub_buff, 200);
  
  JsonParser<64> parser;
  JsonArray root = parser.parseArray(sub_buff);

  if (!root.success()) {
    Serial.println("fail");
  }

  timetoken = root.getString(1);
  Serial.println(timetoken);
  
  delay(5000);
 
}


