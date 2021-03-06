
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network
const char* ssid = "CVR_HOME";
const char* password = "kalilinux";

const char* mqtt_server = "broker.hivemq.com";
const char* topicPub = "shiva";
const char* topicSub = "shakthi";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;// time last message was published
char msg[50]; // holds message that was published
int value = 0; // keeps track of number of publishes
char incoming = '0'; // holds incoming character

void setup() {
pinMode(D8, OUTPUT);
Serial.begin(115200);
// Initialize the BUILTIN_LED pin as an output
setup_wifi();
client.setServer(mqtt_server, 1883);
client.setCallback(callback);
}

void setup_wifi() {
delay(10);
// We start by connecting to a WiFi network

WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);

}

}

void callback(char* topic, byte* payload, unsigned int length) {
/*Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}
Serial.println();
*/
incoming = payload[0];
// Switch on the LED if an 1 was received as first character
if ((char)payload[0] == '0') {
digitalWrite(D8, LOW); // Turn the LED on (Note that LOW is the voltage level
// but actually the LED is on; this is because
// it is acive low on the ESP-01)
} else {digitalWrite(D8, HIGH); // Turn the LED off by making the voltage HIGH
}
}

void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
//Serial.print("Attempting MQTT connection...");
// Attempt to connect
if (client.connect("ESP8266Client")) {
Serial.println("connected");
// Once connected, publish an announcement...
//client.publish(topicPub, "low");
// ... and resubscribe
client.subscribe(topicSub);
} else {
//Serial.print("failed, rc=");
//Serial.print(client.state());
//Serial.println(" try again in 5 seconds");
// Wait 5 seconds before retrying

delay(1000);
}
}
}

void loop() {
if (!client.connected()) {
reconnect(); //establish connection to MQTT
}
client.loop();
long now = millis();
if (now - lastMsg > 2000) { // 2 seconds have elapsed
lastMsg = now;
++value;
// publish low if incoming character is 0 and high if incoming character is 1
snprintf (msg, 75, incoming == '0'?"low":"high"); // compose message
Serial.print("Publish message: ");
Serial.println(msg);
client.publish(topicPub, msg); // publish message
}
}
