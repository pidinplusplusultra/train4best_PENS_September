#include <WiFi.h>
#include <HTTPClient.h>

#include <Arduino_JSON.h>
  
const char* ssid = "ganti";
const char* password =  "ganti";
  
void setup() {
  
  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");
  
}
  
void loop() {
  
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
  
    HTTPClient http;
  
    http.begin("https://api.themotivate365.com/stoic-quote"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request
  
    if (httpCode > 0) { //Check for the returning code
  
        String payload = http.getString();
        Serial.println(httpCode);
        Serial.println(payload);

        JSONVar myObject = JSON.parse(payload);
  
        // JSON.typeof(jsonVar) can be used to get the type of the var
        if (JSON.typeof(myObject) == "undefined") {
          Serial.println("Parsing input failed!");
        } else {
          Serial.print("JSON object = ");
          Serial.println(myObject);

          // myObject.keys() can be used to get an array of all the keys in the object
          JSONVar keys = myObject.keys();

          for (int i = 0; i < keys.length(); i++) {
            JSONVar value = myObject[keys[i]];
            Serial.print(keys[i]);
            Serial.print(" = ");
            Serial.println(value);
          }
        }


      }
  
    else {
      Serial.println("Error on HTTP request");
    }
  
    http.end(); //Free the resources
  }
  
  delay(10000);
  
}
