nclude "DHT.h" // DHT11 library
#include <ESP8266WiFi.h> // Wifi  ibrary 
#include <PubSubClient.h> // MQTT protocol library 


/************************* WiFi *********************************/
const char* network = "*******"; // 
const char* pass = "*******"; 

/************** Mosquitto Mqtt  *********************************/

WiFiClient nodemcu;
PubSubClient client(nodemcu);
const char* mosquitto = "1.0.0.0";

/********************** DHT11  *****************************/
#define DHTPIN 5  
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

/******************************************************************************************/

void init_wifi() {
  delay(1000);

  WiFi.begin(network, pass);
  while (WiFi.status() != WL_CONNECTED) // Loop while wifi is not connected
  {
    delay(500);
  }
  randomSeed(micros());
}

void mqtt_connect() {  //function to connect to Mosquitto MQTT
  
  while (!client.connected())
  {

    String clientId = "nodemcu";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str()))
    {
      Serial.println("Mosquitto MQTT Connected");
    }
    else {
      Serial.print("Connection failed, rc=");
      Serial.print(client.state());
      delay(6000);
    }
  }
} 

void setup() {
  Serial.begin(115200); // Serial monitor starts
  init_wifi(); // Wifi connection
  client.setServer(mosquitto, 1883); // Mosquitto MQTT broker connection
  dht.begin();// Starts the DHT sensor
}

void loop() {
 
  float celsius = dht.readTemperature();   // temperature read

  float hum = dht.readHumidity();   // Read temperature as Celsius (the default)

  float heatindex = dht.computeHeatIndex(celsius, hum, false);   // Compute heat index in Celsius (isFahreheit = false)

  if (isnan(hum) || isnan(celsius)) {
    Serial.println("Measurement failed");
    return;
  }

    if (!client.connected()) {
    mqtt_connect();
  } 

/************************* Mosquitto MQTT publish ************************/

  client.publish("temperature", String(heatindex).c_str());
  client.publish("humidity", String(hum).c_str());

  delay(60000); //Publish the measures each 60 seconds
}
