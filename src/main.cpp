#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "ESPWiFiManager.h"
#include "ESPOTA.h"
#include "ESPP1Serial.h"

#define TCP_LISTEN_PORT 9999
#define MAX_CLIENTS 5
#define LED_BUILTIN_12 2

String hostName = "P1-reader";

ESPWiFiManager wifiManager;
ESPOTA espOta;
ESPP1Serial espP1Serial;

WiFiServer server(TCP_LISTEN_PORT);
WiFiClient *clients[MAX_CLIENTS] = {NULL};
bool enabled = LOW;
int i;

void setup() {
  pinMode(LED_BUILTIN_12, OUTPUT);
  wifiManager.begin(hostName);
  espP1Serial.begin();
  espOta.begin(hostName);
  server.begin();
}

int number_connected = 0;

void loop() {
  wifiManager.loop();
  espOta.loop();
  espP1Serial.loop(enabled);

  WiFiClient client = server.available();
  if(client) {
    enabled = HIGH;
    for(i=0; i<MAX_CLIENTS; ++i) {
      if(clients[i] == NULL) {
        clients[i] = new WiFiClient(client);
        break;
      }
    }
    if (i >= MAX_CLIENTS) {
      client.write("Max connections reached, closing.");
      client.stop();
    }
  }

  number_connected = 0;

  for(i=0; i<MAX_CLIENTS; ++i) {
    if(clients[i] == NULL) {
      continue;
    }

    if(clients[i]->connected()) {
      number_connected++;
      digitalWrite(LED_BUILTIN_12, LOW);
      if(espP1Serial.getBytesRead() > 0) {
        clients[i]->write((const uint8_t*)espP1Serial.serial_buf, espP1Serial.getBytesRead());
      }
      digitalWrite(LED_BUILTIN_12, HIGH);
      continue;
    }

    clients[i]->stop();
    delete clients[i];
    clients[i] = NULL;
  }

  if(number_connected == 0 && enabled) {
    enabled = LOW;
  }

  delay(5);
}
