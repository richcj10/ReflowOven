#define USERLED 19
#define SCOPE 25
#define FOURM 10

#define DSPLYRSET 5
void setup() {
  // initialize serial communication at 9600 bits per second:
  pinMode(DSPLYRSET, OUTPUT);
  digitalWrite(DSPLYRSET, LOW);
  Serial.begin(115200);
  delay(100);
  pinMode(DSPLYRSET, INPUT);
  delay(3000);
  WriteObject(FOURM, 4, 1);
  genieWriteStr(3, "1.2.8");
  genieWriteStr(4, "Hello!");
  delay(3000);
  WriteObject(FOURM, 0, 1);
}

int Temp = 0;
// the loop routine runs over and over again forever:
void loop() {
  delay(200);
  Temp = Temp +10;
  if(Temp > 100){
    Temp = 0;
  }
  WriteObject(USERLED, 0, 1);
  WriteObject(SCOPE, 0, Temp);
  delay(200);
  WriteObject(USERLED, 0, 0);
  genieWriteStr(0, "10");
}


void WriteObject(uint16_t object, uint16_t index, uint16_t data) {
    uint16_t msb, lsb;
    uint8_t checksum;
    lsb = lowByte(data);
    msb = highByte(data);
    Serial.write(1);
    checksum  = 1;
    Serial.write(object);
    checksum ^= object;
    Serial.write(index);
    checksum ^= index;
    Serial.write(msb);
    checksum ^= msb;
    Serial.write(lsb);
    checksum ^= lsb;
    Serial.write(checksum);
}

char genieWriteStr(uint16_t index, char *string) {
    char* p;
    uint8_t checksum;
    int len = strlen(string);
    if (len > 255)
        return -1;
    Serial.write(2);
    checksum = 2;
    Serial.write(index);
    checksum ^= index;
    Serial.write((unsigned char)len);
    checksum ^= len;
    for (p = string; *p; ++p) {
        Serial.write(*p);
        checksum ^= *p;
    }
    Serial.write(checksum);
}

char CheckScreenFB(){
  char SreenData[20];
  char Lenth;
  if(Serial.available() >= 6){
    Lenth = Serial.available();
    for(char k=0;k<Lenth;k++){
      SreenData[k] = Serial.read();
    }
  }
  else{
    return -1;
  }
}
