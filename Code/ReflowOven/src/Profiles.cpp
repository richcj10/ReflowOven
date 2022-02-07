#include "Profiles.h"
#include "ReflowControl.h"
#include <LittleFS.h>
#include "ArduinoJson.h"
#include "Define.h"

String WiFiSSID;
String WifiPassword;

char WiFiConfig = 1;
char ReflowProfile = 0;
char ProfileNames[MAXPROFILECOUNT][NAMELENTH]; 

bool loadWiFiConfig();

void ProfileSetup(){
  // Initialize SPIFFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  listDir("/");

  saveConfig("Lights.Camera.Action", "RR58fa!8");
  //strcpy(ProfileNames[0], "New");
  if(loadWiFiConfig()){
      Serial.println("Wifi Config Found");
  }
  else{
      WiFiConfig = 0;
  }
  LoadProfile();
}


void SaveProfile(String Name, float PreheatTemp, float PreheatRamp, float PreheatDwel, float FlowTemp, float FlowDwel, float FlowRamp, float CoolRamp, float CoolOff){

}

void LoadProfile(){
  Serial.print(F("Open Profile "));
  File file = LittleFS.open("/Profiles.json", "r");
  if (!file){
    Serial.print(F("Open file failed: "));
    return;
  }

  StaticJsonDocument<2000> doc;

  DeserializationError error = deserializeJson(doc, file);

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  int Count = doc["profilecount"];
  //Serial.print("Profile Count = ");
  //Serial.println(Count);
  for (JsonObject module : doc["profile"].as<JsonArray>())
  {
    const char* Name = module["profileName"];//.as<String>();
    strcpy(ProfileNames[ReflowProfile], Name);
    //Serial.println(Name);
    ReflowProfile += 1;
  }
  char value = 0;
  for(char i = 0;i<ReflowProfile;i++){
    for(char k = 0;k<NAMELENTH;k++){
      value = ProfileNames[i][k];
      if(value != NULL){
        Serial.print(value);
      }
      else{
        break;
      }
    }
    Serial.println();
  }
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
    //Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    //Serial.println("Config file size is too large");
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    //Serial.println("Failed to parse config file");
    return false;
  }

  const char* strA = doc["SSID"];
  std::string WiFiSSID = strA;
  const char* strB = doc["PSWD"];
  std::string WifiPassword = strB;

  //Serial.print("Loaded WiFi: ");
  //Serial.println(WiFiSSID.c_str());
  //Serial.print("Loaded pswd: ");
  //Serial.println(WifiPassword.c_str());
  return true;
}

bool saveConfig(String SSID, String Password){
  StaticJsonDocument<200> doc;
  doc["SSID"] = SSID;
  doc["PSWD"] = Password;

  File configFile = LittleFS.open("/WiFi.json", "w");
  if (!configFile) {
    //Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}

void LoadProfileData(char ProfileNumber){
  int Value = 0;
  Serial.print(F("Open Profile "));
  File file = LittleFS.open("/Profiles.json", "r");
  if (!file){
    //Serial.print(F("Open file failed: "));
    return;
  }

  StaticJsonDocument<2000> doc;

  DeserializationError error = deserializeJson(doc, file);

  if (error)
  {
    //Serial.print(F("deserializeJson() failed: "));
    //Serial.println(error.f_str());
    return;
  }
  //Serial.print("Profile = ");
  //Serial.println(int(ProfileNumber));
  JsonArray array = doc["profile"].as<JsonArray>();
  Value = int(array[ProfileNumber]["PreheatTemp"]);
  SetProfileValue(PREHEATTMP,Value);
  Value = int(array[ProfileNumber]["PreheatRamp"]);
  SetProfileValue(PREHEATRMP,Value);
  Value = int(array[ProfileNumber]["PreheatDwel"]);
  SetProfileValue(PREHEATDWL,Value);
  Value = int(array[ProfileNumber]["FlowTemp"]);
  SetProfileValue(FLWTMP,Value);
  Value = int(array[ProfileNumber]["FlowDwel"]);
  SetProfileValue(FLWDWL,Value);
  Value = int(array[ProfileNumber]["FlowRamp"]);
  SetProfileValue(FLWRMP,Value);
  Value = int(array[ProfileNumber]["CoolRamp"]);
  SetProfileValue(COOLRMP,Value);
  Value = int(array[ProfileNumber]["CoolStop"]);
  SetProfileValue(COOLSTOP,Value);
}

String GetSSID(){
    return WiFiSSID;
}

String GetSSIDPassword(){
    return WifiPassword;
}

char WifiConfigStatus(){
    return WiFiConfig;
}


String GetProfileNames(){
  char value = 0;
  String StringValue ="{\"data\":[";//48.756080,2.302038]}";
  //String StringValue = "{PName:";
  for(char i = 0;i<ReflowProfile;i++){
    for(char k = 0;k<NAMELENTH;k++){
      value = ProfileNames[i][k];
      if(value != NULL){
        StringValue += String(value);
      }
      else{
        StringValue += ",";
        break;
      }
    }
  }
  StringValue += "New]}";
  //Serial.println(StringValue);
  return StringValue;
}