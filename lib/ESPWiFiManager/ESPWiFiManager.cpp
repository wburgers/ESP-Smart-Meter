#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "ESPP1Dns.h"
#include "ESPWiFiManager.h"

#define TIMEOUT_MILLIS 10000
#define LED_BUILTIN_12 2
#define CONFIG_FILENAME "/WiFiConfig.json"
#define CONFIG_SSID "SSID"
#define CONFIG_PASSWORD "Password"
#define CONFIG_FIRSTTRY "FirstTry"
#define AP_SSID "P1 ESP"
#define AP_PASSWORD "P1WiFi"

void ESPWiFiManager::begin(String hostName) {
  this->hostName = hostName;

  if(!loadConfig()) {
    beginAPMode();
  } else {
    beginConnectedMode();
  }
}

bool ESPWiFiManager::loadConfig() {
  SPIFFS.begin();
  File configFile = SPIFFS.open(CONFIG_FILENAME, "r");

  if (!configFile) {
    SPIFFS.end();
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, buf.get());

  if (error) {
    return false;
  }

  String SSIDValue = jsonDocument[CONFIG_SSID];
  String PasswordValue = jsonDocument[CONFIG_PASSWORD];
  bool firstTryValue = jsonDocument[CONFIG_FIRSTTRY];

  this->SSID = SSIDValue;
  this->Password = PasswordValue;
  this->FirstTry = firstTryValue;

  return true;
}

void ESPWiFiManager::beginAPMode() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.hostname(hostName);
  WiFi.softAPConfig(ip, ip, netmask);
  bool result = WiFi.softAP(AP_SSID, AP_PASSWORD);

  if(result) {
    if (ESPP1DNS::begin(hostName)) {
      ESPP1DNS::mdns.addService("http", "tcp", 80);
    }
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", ip);
  }
  APModeStarted = true;
}

void ESPWiFiManager::beginConnectedMode() {
  if(!try_connect()) {
    if(FirstTry) {
      removeConfig();
      ESP.restart();
    }
    return;
  }

  if(FirstTry) {
    saveConfig(this->SSID, this->Password, false);
  }
}

bool ESPWiFiManager::try_connect() {
  digitalWrite(LED_BUILTIN_12, LOW);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.hostname(hostName);
  WiFi.begin(this->SSID.c_str(), this->Password.c_str());

  unsigned long timeout_start = millis();
  int result;
  while ((result = WiFi.status()) != WL_CONNECTED &&  (millis() - timeout_start < TIMEOUT_MILLIS)) {
    delay(500);
  }

  if(result != WL_CONNECTED) {
    return false;
  }

  digitalWrite(LED_BUILTIN_12, HIGH);
  return true;
}

bool ESPWiFiManager::saveConfig(String SSID, String Password, bool FirstTry) {
  SPIFFS.begin();
  File configFile = SPIFFS.open(CONFIG_FILENAME, "w");
  if(!configFile) {
    return false;
  }

  const size_t bufferSize = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument jsonDocument(bufferSize);

  jsonDocument[CONFIG_SSID] = SSID;
  jsonDocument[CONFIG_PASSWORD] = Password;
  jsonDocument[CONFIG_FIRSTTRY] = FirstTry;

  serializeJson(jsonDocument, configFile);

  return true;
}

bool ESPWiFiManager::removeConfig() {
  SPIFFS.begin();
  return SPIFFS.remove(CONFIG_FILENAME);
}

void ESPWiFiManager::loop() {
  if(WiFi.status() != WL_CONNECTED) {
    if(!try_connect()) {
      return;
    }
  }

  if (APModeStarted) {
    dnsServer.processNextRequest();
  }
}
