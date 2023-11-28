#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "xxxxxxxxxx";
const char* password = "xxxxxxxxx";
const char* mqttServer = "192.168.4.2";
const int mqttPort = 1883;
const char* mqttUser = "esp123";
const char* mqttPassword = "xxxx";
const char* subscribeTopic = "alert";

WiFiClient espClient;
PubSubClient client(espClient);
int Speaker = D2; // GPIO03
int light = D1;   // GPIO04

int frequency[56] = {
  320, 0, 320, 0, 320, 0,  // ... add your frequencies here
};
int duration[56] = {
  220, 220, 220, 220, 220,  // ... add your durations here
};

void playTone(int freq, int dur) {
  tone(Speaker, freq, dur);
  delay(dur);
  noTone(Speaker);
}

void rickroll() {
  for (int i = 0; i < 56; i++) {
    playTone(frequency[i], duration[i]);
  }
}

// This function is executed when a message is received
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();
  if (strcmp(topic, subscribeTopic) == 0) {
    Serial.println("alert");
    digitalWrite(light, LOW);
    rickroll();
    Serial.println("Lights On");
    //delay(10000);
    digitalWrite(light, HIGH);
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("esp123", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(subscribeTopic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  pinMode(light, OUTPUT);
  digitalWrite(light, HIGH);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Hotspot setup
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ckdr", "ckdr@1234");

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // Your loop code here
}
