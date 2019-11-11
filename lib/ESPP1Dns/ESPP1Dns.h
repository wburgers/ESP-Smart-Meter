#ifndef ESP_P1_DNS_H_
#define ESP_P1_DNS_H_

#include <ESP8266mDNS.h>

class ESPP1DNS {
  public:
    static MDNSResponder mdns;
    static bool begin(String hostName);
};

#endif
