#include <Adafruit_FONA.h>
#include <BT2.h>
#include <Adafruit_GPS.h>

//LEDS
int green = 10;
int red = 9;
int blue = 8;

//Vars

String incomingByte;   // for incoming serial data
unsigned long x = 0x010C;
String testString;
String testGPS;
bool startUp = false;
bool btConnected = false;
char c;
String NMEA1;
String NMEA2;
byte rx_byte = 0;

//Serial declarations
//Adafruit_FONA fona = Adafruit_FONA(&bt);
Adafruit_GPS GPS(&Serial2);
BT2 bt;
//BT2 BT(&Serial);


void setup() {
  pinMode(green,OUTPUT);
  pinMode(red,OUTPUT);
  pinMode(blue,OUTPUT);
  Serial.begin(115200);         // OBD2
  Serial1.begin(115200);       // sim card or serial monitor
  Serial2.begin(9600);      // GPS opens serial port, sets data rate to 9600 bps
  bt.begin(Serial3, 9600);    // Bluetooth
  Serial.println("Begin");
  GPS.sendCommand("$PGCMD,33,O*6D");
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  delay(1000);
}

void loop() {
  /*while (Serial.available()) {
    delay(2);
    rx_byte = Serial.read();
    if (rx_byte != 0x0A){       //if its not a newline char
      //Serial.println(rx_byte);
      Serial2.write(rx_byte);
    }
  }*/
  while (Serial.available()){
    delay(2);
    char c = Serial.read();
    testString += c;
  }
  //bt.print(testString);
  testString = "";
  if (startUp==false){
    startUpLights();
    startUp = true;
    digitalWrite(green,HIGH);
  }
  //Serial.println(bt.read());

  readGPS();
  
  while(Serial1.available()){
    delay(2);
    Serial.println(Serial1.read());
    //char c = bt.read();
    //Serial.println(c);
    //Serial.print("-");
    //testString += c;
  }
  //Serial.println("PP");
  if (testString != ""){
    //Serial.println(testString);
  }
  testString = "";
  //Serial.print("end");
  //testString = bt.readSentence(Serial3);
  //Serial.println(testString);
  if(testString.length() > 0){
    testString.trim();
    if(testString=="blue"){
      digitalWrite(blue,HIGH);
    }
    else if(testString=="nblue"){
      digitalWrite(blue,LOW);
    }
    else if(testString=="red"){
      digitalWrite(red,HIGH);
    }
    else if(testString=="nred"){
      digitalWrite(red,LOW);
    }
    else{
      Serial.println("You have entered an incorrect command");
    }
    testString = "";
  }
  // send data only when you receive data:
}

void startUpLights(){
  digitalWrite(green,HIGH);
  delay(200);
  digitalWrite(red,HIGH);
  delay(200);
  digitalWrite(blue,HIGH);
  delay(200);
  digitalWrite(blue,LOW);
  delay(200);
  digitalWrite(red,LOW);
  delay(200);
  digitalWrite(green,LOW);
  delay(200);
}

void readGPS(){
  clearGPS();
  while(!GPS.newNMEAreceived()){
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  NMEA1=GPS.lastNMEA();
  
  while(!GPS.newNMEAreceived()){
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  NMEA2=GPS.lastNMEA();
  //Serial.print(NMEA1);
  //Serial.println(NMEA2);
  String lat = getValue(NMEA1,',',2);
  String latDir = getValue(NMEA1,',',3);
  String lon = getValue(NMEA1,',',4);
  String lonDir = getValue(NMEA1,',',5);
  String mapsLink = "https://www.google.ie/maps/search/" + getCoord(lat,latDir) + "," + getCoord(lon,lonDir);
  Serial.println(mapsLink);  //Printing gps link for google maps
}

void clearGPS(){
  while(!GPS.newNMEAreceived()){
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  while(!GPS.newNMEAreceived()){
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  while(!GPS.newNMEAreceived()){
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  while(!GPS.newNMEAreceived()){
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String getCoord(String number, String direct){
  String value1 = getValue(number,'.',0);
  String value2 = getValue(number,'.',1);
  int len = value1.length();
  //Serial.println(value2);
  String rem = value1.substring((len-2),(len));
  String c = value1.substring(0,(len-2));

  String x = rem + '.' + value2;
  //Serial.println(x);
  float xvalue = x.toFloat();
  //Serial.println(c);
  xvalue = xvalue/60;
  //Serial.println(xvalue,10);
  float coord = c.toFloat();
  //Serial.println(rem);
  coord = coord + xvalue;
  
  if (direct == "S" || direct == "W"){
      coord = coord * -1;
  }
  String complete = String(coord,10);
  complete = removeZeros(complete);
  return complete;
}

String removeZeros(String s){
  int i = s.length();
  while (s.substring(i-1) == "0") {
    s = s.substring(0,i-1);
    i = s.length();
  }
  return s;
}
