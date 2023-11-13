#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
#include "esp_camera.h"

#include <WiFi.h>
#include <HTTPClient.h>

#define ssid "Access-Control"
#define password "access-12345"

int * flagPtr;    // Enable Image Ca[ture URL
int * startHTTP();

HTTPClient http;

void sendHTTP(char * url){
   http.begin(url);
   int code = http.GET();
   http.end();
}

void executeOpcode(){
  char c = 0;
  while(Serial.available()){
    c = Serial.read();
    switch(c){
       case 0x41:
          *flagPtr = 1;
          break; 
             
       case 0x43:
          sendHTTP("http://192.168.4.3:8000/user1");
          break;

       case 0x44:
          sendHTTP("http://192.168.4.3:8000/user2");
          break;

       case 0x45:
          sendHTTP("http://192.168.4.3:8000/biometric");
          break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  flagPtr = startHTTP();
  Serial.printf("[+] Flag Addr (THE OTHER SIDE) : %p", flagPtr);
  Serial.println();
  pinMode(4,OUTPUT);
  digitalWrite(4, HIGH);
  delay(1000);
  digitalWrite(4, LOW);
}


void loop() {
  executeOpcode();
  delay(1);
}
