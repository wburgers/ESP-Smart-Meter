#ifndef ESP_WIFI_MANAGER_H_
#define ESP_WIFI_MANAGER_H_

#include <Arduino.h>
#include <DNSServer.h>

class ESPWiFiManager {
  private:
    String hostName = "", SSID = "", Password = "";
    bool FirstTry = false;
    bool APModeStarted = false;

    const byte DNS_PORT = 53;
    const IPAddress ip = IPAddress(192, 168, 4, 1);
    const IPAddress netmask = IPAddress(255, 255, 255, 0);

    DNSServer dnsServer;

    void beginAPMode();
    void beginConnectedMode();
    bool loadConfig();
    bool saveConfig(String SSID, String Password, bool FirstTry);
    bool removeConfig();
    bool try_connect();
  public:
    void begin(String hostName);
    void loop();
};

#endif
