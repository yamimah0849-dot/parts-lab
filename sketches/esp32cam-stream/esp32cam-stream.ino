/*
  Wi-Fi Camera Stream — Parts Lab
  Board: AI-Thinker ESP32-CAM (program it with the ESP32-CAM-MB board)
  Open the IP address from the Serial Monitor (115200) on your phone. One viewer at a time.
*/
#include <WiFi.h>
#include <WebServer.h>
#include "esp_camera.h"

const char* WIFI_SSID = "YOUR_WIFI_NAME";         // CHANGE ME (2.4 GHz network)
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // CHANGE ME

// AI-Thinker ESP32-CAM camera pins
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

WebServer server(80);

const char PAGE[] = R"HTML(<!doctype html><html><head>
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Lab Cam</title>
<style>body{margin:0;background:#06070c;color:#e8f1ff;font-family:system-ui,sans-serif;text-align:center}
img{width:100%;max-width:800px}</style></head>
<body><h3>&#128247; Lab Cam</h3><img src="/stream"></body></html>)HTML";

bool startCamera() {
  camera_config_t c = {};
  c.ledc_channel = LEDC_CHANNEL_0;
  c.ledc_timer = LEDC_TIMER_0;
  c.pin_d0 = Y2_GPIO_NUM; c.pin_d1 = Y3_GPIO_NUM; c.pin_d2 = Y4_GPIO_NUM; c.pin_d3 = Y5_GPIO_NUM;
  c.pin_d4 = Y6_GPIO_NUM; c.pin_d5 = Y7_GPIO_NUM; c.pin_d6 = Y8_GPIO_NUM; c.pin_d7 = Y9_GPIO_NUM;
  c.pin_xclk = XCLK_GPIO_NUM;
  c.pin_pclk = PCLK_GPIO_NUM;
  c.pin_vsync = VSYNC_GPIO_NUM;
  c.pin_href = HREF_GPIO_NUM;
  c.pin_sccb_sda = SIOD_GPIO_NUM;
  c.pin_sccb_scl = SIOC_GPIO_NUM;
  c.pin_pwdn = PWDN_GPIO_NUM;
  c.pin_reset = RESET_GPIO_NUM;
  c.xclk_freq_hz = 20000000;
  c.pixel_format = PIXFORMAT_JPEG;
  c.frame_size = FRAMESIZE_VGA;        // 640x480
  c.jpeg_quality = 12;                 // 10 = better quality, 30 = smaller/faster
  c.fb_count = psramFound() ? 2 : 1;
  c.fb_location = psramFound() ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM;
  c.grab_mode = CAMERA_GRAB_LATEST;
  return esp_camera_init(&c) == ESP_OK;
}

// Send JPEG frames one after another: browsers show this "MJPEG" as video
void handleStream() {
  WiFiClient client = server.client();
  client.print("HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n");
  while (client.connected()) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) { Serial.println("Camera frame failed"); break; }
    client.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", (unsigned)fb->len);
    client.write(fb->buf, fb->len);
    client.print("\r\n");
    esp_camera_fb_return(fb);
  }
}

void setup() {
  Serial.begin(115200);
  if (!startCamera()) {
    Serial.println("Camera init failed: is the ribbon cable clicked in?");
    while (true) delay(1000);
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.print("\nOpen http://");
  Serial.println(WiFi.localIP());

  server.on("/", []() { server.send(200, "text/html", String(PAGE)); });
  server.on("/stream", HTTP_GET, handleStream);
  server.begin();
}

void loop() {
  server.handleClient();
}
