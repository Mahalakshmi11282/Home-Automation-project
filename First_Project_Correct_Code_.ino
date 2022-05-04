#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/kentaylor/WiFiManager
WiFiClient client;
WiFiServer server(80);

#define led D4

#define WLAN_SSID       "Muthunagu"
#define WLAN_PASS       "Siva@100"

//To access adafruit


#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
// Onboard LED I/O pin on NodeMCU board
//const int PIN_LED = 2; // D4 on NodeMCU and WeMos. Controls the onboard LED.

/*** Adafruit.io Setup *****/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "Project_First_101"                //username of adafruit
#define AIO_KEY       "aio_SiwX29fwuwxCp8Vtv1ddGGgFdQEN" // AIO key


//to connect mqtt



Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Subscribe Switch = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Switch");

void MQTT_connect();
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}

void setup() {
  // put your setup code here, to run once:
  // initialize the LED digital pin as an output.
   // put your setup code here, to run once:
  Serial.begin(112500);
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  pinMode(led, OUTPUT);
  
  //to subscribe values in adafruit via mqtt
  mqtt.subscribe(&Switch);
}
 
 
void loop() {
MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &Switch) {
      Serial.println((char *)Switch.lastread);
      String value=(char *)Switch.lastread;

      if(value=="25"){
        digitalWrite(led,HIGH);
      }
      else{
       digitalWrite(led,LOW);
      }
    }
  }
  if(! mqtt.ping()) {
    mqtt.disconnect();
  } 
 
}
