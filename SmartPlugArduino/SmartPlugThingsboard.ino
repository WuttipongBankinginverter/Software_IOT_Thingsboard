#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "[HCI Guest]";
const char* password = "usfreedeejung";
const char* mqtt_server = "192.168.1.200";
const char* mqttUser = "1qw23eds4546WE";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);
#define LIGHT D1

String stringOne,stringTwo;

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
  String msg = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg += (char)payload[i];
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if (stringOne == msg)
  {
    digitalWrite(LIGHT, HIGH);
    Serial.print("ON");
  }
  else if (stringTwo == msg)
  {
    digitalWrite(LIGHT, LOW);
    Serial.print("OFF");
  }
  else if(msg == "1")
  {
    digitalWrite(LIGHT, LOW);
    //client.publish("Lamp : on", msg);
  }
    else if(msg == "0")
  {
    digitalWrite(LIGHT, HIGH);
    //client.publish("Lamp : off", msg);
  }
//  if ((char)payload[0] == '1') {
//    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
//    // but actually the LED is on; this is because
//    // it is active low on the ESP-01)
//  } else {
//    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
//  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "SmartPlug";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqttUser,mqttPassword)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      // ... and resubscribe
      client.subscribe("v1/devices/me/rpc/request/+");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  //client.subscribe("inTopic");
}

void setup() {
  pinMode(LIGHT, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  stringOne = String("{\"method\":\"setValue\",\"params\":true}");
  stringTwo = String("{\"method\":\"setValue\",\"params\":false}");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    
    client.publish("v1/devices/me/telemetry","{\"Status\":\"ON\"}");

  }
}
