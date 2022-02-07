#define USERLED 19
#define SCOPE 25
#define FOURM 10
#define DSPLYRSET 5


char ScreenPage = 0;
char previousState = 0;
char Led = 0;
char Led2 = 0;
char ButtonPressed = 0;
char ScreenAlive = 0;
char ReflowOvenMode = 0;

//WriteObject(FOURM, 4, 1); //Show fourm 4
void setup() {
  // initialize serial communication at 9600 bits per second:
  pinMode(13, OUTPUT);
  pinMode(DSPLYRSET, OUTPUT);
  digitalWrite(DSPLYRSET, LOW);
  Serial.begin(115200);
  delay(100);
  pinMode(DSPLYRSET, INPUT);
  delay(3000);
}

int Temp = 0;
// the loop routine runs over and over again forever:
void loop() {
  CheckScreenFB();
  ScreenDataupdate();
  delay(100);
}

void ScreenDataupdate(){
  switch (ScreenPage) {
    case 0:    // your hand is on the sensor
      if(previousState != ScreenPage){  //Does it only need to run it once in this state, put the code here
        previousState = ScreenPage;
      }
      Led = !Led;
      WriteObject(USERLED, 0, Led);
      genieWriteStr(0, "10");
      if(ReflowOvenMode){
        digitalWrite(13,HIGH);
      }
      else{
        digitalWrite(13,LOW);
      }
      break;
    case 1:    // your hand is close to the sensor
      break;
    case 2:    // your hand is a few inches from the sensor
      if(previousState != ScreenPage){  //Does it only need to run it once in this state, put the code here
        previousState = ScreenPage;
        genieWriteStr(1, "192.168.5.1/rLights.Camera.Action");
      }
      if(ButtonPressed == 5){
        //WiFi AP Mode
        ButtonPressed = 0;
        
      }
      break;
    case 3:    // your hand is nowhere near the sensor
      if(previousState != ScreenPage){  //Does it only need to run it once in this state, put the code here
        previousState = ScreenPage;
        genieWriteStr(2, "Reflow Profile");
      }
      break;
    case 4:    // your hand is nowhere near the sensor
      if(previousState != ScreenPage){  //Does it only need to run it once in this state, put the code here
        previousState = ScreenPage;
        genieWriteStr(3, "1.2.3");
        genieWriteStr(4, "Custom");
      }
      break;
  }
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
  char dataIn;
  if(Serial.available() >= 2){
    Lenth = Serial.available();
    for(char k=0;k<Lenth;k++){
      SreenData[0] = Serial.read();
      if(SreenData[0] == 6){
        ScreenAlive = 1;
      }
      if(SreenData[0] == 7){
        SreenData[1] = Serial.read();
        if(SreenData[1] == 10){ //Fourm Comand
          ScreenPage = Serial.read();
        }
        else if(SreenData[1] == 6){ //Btn Comand
          ButtonPressed = Serial.read();
        }
        else if(SreenData[1] == 30){ //Spcl Button Command
          Serial.read();
          Serial.read();
          ReflowOvenMode = Serial.read();
        }
      }
    }
  }
}
