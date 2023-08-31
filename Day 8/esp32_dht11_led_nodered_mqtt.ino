/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#define DHT_SENSOR_PIN  5 // ESP32 pin GPIO21 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

// Update these with values suitable for your network.

const char* ssid = "ganti";
const char* password = "ganti";
const char* mqtt_server = "ganti";

char* topic_subscribe = "esp32/led";
char* topic_publish_temp = "esp32/dht11/temp";
char* topic_publish_hum = "esp32/dht11/hum";


WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char temp_char[MSG_BUFFER_SIZE];
char hum_char[MSG_BUFFER_SIZE];
int value = 0;

#define BUILTIN_LED 2

void setup_wifi() {

  delay(10);
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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String payload_str = "";
  for (int i = 0; i < length; i++) {
    payload_str += (char)payload[i];
  }
  
  Serial.println(payload_str);
  

  // Switch on the LED if an 1 was received as first character
  if (payload_str == "on") {
    digitalWrite(BUILTIN_LED, HIGH);   // Turn the LED on 
  } else if (payload_str == "off"){
    digitalWrite(BUILTIN_LED, LOW);  // Turn the LED off 
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
//      // Once connected, publish an announcement...
//      client.publish(topic_publish, "hello world");
      // ... and resubscribe
      client.subscribe(topic_subscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  dht_sensor.begin(); // initialize the DHT sensor
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    // read humidity
    float humi  = dht_sensor.readHumidity();
    // read temperature in Celsius
    float tempC = dht_sensor.readTemperature();
    // read temperature in Fahrenheit
    float tempF = dht_sensor.readTemperature(true);
    lastMsg = now;

//    ++value;
//    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
    snprintf (temp_char, MSG_BUFFER_SIZE, "%f", tempC);
    snprintf (hum_char, MSG_BUFFER_SIZE, "%f", humi);
    Serial.print("Humidity: ");
    Serial.print(hum_char);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(temp_char);
    Serial.println("°C  ~  ");
    client.publish(topic_publish_temp, temp_char);
    client.publish(topic_publish_hum, hum_char);
  }
}
