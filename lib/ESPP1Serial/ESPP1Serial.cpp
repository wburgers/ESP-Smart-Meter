#include "ESPP1Serial.h"

#define P1_SERIAL_ENABLE 12
#define P1_IDLE_TIME_MILLIS 29000
#define P1_ACTIVE_TIME_MILLIS 1000

void ESPP1Serial::begin() {
  pinMode(P1_SERIAL_ENABLE, OUTPUT);
  Serial.begin(115200);
  digitalWrite(P1_SERIAL_ENABLE, this->enabled);
  this->timeout_start = millis();
}

void ESPP1Serial::loop(bool clients_connected) {
  if(!this->enabled && millis() - this->timeout_start < P1_IDLE_TIME_MILLIS) {
    return;
  }
  if(!this->enabled) {
    if(clients_connected) {
      this->enabled = HIGH;
    }
    Serial.flush();
    digitalWrite(P1_SERIAL_ENABLE, this->enabled);
    this->timeout_start = millis();
  }
  if(this->enabled && millis() - this->timeout_start < P1_ACTIVE_TIME_MILLIS) {
    this->bytes_read = 0;
    while(Serial.available() && this->bytes_read < MAXLINELENGTH) {
      this->serial_buf[this->bytes_read] = Serial.read();
      this->bytes_read++;
    }
    return;
  }
  if(this->enabled) {
    this->enabled = LOW;
    digitalWrite(P1_SERIAL_ENABLE, this->enabled);
    this->timeout_start = millis();
  }
}

size_t ESPP1Serial::getBytesRead() {
  return this->bytes_read;
}
