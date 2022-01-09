#include "WebService.h"
#include "WiFiServer.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include <LittleFS.h>
#include "Temperature.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
const int ledPin = 5;

// Stores LED state
String ledState;


const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pswd";

String processor(const String& var);
String getTemperatureHTML();

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        const uint8_t size = JSON_OBJECT_SIZE(1);
        StaticJsonDocument<200> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }
        serializeJsonPretty(json, Serial);

        // const char *action = json["action"];
        // if (strcmp(action, "toggle") == 0) {
        //     led.on = !led.on;
        //     notifyClients();
        // }

    }
}

void onEvent(AsyncWebSocket       *server,
             AsyncWebSocketClient *client,
             AwsEventType          type,
             void                 *arg,
             uint8_t              *data,
             size_t                len) {

    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

String getTemperatureHTML() {
  float temperature = TempRead();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float temperature = 1.8 * bme.readTemperature() + 32;
  Serial.println(temperature);
  return String(temperature);
}

void WebserviceBegin(){
  pinMode(ledPin, OUTPUT);
  initWebSocket();
  // Route to set GPIO to HIGH
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      Serial.println("connection");
      request->send(LittleFS, "/index.html", String(), false, processor);
  });
// Route for root / web page
  server.serveStatic("/", LittleFS, "/");

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(TempRead()).c_str());
  });

  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.print("got unhandled request for ");
    Serial.println(request->url());
    request->send(404);
  });

  server.begin();
}

  // Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  else if (var == "TEMPERATURE"){
    return getTemperatureHTML();
  }
  else{
    return "NA";
  }
}