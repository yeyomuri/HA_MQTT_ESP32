/*********
  Aurelio Muñoz
  Touch Sensor Pins Controller with MQTT
  
  IMPORTANT!!! 
   - Select Board "ESP32 DEV MODULE"
   - Use Touch Sensor Pins: 4, 12, 13, 14, 15, 27, 32, 33

*********/

#include <WiFi.h>
#include <PubSubClient.h>

// Credenciales WiFi
const PROGMEM char* WIFI_SSID = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM char* WIFI_PASSWORD = "XXXXXXXXXXXXXXXXXXXX";

// Credenciales MQTT
const PROGMEM char* MQTT_CLIENT_ID = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM char* MQTT_SERVER_IP = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "XXXXXXXXXXXXXXXXXXXX";
const PROGMEM char* MQTT_PASSWORD = "XXXXXXXXXXXXXXXXXXXX";


// payload por defecto
const PROGMEM char* MOTION_ON = "ON";
const PROGMEM char* MOTION_OFF = "OFF";

// Variables de estado       
byte buttonState;             
byte lastButtonState = LOW;   
byte estadoContacto = LOW;

//Variables para la gestión del tiempo
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;

//Arrays de pines de Touch y topicos
const PROGMEM byte pinTouch[7] = {4, 12, 13, 14, 15, 27, 32}; 
const PROGMEM char* topico[7] = {"touch1", "touch2", "touch3", "touch4", "touch5", "touch6", "touch7"}; 
boolean estadoToogle[7] = {0, 0, 0, 0, 0, 0, 0};
byte i = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

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
   
  for(i = 0; i < sizeof(pinTouch); i++){ //Reemplazar por while
    if (touchRead(pinTouch[i]) < 40){
      estadoContacto = HIGH;
      break; //Captura posicion de pinTouch en la variable i
    }
    else{
      estadoContacto = LOW;
    }
  }
  if (estadoContacto != lastButtonState) {
    // restablece el temporizador de eliminación de rebotes
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Estado actual del touch
    if (estadoContacto != buttonState) {
      buttonState = estadoContacto;

      // Condición encargada del toogle, solo ocurre si buttonState=HIGH
      if (buttonState == HIGH) {
        estadoToogle[i] = !estadoToogle[i];
        if(estadoToogle[i]){
           Serial.println("INFO: Encendido");
           client.publish(topico[i], MOTION_OFF, true);   
         }
         else{
            client.publish(topico[i], MOTION_ON, true);
            Serial.println("INFO: Apagado");
                
          }
      }
    }
  }
  // Estado final del sensor touch
  lastButtonState = estadoContacto;
}


