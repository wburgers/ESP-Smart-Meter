#include <ArduinoOTA.h>

#include "ESPOTA.h"

void ESPOTA::begin(String hostName) {
  ArduinoOTA.setHostname(hostName.c_str());
  ArduinoOTA.onStart([]() {});
  ArduinoOTA.onEnd([]() {});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
  ArduinoOTA.onError([](ota_error_t error) {});
  ArduinoOTA.begin();
}

void ESPOTA::loop() {
  ArduinoOTA.handle();
}
