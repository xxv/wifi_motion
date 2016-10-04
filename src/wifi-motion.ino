#include "Secrets.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define MOTION_PIN 13

ESP8266WiFiMulti wifi;

bool statusLed = HIGH;

char *post_body = "motion";

void setup() {
  pinMode(0, OUTPUT);
  pinMode(MOTION_PIN, INPUT);

  Serial.begin(115200);

  Serial.println("Connecting...");
  wifi.addAP(ap_name, ap_password);
}

void connected_loop() {
  while (! digitalRead(MOTION_PIN)) {
    delay(10);
  }

  HTTPClient http;

  http.begin(http_server, http_port, http_path); //HTTP

  int httpCode = http.POST((uint8_t *)post_body, strlen(post_body));
  if(httpCode) {
      Serial.printf("Success!: %d\n", httpCode);
  } else {
      Serial.write("Error POSTing\n");
  }

  delay(15 * 1000);
}

void loop() {
  digitalWrite(0, statusLed);
  // wait for WiFi connection
   if((wifi.run() == WL_CONNECTED)) {
      digitalWrite(0, HIGH);
        Serial.print("Connected!\n");

        while (wifi.run() == WL_CONNECTED) {
          connected_loop();
        }

        // Turn off again
        digitalWrite(0, LOW);
    }

  delay(500);

  statusLed = statusLed == LOW ? HIGH : LOW;
}
