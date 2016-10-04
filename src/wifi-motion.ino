#include "Secrets.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MOTION_PIN 13
// milliseconds
#define RETRIGGER_DELAY 15000

WiFiClient wifi;
PubSubClient client(wifi);

long last_motion = 0;

void setup() {
  pinMode(0, OUTPUT);
  pinMode(MOTION_PIN, INPUT);

  Serial.begin(115200);

  Serial.println("Connecting...");
  WiFi.begin(ap_name, ap_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("Wifi Connected");

  client.setServer(mqtt_host, mqtt_port);
}

void check_motion() {
  long now = millis();
  if (digitalRead(MOTION_PIN) && (last_motion == 0 || (now - last_motion) > RETRIGGER_DELAY)) {
    last_motion = now;
    client.publish(mqtt_topic, "");
    delay(500);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  while (!client.connected()) {
    reconnect();
  }

  client.loop();
  check_motion();
}
