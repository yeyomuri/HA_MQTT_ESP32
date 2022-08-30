#include <Stepper.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Cambié las credenciales a continuación, para que su ESP32 se conecte a su router
#define ssid "XXXXXXXXXXXXXXXXX"
#define password "XXXXXXXXX"

// Cambié las credenciales para conectarse al broker MQTT
#define mqtt_server "XXXXXXXXXXXXXX"
#define mqtt_user "XXXXXXXXXXXXXXX"
#define mqtt_password "XXXXXXXXXXXX"


WiFiClient espClient;
PubSubClient client(espClient);

//Parametros(numero de pasos por una revolucion, x, Y{primer par bobina},Z,W{segundo par de bobina})
Stepper MotorPasos1(50, 5, 18, 19, 21); 
const int recorrido_completo = 50; //Cambiar en función del largo de la cortina
float aperturaActual = 0;
float aperturaFutura = 0;

// No cambié la función a continuación. Esta función conecta su ESP32 a su red WiFi 

void setup_wifi() {
  int i=0;
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(i == 5) ESP.restart();
    i++;
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Esta función se ejecuta cuando algún dispositivo publica un mensaje sobre un tema o tópico al que está suscrito su ESP32
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
//Programacio cortina
  if(topic=="casa/habitacion/principal/cortina"){
    aperturaActual = aperturaFutura;
    aperturaFutura = messageTemp.toFloat();   
    MotorPasos1.step((aperturaFutura - aperturaActual)/100*recorrido_completo);
  }
}

// Esta función vuelve a conectar su ESP8266 a su broker MQTT 
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32LED", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      //Suscripción de tópicos, añada más si usted lo requiere
      client.subscribe("casa/habitacion/principal/cortina");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//Inicializa GPIOS, MQTT broker y llama a la función callback
void setup() { 
  MotorPasos1.setSpeed(50);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

//Se asegura de que la esp32 este conectada al broker
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 
}
