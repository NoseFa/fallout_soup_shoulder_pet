#include <Arduino.h>
#include <AsyncTCP.h>
#include <WiFi.h>

#include <ESPAsyncWebServer.h>


// idk find a better way to laod the html file
// https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
static const char *htmlContent PROGMEM = R"(

)";

static const size_t htmlContentLength = strlen_P(htmlContent);

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

void setup() {
  Serial.begin(115200);

  // setup wifi connection for local webpanel
  #if ASYNCWEBSERVER_WIFI_SUPPORTED
    WiFi.mode(WIFI_AP);
    WiFi.softAP("esp-captive");
  #endif

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", (uint8_t *)htmlContent, htmlContentLength);
  });

  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    (void)len;

    if (type == WS_EVT_CONNECT) {
      Serial.println("ws connect");
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.println("ws disconnect");
    } else if (type == WS_EVT_ERROR) {
      Serial.println("ws error");
    } else if (type == WS_EVT_DATA) {
      AwsFrameInfo *info = (AwsFrameInfo *)arg;
      Serial.printf(
        "index: %" PRIu64 ", len: %" PRIu64 ", final: %" PRIu8 ", opcode: %" PRIu8 ", framelen: %d\n", info->index, info->len, info->final,
        info->message_opcode, len
      );

      //todo handling messages
    }
  });


  server.addHandler(&ws);
  server.begin();
}

void loop() {
  delay(100);
}
