#include "WebService.h"
#include "WiFiServer.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ArduinoJson.h"
#include <LittleFS.h>
#include "Temperature.h"
#include "Profiles.h"
#include "Version.h"

StaticJsonDocument<200> jsonDocTx;
static char output[512];

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
const int ledPin = 5;

char Status = 0;

// Stores LED state
String ledState;


const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pswd";

String processor(const String& var);
String getTemperatureHTML();

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    //const uint8_t size = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<200> json;
    DeserializationError err = deserializeJson(json, data);
    if (err) {
      //Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
      return;
    }
    //serializeJsonPretty(json, Serial);
    char Type = json["TYP"];
    if(Type == 1){ //WiFi Json Array Recived 
      //Serial.println("Type WiFi");
      const char* WiFiSSID = json["WiFiSSID"];
      const char* WiFiPSWD = json["WiFiPSWD"];
      Serial.println(WiFiSSID);
      Serial.println(WiFiPSWD);
      saveConfig(String(WiFiSSID),String(WiFiPSWD));
    }
    if(Type == 2){ //Reflow Json Array Recived 
      Serial.print("Pro: "); 
      char ProfileNumber = json["Profile"];
      Serial.println(ProfileNumber); 
      jsonDocTx.clear();
      jsonDocTx["CNTTMP"] = TempRead();

      serializeJson(jsonDocTx, output, 512);

      Serial.printf("Sending: %s", output);
      if (ws.availableForWriteAll()) {
        ws.textAll(output);
        Serial.printf("WS Send Pro\r\n");
      } 
      else {
        Serial.printf("...queue is full\r\n");
      }
    }
    if(Type == 3){ //Send Reflow Profile Names 
      Serial.println("Temp"); 
      jsonDocTx.clear();
      jsonDocTx["CNTTMP"] = TempRead();

      serializeJson(jsonDocTx, output, 512);

      Serial.printf("Sending: %s", output);
      if (ws.availableForWriteAll()) {
        ws.textAll(output);
        Serial.printf("WS Send TMP\r\n");
      } 
      else {
        Serial.printf("...queue is full\r\n");
      }
    }
    if(Type == 4){ //Send Reflow Profile Names 
      Serial.println("Type Verson"); 
      jsonDocTx.clear();
      jsonDocTx["VER"] = String(VERSION);

      serializeJson(jsonDocTx, output, 512);

      Serial.printf("Sending: %s", output);
      if (ws.availableForWriteAll()) {
        ws.textAll(output);
        Serial.printf("WS Send Ver\r\n");
      } 
      else {
        Serial.printf("...queue is full\r\n");
      }
    }
    if(Type == 5){ //StartProfile
      Status = !Status;
      Serial.println("Start Profile"); 
      jsonDocTx.clear();
      jsonDocTx["RESP"] = Status;

      serializeJson(jsonDocTx, output, 512);

      Serial.printf("Sending: %s", output);
      if (ws.availableForWriteAll()) {
        ws.textAll(output);
        Serial.printf("WS Send Ver\r\n");
      } 
      else {
        Serial.printf("...queue is full\r\n");
      }
    }
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

  server.on("/profileNames", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(GetProfileNames()).c_str());
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

char GetWebpageStart(){
  return Status;
}