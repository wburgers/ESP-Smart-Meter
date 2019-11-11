#include "ESPP1Dns.h"

MDNSResponder ESPP1DNS::mdns;

bool ESPP1DNS::begin(String hostName) {
  return ESPP1DNS::mdns.begin(hostName);
}
