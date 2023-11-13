#define ALERT_URL "http://192.168.4.1/alert"
#define SANTZR_URL "http://192.168.4.1/santizer"
#define ACCESS_URL "http://192.168.4.1/access"

#define BUFFER_MAX_LEN 8
#define MAX_SAMPLE_VALUE 1024

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>


#define AP_SSID "Access-Control"
#define AP_PASS "access-12345"

HTTPClient http;
WiFiClient client;


void initWiFi(){
  WiFi.mode(WIFI_STA);
  pinMode(D4, OUTPUT);
    
  WiFi.hostname("Mask_UP_Zimbabwe");
  WiFi.begin(AP_SSID, AP_PASS);
  
  while(WiFi.status() != WL_CONNECTED){
    digitalWrite(D4, LOW);
    delay(100);
    digitalWrite(D4, HIGH);
    delay(100);    
  }

  digitalWrite(D4, HIGH);
}

int httpGet(char * url){
  
  http.begin(client, url);  // begin HTTP GET using given URL
  int errorCode = http.GET(); // Send HTTP GET
  http.end();
  return errorCode;
}
