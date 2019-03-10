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

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
#include <WiFiUdp.h>
#include <time.h>

// #define MQTT_SERVER "192.168.0.12"
// #define MQTT_PORT 1883 // use 8883 for SSL
#define MQTT_SERVER "m13.cloudmqtt.com"
#define MQTT_PORT 16342 // use 8883 for SSL
#define MQTT_USER "jdlolrja"
#define MQTT_PASS "4NlQppdX7Qax"
#define DELIVER_WAIT 5000

// Update these with values suitable for your network.

const char *ssid = "VTR-2778044";
const char *password = "dnn6jtjpXvSq";

const char *DHT11_TOPIC = "sevengrow/307/sensor/log/dht11";
const char *MQTT_TOPIC = "sevengrow/307/sensor/log/mqtt";
const char *CO2_TOPIC = "sevengrow/307/sensor/co2";
const char *HUMIDITY_TOPIC = "sevengrow/307/sensor/humidity";
const char *POWER_TOPIC = "sevengrow/307/sensor/power";
const char *TEMPERATURE_TOPIC = "sevengrow/307/sensor/temperature";

int timezone = 0;
int dst = 0;

// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);
WiFiUDP udp;
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1')
    {
        digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
                                        // but actually the LED is on; this is because
                                        // it is active low on the ESP-01)
    }
    else
    {
        digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
    }
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        // if (client.connect(clientId.c_str()), MQTT_USER, MQTT_PASS)
        if (client.connect("ESP8266Client", MQTT_USER, MQTT_PASS))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            // client.publish(MQTT_TOPIC, "MQTT reconnect");
            // ... and resubscribe
            // client.subscribe("inTopic");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
