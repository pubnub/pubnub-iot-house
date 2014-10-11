#include <SPI.h>
#include <EthernetV2_0.h>
#include <PubNub.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(173,194,33,104); // Google


const int subLedPin = 8;
const int pubLedPin = 9;

char pubkey[] = "demo";
char subkey[] = "demo";
char channel[] = "pubnub_iot_house";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient *client;

#define W5200_CS  10
#define SDCARD_CS 4

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);

  pinMode(SDCARD_CS,OUTPUT);
  digitalWrite(SDCARD_CS,HIGH);//Deselect the SD card
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);

  Serial.println("Ethernet set up");

  PubNub.begin(pubkey, subkey);
  Serial.println("PubNub set up");
  
}

void flash(int ledPin) {

  /* Flash LED three times. */
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}

void loop() {
  
  Serial.println("publishing a message");
  client = PubNub.publish(channel, "\"\\\"Hello world!\\\" she said.\"");
  
  if (!client) {
    Serial.println("publishing error");
    delay(1000);
    return;
  }
  
  while (client->connected()) {
    while (client->connected() && !client->available()) ; // wait
    char c = client->read();
    Serial.print(c);
  }
  
  client->stop();
  Serial.println();
  flash(pubLedPin);

  Serial.println("waiting for a message (subscribe)");
  PubSubClient *pclient = PubNub.subscribe(channel);
  
  if (!pclient) {
    Serial.println("subscription error");
    delay(1000);
    return;
  }
  
  while (pclient->wait_for_data()) {
    char c = pclient->read();
    
    char *json_String=aJson.print(aJsonObject);

    Serial.print(aJsonObject);
  }
  
  pclient->stop();
  Serial.println();
  flash(subLedPin);

  /*
  Serial.println("retrieving message history");
  client = PubNub.history(channel);
  
  if (!client) {
    Serial.println("history error");
    delay(1000);
    return;
  }
  */
  
  while (client->connected()) {
    while (client->connected() && !client->available()) ; // wait
    char c = client->read();
    Serial.print(c);
  }
  
  client->stop();
  Serial.println();

  delay(10000);

}

