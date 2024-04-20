/*
   MQTT Lights for Home-Assistant - NodeMCU (ESP32)

   Libraries :
    - ESP8266 core for Arduino : https://github.com/esp8266/Arduino
    - PubSubClient : https://github.com/knolleary/pubsubclient

   Sources :
    - File > Examples > ES8266WiFi > WiFiClient
    - File > Examples > PubSubClient > mqtt_auth
    - File > Examples > PubSubClient > mqtt_esp8266


   Aurelio M. - v1.1 - 04.2024
   If you like this example, please add a star! Thank you!
   https://github.com/yeyomuri/HA_MQTT_ESP32
*/
#include <WiFi.h>
#include <PubSubClient.h>

// Credenciales WiFi
const PROGMEM char* WIFI_SSID = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM char* WIFI_PASSWORD = "XXXXXXXXXXXXXXXXXXXX";

// Credenciales MQTT
const PROGMEM char* MQTT_CLIENT_ID = "TouchSwitchSensor";
const PROGMEM char* MQTT_SERVER_IP = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM char* MQTT_PASSWORD = "XXXXXXXXXXXXXXXXXXXX";

// MQTT: topic
const PROGMEM char* MQTT_MOTION_STATUS_TOPIC = "office/motion/status";

// payload por defecto
const PROGMEM char* MOTION_ON = "ON";
const PROGMEM char* MOTION_OFF = "OFF";

uint8_t estadoTouch = LOW; 

// PIR : D1/GPIO5
const PROGMEM uint8_t PIR_PIN = 4;
uint8_t m_pir_state = LOW; // no motion detected
uint8_t m_pir_value = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
// function called to publish the state of the pir sensor
void publishPirSensorState() {
  if (m_pir_state) {
    client.publish(MQTT_MOTION_STATUS_TOPIC, MOTION_OFF, true);
  } else {
    client.publish(MQTT_MOTION_STATUS_TOPIC, MOTION_ON, true);
  }
}

void reconnect() {
  // Función que se reconecta al broker MQTT
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Intento de conectarse
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  int i = 0;
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(i == 5) ESP.restart();
    i++;
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.println("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
}

//Se asegura de que la esp32 este conectada al broker
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    // read the PIR sensor
  m_pir_value = digitalRead(PIR_PIN);
  if (m_pir_value == HIGH) {
    if (m_pir_state == LOW) {
      // a motion is detected
      Serial.println("INFO: Motion detected");
      publishPirSensorState();
      m_pir_state = HIGH;
    }
  } else {
    if (m_pir_state == HIGH) {
      publishPirSensorState();
      Serial.println("INFO: Motion ended");
      m_pir_state = LOW;
    }
  }
}
