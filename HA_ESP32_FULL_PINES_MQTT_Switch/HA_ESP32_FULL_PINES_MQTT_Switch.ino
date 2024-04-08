/*
   MQTT Lights for Home-Assistant - NodeMCU (ESP32)
   https://home-assistant.io/components/light.mqtt/

   Libraries :
    - ESP8266 core for Arduino : https://github.com/esp8266/Arduino
    - PubSubClient : https://github.com/knolleary/pubsubclient

   Sources :
    - File > Examples > ES8266WiFi > WiFiClient
    - File > Examples > PubSubClient > mqtt_auth
    - File > Examples > PubSubClient > mqtt_esp8266

   Schematic :
    - GND - LED - Resistor 220 Ohms - D1/GPIO5
    - Module Relay 5V - D1/GPIO5

   Aurelio M. - v1.1 - 04.2024
   If you like this example, please add a star! Thank you!
   https://github.com/yeyomuri/HA_MQTT_ESP32
*/

#include <WiFi.h>
#include <PubSubClient.h>

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Credenciales WiFi
const PROGMEM char* WIFI_SSID = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM char* WIFI_PASSWORD = "XXXXXXXXXXXXXXXXXXXX";

// Credenciales MQTT
const PROGMEM char* MQTT_CLIENT_ID = "TouchSwitchSensor";
const PROGMEM char* MQTT_SERVER_IP = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM char* MQTT_PASSWORD = "XXXXXXXXXXXXXXXXXXXX";


const PROGMEM byte pinLightList[14] = {2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};  
boolean statePinList[14] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};  
// MQTT: topics
const PROGMEM char* topicList[14] = { "topico1", "topico2", "topico3", "topico4", "topico5", "topico6", "topico7", "topico8", "topico9", "topico10", "topico11", "topico12", "topico13", "topico14", "topico15", "topico16", "topico17", "topico18", "topico19" };

// payloads by default (on/off)
const char* LIGHT_ON = "ON";
const char* LIGHT_OFF = "OFF";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  
  for(int i = 0; i < sizeof(pinLightList) - 1; i++){
    // handle message topic
    if (String(topicList[i]).equals(p_topic)) {
      // test if the payload is equal to "ON" or "OFF"
      if (payload.equals(String(LIGHT_ON))) {
        if (statePinList[i] != true) {
          statePinList[i] = true;
          digitalWrite(pinLightList[i], HIGH);
        }
      } else if (payload.equals(String(LIGHT_OFF))) {
        if (statePinList[i] != false) {
          statePinList[i] = false;
          digitalWrite(pinLightList[i], LOW);
        }
      }
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      //Serial.println("INFO: connected");
      // ... and resubscribe
      for(int i = 0; i < sizeof(pinLightList) - 1; i++){
        client.subscribe(topicList[i]);
      }
    } else {
      //Serial.print("ERROR: failed, rc=");
      //Serial.print(client.state());
      //Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  int i = 0;
  // init the serial
  Serial.begin(115200);

  // init the led
  for(int i = 0; i < sizeof(pinLightList) - 1; i++){
    pinMode(pinLightList[i], OUTPUT);
  }

  // init the WiFi connection
  // Serial.println();
  // Serial.println();
  // Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  //Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    if(i == 5) ESP.restart();
    i++;
  }

  // Serial.println("");
  // Serial.println("INFO: WiFi connected");
  // Serial.print("INFO: IP address: ");
  // Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
