/*
  Adapted from Example from ThingSpeak Library (Mathworks)
  
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-thingspeak-publish-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include "ThingSpeak.h"

const char* ssid = "ganti";   // your network SSID (name) 
const char* password = "ganti";   // your network password

WiFiClient  client;

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "ganti";

unsigned long timerDelay = 15000;

// Variable to hold temperature readings
int randomNum1;
int randomNum2;

int field1 = 1;
int field2 = 2;


void setup() {
  Serial.begin(115200);  //Initialize serial

    // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

    // print a random number from 0 to 299
    randomNum1 = random(0,100);
    Serial.print("randomNum1 = ");
    Serial.println(randomNum1);
    
    randomNum2 = random(200,300);
    Serial.print("randomNum2 = ");
    Serial.println(randomNum2);
    
    //uncomment if you want to get temperature in Fahrenheit
    /*temperatureF = 1.8 * bme.readTemperature() + 32;
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureF);*/

    // set the fields with the values
    ThingSpeak.setField(1, randomNum1);
    ThingSpeak.setField(2, randomNum2);
    
    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(x == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    delay(timerDelay);

}
