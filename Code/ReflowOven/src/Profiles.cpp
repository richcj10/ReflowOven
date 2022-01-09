#include "Profiles.h"
#include <LittleFS.h>
#include "ArduinoJson.h"

String WiFiSSID;
String WifiPassword;

char Config = 1;

bool loadWiFiConfig();

void ProfileSetup(){
  // Initialize SPIFFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  listDir("/");

  saveConfig("Lights.Camera.Action", "RR58fa!8");

  if(loadWiFiConfig()){
      Serial.println("Wifi Config Found");
  }
  else{
      Config = 0;
  }
  ///writeFile("/hello.txt", "Hello ");
  //listDir("/");
}

// var PreheatTemp = 0;
// var PreheatRamp = 0;
// var PreheatDwel = 0;
// var FlowTemp = 0;
// var FlowDwel = 0;
// var FlowRamp = 0;
// var CoolRamp = 0;

void SaveProfile(String Name, float PreheatTemp, float PreheatRamp, float PreheatDwel, float FlowTemp, float FlowDwel, float FlowRamp, float CoolRamp){

}

void writeFile(const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  delay(2000); // Make sure the CREATE and LASTWRITE times are different
  file.close();
}

void listDir(const char * dirname) {
  Serial.printf("Listing directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    Serial.print("  FILE: ");
    Serial.print(root.fileName());
    Serial.print("  SIZE: ");
    Serial.println(file.size());
    //time_t cr = file.getCreationTime();
    //time_t lw = file.getLastWrite();
    file.close();
    //struct tm * tmstruct = localtime(&cr);
    //Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    //tmstruct = localtime(&lw);
    //Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
}

bool loadWiFiConfig() {
  File configFile = LittleFS.open("/WiFi.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* strA = doc["SSID"];
  std::string WiFiSSID = strA;
  const char* strB = doc["PSWD"];
  std::string WifiPassword = strB;

  Serial.print("Loaded WiFi: ");
  Serial.println(WiFiSSID.c_str());
  Serial.print("Loaded pswd: ");
  Serial.println(WifiPassword.c_str());
  return true;
}

bool saveConfig(String SSID, String Password){
  StaticJsonDocument<200> doc;
  doc["SSID"] = SSID;
  doc["PSWD"] = Password;

  File configFile = LittleFS.open("/WiFi.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}

String GetSSID(){
    return WiFiSSID;
}

String GetSSIDPassword(){
    return WifiPassword;
}

char WifiConfigStatus(){
    return Config;
}