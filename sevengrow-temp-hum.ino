#include "header.h"

void setup()
{
    pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
    Serial.begin(115200);
    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);

    Serial.setDebugOutput(true);
    configTime(timezone * 3600, 0, "clock.wenu.cl", "time.nist.gov");
    Serial.println("\nWaiting for time");
    while (!time(nullptr))
    {
        Serial.print(".");
        delay(1000);
    }
}

void loop()
{
    byte temperature = 0;
    byte humidity = 0;
    int err = SimpleDHTErrSuccess;

    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    // read without samples.
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
    {
        Serial.print("Read DHT11 failed, err=");
        Serial.println(err);
        char dht11_err_message[40];
        sprintf(dht11_err_message, "Read DHT11 failed, err=%s", err);
        // client.publish(DHT11_TOPIC, dht11_err_message);
        delay(1000);
        return;
    }

    char temp[40];
    char hum[40];

    time_t now = time(nullptr);
    sprintf(temp, "%3i,%d", (int)temperature, now);
    sprintf(hum, "%3i,%d", (int)humidity, now);

    client.publish(TEMPERATURE_TOPIC, temp);
    client.publish(HUMIDITY_TOPIC, hum);

    Serial.println(now);
    delay(DELIVER_WAIT);
}