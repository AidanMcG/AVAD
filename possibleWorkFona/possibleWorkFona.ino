//#include <SoftwareSerial.h>

#define FONA_TX 10 //comms
#define FONA_RX 11 //comms

HardwareSerial & fonaSS = Serial1;
//HardwareSerial fonaSS = HardwareSerial(Serial1);
//SoftwareSerial fonaSS = SoftwareSerial(FONA_RX, FONA_TX); //initialize software serial
char inChar = 0;

void setup() {
    //pinMode(FONA_PS, INPUT);
    //pinMode(FONA_KEY,OUTPUT);   
    //digitalWrite(FONA_KEY, HIGH);
    Serial.begin(9600);
    Serial.println("Serial Ready");
    fonaSS.begin(9600);
    Serial.println("Software Serial Ready");
}

void loop() {
  checkFona();
  
}

void checkFona() {
    if (fonaSS.available()){
        inChar = fonaSS.read();
        Serial.write(inChar);
        delay(20);
    }
    if (Serial.available()>0){
        fonaSS.write(Serial.read());
    }
}
