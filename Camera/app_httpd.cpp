#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "Arduino.h"

#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"

#define ENROLL_CONFIRM_TIMES 5

int accessModeEnabled = 0;

typedef struct {
        size_t size; //number of values used for filtering
        size_t index; //current value index
        size_t count; //value count
        int sum;
        int * values; //array to be filled with values
} ra_filter_t;

typedef struct {
        httpd_req_t *req;
        size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static ra_filter_t ra_filter;
httpd_handle_t control_httpd = NULL;
httpd_config_t config;
httpd_uri_t capture_uri, alert_uri, sanitizer_alert_uri;
httpd_uri_t get_access_uri, access_uri, rfid_access_uri, fps_access_uri, program_mode_uri;

static mtmn_config_t mtmn_config = {0};
int tempAlert = 0;
int sanitizerAlert = 0;
int gotImageFlag = 0;
char buff[12];

static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len){
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if(!index){
        j->len = 0;
    }
    if(httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK){
        return 0;
    }
    j->len += len;
    return len;
}

static esp_err_t capture_handler(httpd_req_t *req){
  
    digitalWrite(4,HIGH);
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start = esp_timer_get_time();

    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        httpd_resp_send_500(req);
        digitalWrite(4,LOW);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    size_t out_len, out_width, out_height;
    uint8_t * out_buf;
    bool s;
    if(fb->width > 400){
        size_t fb_len = 0;
        if(fb->format == PIXFORMAT_JPEG){
            fb_len = fb->len;
            res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
        } else {
            jpg_chunking_t jchunk = {req, 0};
            res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk)?ESP_OK:ESP_FAIL;
            httpd_resp_send_chunk(req, NULL, 0);
            fb_len = jchunk.len;
        }
        esp_camera_fb_return(fb);
        int64_t fr_end = esp_timer_get_time();
        Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start)/1000));
        digitalWrite(4,LOW);                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
        return res;
    }

    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    if (!image_matrix) {
        esp_camera_fb_return(fb);
        Serial.println("dl_matrix3du_alloc failed");
        httpd_resp_send_500(req);
        digitalWrite(4,LOW);
        return ESP_FAIL;
    }

    out_buf = image_matrix->item;
    out_len = fb->width * fb->height * 3;
    out_width = fb->width;
    out_height = fb->height;

    s = fmt2rgb888(fb->buf, fb->len, fb->format, out_buf);
    esp_camera_fb_return(fb);
    if(!s){
        dl_matrix3du_free(image_matrix);
        Serial.println("to rgb888 failed");
        httpd_resp_send_500(req);
        digitalWrite(4,LOW);
        return ESP_FAIL;
    }

    box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);

    jpg_chunking_t jchunk = {req, 0};
    s = fmt2jpg_cb(out_buf, out_len, out_width, out_height, PIXFORMAT_RGB888, 90, jpg_encode_stream, &jchunk);
    dl_matrix3du_free(image_matrix);
    if(!s){
        Serial.println("JPEG compression failed");
        digitalWrite(4,LOW);
        return ESP_FAIL;
    }

    int64_t fr_end = esp_timer_get_time();
    digitalWrite(4,LOW);
    gotImageFlag = 0;
    return res;
}

static esp_err_t get_image_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  String(gotImageFlag).toCharArray(buff, 12);
  return httpd_resp_send(req, buff, strlen(buff));
}

static esp_err_t alert_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  tempAlert = (tempAlert == 0) ? 1 : 0; 
  String(tempAlert).toCharArray(buff, 12);
  return httpd_resp_send(req, buff, strlen(buff));
}

static esp_err_t get_alert_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  String(tempAlert).toCharArray(buff, 12);
  return httpd_resp_send(req, buff, strlen(buff));
}

static esp_err_t sanitizer_alert_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  sanitizerAlert = 1; 
  String(sanitizerAlert).toCharArray(buff, 12);
  return httpd_resp_send(req, buff, strlen(buff));
}

static esp_err_t sanitizer_off_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  sanitizerAlert = 0; 
  String(sanitizerAlert).toCharArray(buff, 12);
  return httpd_resp_send(req, buff, strlen(buff));
}

static esp_err_t get_access_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  String(accessModeEnabled).toCharArray(buff, 2);
  accessModeEnabled = 0;
  return httpd_resp_send(req, buff, strlen(buff));
}

static esp_err_t access_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  accessModeEnabled = 1;
  return httpd_resp_send(req, "OK", 2);
}

static esp_err_t fps_access_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  Serial.write(0x42); // Send FPS Access Mode OPCode
  return httpd_resp_send(req, "OK", 2);
}

static esp_err_t rfid_access_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  Serial.write(0x44); // Send RFID Access Mode OPCode
  return httpd_resp_send(req, "OK", 2);
}

static esp_err_t program_handler(httpd_req_t * req){
  httpd_resp_set_type(req, "text/html");
  Serial.write(0x43); // Send Program Mode OPCode
  return httpd_resp_send(req, "OK", 2);
}

int * startHTTP(){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    
  capture_uri = {
        .uri       = "/capture",
        .method    = HTTP_GET,
        .handler   = capture_handler,
        .user_ctx  = NULL
    };
 
  alert_uri = {
    .uri = "/alert",
    .method = HTTP_GET,
    .handler = alert_handler
  };

  sanitizer_alert_uri = {
    .uri = "/sanitizer",
    .method = HTTP_GET,
    .handler = sanitizer_alert_handler
  };

  fps_access_uri = {
    .uri = "/fps_access",
    .method = HTTP_GET,
    .handler = fps_access_handler
  };

  rfid_access_uri = {
    .uri = "/rfid_access",
    .method = HTTP_GET,
    .handler = rfid_access_handler
  };

  access_uri = {
    .uri = "/access",
    .method = HTTP_GET,
    .handler = access_handler
  };

  get_access_uri = {
    .uri = "/get_access",
    .method = HTTP_GET,
    .handler = get_access_handler
  };

  program_mode_uri = {
      .uri = "/program",
      .method = HTTP_GET,
      .handler = program_handler
  };
    
    Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&control_httpd, &config) == ESP_OK) {
      
        httpd_register_uri_handler(control_httpd, &capture_uri);
        httpd_register_uri_handler(control_httpd, &alert_uri);
        httpd_register_uri_handler(control_httpd, &sanitizer_alert_uri);
        httpd_register_uri_handler(control_httpd, &fps_access_uri);
        httpd_register_uri_handler(control_httpd, &rfid_access_uri);
        httpd_register_uri_handler(control_httpd, &access_uri);
        httpd_register_uri_handler(control_httpd, &get_access_uri);
        httpd_register_uri_handler(control_httpd, &program_mode_uri); 
        
      Serial.printf("[-] FLag Addr : %p", &gotImageFlag);
      Serial.println();
      return &gotImageFlag;
    }

}
