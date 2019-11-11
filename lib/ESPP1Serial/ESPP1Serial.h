#ifndef ESP_P1_SERIAL_H_
#define ESP_P1_SERIAL_H_

#include <Arduino.h>

#define MAXLINELENGTH 256

class ESPP1Serial {
  private:
    size_t bytes_read;
    unsigned long timeout_start;
  public:
    bool enabled = LOW;
    uint8_t serial_buf[MAXLINELENGTH];
    void begin();
    void loop(bool clients_connected);
    size_t getBytesRead();
};

#endif
