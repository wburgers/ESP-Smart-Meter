#ifndef ESP_OTA_H_
#define ESP_OTA_H_

#include <Arduino.h>

class ESPOTA {
  public:
    void begin(String hostName);
    void loop();
};

#endif
