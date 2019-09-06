#include <Adafruit_FONA.h>
#include <BT2.h>
#include <Adafruit_GPS.h>
#include <EEPROM.h>

#define FONA_RST 4


//Blue LED for bluetooth
//Red LED for setup mode
//Green LED for working mode

bool carStolen = false;
bool setupMode = true;
bool silentMode = false;
bool btConnected = false;

//int reset = 5;
const int greenLED = 23;
const int blueLED = 25;
const int redLED = 27;

int speedCounter = 0;

char inChar = 0;
char replybuffer[255];
char c;
char currentGPS[255];
char macList[255];
char btDevices[255];
bool disconnected = false;

//GSM Module
//Adafruit_FONA fona(&Serial1);
//HardwareSerial & fonaSerial = Serial1;
HardwareSerial *fonaSerial = &Serial1;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
//GPS Module
Adafruit_GPS GPS(&Serial2);
//BT Module
BT2 bt;

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
char fonaNotificationBuffer[64];          //for notifications from the FONA
char smsBuffer[250];


char user[20];
char pin[6];






void setup() 
{
  pinMode(greenLED,OUTPUT);
  pinMode(blueLED,OUTPUT);
  pinMode(redLED,OUTPUT);
  digitalWrite(greenLED,HIGH);
  digitalWrite(blueLED,HIGH);
  digitalWrite(redLED,HIGH);
  delay(10);
  Serial.begin(115200);
  Serial.println("Serial Ready");
  GPS.begin(9600);
  Serial.println("GPS Ready");
  Serial3.begin(115200);
  Serial.println("Bluetooth Ready"); 
  Serial.println("Start");
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println("Couldn't find FONA");
    while(1);
  }
  
  Serial.println("FONA is OK");
  Serial.println("FONA Ready");
  Serial.println("All systems are go!");
  GPS.sendCommand("$PGCMD,33,O*6D");
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);    //configuring gps NMEA sentences
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);    //Only receive RMC NMEA messages
  char md = EEPROM.read(255);
  if (md == 'f')//create user, pin and mac address from eeprom
  {
    if (digitalRead(redLED == HIGH))//turning off red led to show its no longer in setup mode.
    {
      digitalWrite(redLED,LOW);
    }
    setupMode = false;
    Serial.println("Setup mode is false");
    readUser();
    delay(100);
    readPin();
    delay(100);
    readMACaddr();
    delay(100);
  }
  Serial.println("End of setup!");
  delay(5000);
}

void loop() 
{
  if (setupMode == true) //Enter setup mode
  {
    startSetup();
    Serial.println("Setup mode");
    delay(1000);
  }

  else if(setupMode == false && btConnected == false)//Enter working mode
  {
    String strGPS = readGPS();
    strcpy(currentGPS,strGPS.c_str());
    Serial.println(currentGPS);
    Serial.println("MIddle");
    checkForSMS();      // check for a silence text
    if (carStolen == true)
    {
      if (silentMode == false)
      {
        sendLocation(user, currentGPS);//send sms
        Serial.println(user);
        Serial.println(currentGPS);
        Serial.println("Sending SMS");
        delay(60000);
      }
    }
    
    if (btConnected == false)
    {
      connectBT();           //if the correct bluetooth device connects enter into third mode which only receives texts for location
    }

    float spd = getSpeed();
    Serial.println(spd);
    if (spd > 10.00 && btConnected == false)
    {
      if (speedCounter < 1)   // to ensure it doesnt trigger before 10kph check that it is over 10kph 2 times in a row
      {
        speedCounter++;
      }
      else
      {
        carStolen = true;
        Serial.println("CAR IS STOLEN");
      }
    }
    else if (speedCounter != 0)
    {
      speedCounter = 0;
    }
    //Serial.println("END of middle");
    
  }
  else if(setupMode == false && btConnected == true)
  {
    if (digitalRead(blueLED == HIGH))
    {
      digitalWrite(blueLED,LOW);
    }
    String strGPS = readGPS();
    strcpy(currentGPS,strGPS.c_str());
    Serial.println(currentGPS);
    if (disconnected==false)
    {
      disconnectBT();
      disconnected=true;
    }
    Serial.println("END");
    checkForSMS();//wait for texts
    delay(5000);
  }
}



//------FUNCTIONS-------------------------






void startSetup()
{
  
  Serial.println("Yppp");
  //checkMode();
  Serial.println("NIB");
  if(checkMode())     //check eeprom to see if new user has occurred
  {
    delay(1000);
    readUser();
    readPin();
    
    while (Serial3.available())
    {
      Serial.write(Serial3.read());
    }
    printBTpin();
    delay(1000);
    Serial3.write("$$$");
    delay(1000);
    Serial3.write('\n');
    delay(100);
    while (Serial3.available())
    {
      Serial.write(Serial3.read());
    }
    char tmp[] = {'S','P',',',pin[0],pin[1],pin[2],pin[3],'\n','\0'};   //write bluetooth pin to bluetooth module
    Serial3.write(tmp);           //set bluetooth connection pin to equal pin
    //delay(100);
    delay(1000);
    while (Serial3.available())
    {
      Serial.write(Serial3.read());
    }
    Serial3.write("---");
    Serial3.write('\n');
    delay(100);
    while (Serial3.available())
    {
      Serial.write(Serial3.read());
    }
    printBTpin();
    Serial.println("lsfnlsi");
    //turn on blue led
    while(btConnected == false)
    {
      Serial.println("Scanning for bt");\
      Serial.print("This should be the user : ");
      Serial.println(user);
      Serial.print("This should be the pin : ");
      Serial.println(pin);
      delay(1000);
      scan(); //scan for bt devices and add them to macList separated by ',' and ended with '-'
      delay(1000);
      attemptConnect();
      delay(2000);
      while (Serial3.available())
      { // If data comes in from XBee, send it out to serial monitor
        Serial.write(Serial3.read());
      }// put your main code here, to run repeatedly:
      checkForSMS();
    }

    writeMACaddr();
    
    setupMode = false;
    EEPROM.write(255,'f');
    if (digitalRead(redLED == HIGH))
    {
      digitalWrite(redLED,LOW);
    }
  }
  Serial.println("Checking for new sms");
  checkForSetupSMS(); //check if setup initiated, if it is write user and pin to the eeprom
  //readUser();
  //readPin();
  
  
  Serial.print("This should be the user : ");
  Serial.println(user);
  Serial.print("This should be the pin : ");
  Serial.println(pin);
}

void writeMACaddr()
{
  int address = 0;
  while (EEPROM.read(address) != 'z')
  {
    address++;
  }
  address++;
  while (EEPROM.read(address) != 'z')
  {
    address++;
  }
  address++;
  int tmpInt;
  for (int l=0; l<13; l++)
  {
    EEPROM.write(address, btDevices[l]);
    address++;
    tmpInt=l;
  }
  EEPROM.write(address, btDevices[tmpInt]);
}

void writeUser(char user[])
{
  int address = 0;
  while (address < (charLen(user)-1))
  {
    EEPROM.write(address, user[address]);
    address++;
  }
  EEPROM.write(address, 'z');
}

void writePin(char p[])
{
  //Serial.println("Starting pin write");
  int address = 0;
  while (EEPROM.read(address) != 'z')
  {
    Serial.println(EEPROM.read(address));
    Serial.write(EEPROM.read(address));
    Serial.println();
    address++;
  }
  Serial.println("BREAK");
  Serial.println(EEPROM.read(address));
  Serial.write(EEPROM.read(address));
  Serial.println();
  address++;
  Serial.println(EEPROM.read(address));
  Serial.write(EEPROM.read(address));
  Serial.println();
  Serial.println("PIN WRITE START");
  Serial.println(p);
  Serial.println("this should print pin");
  for (int i=6; i<10; i++)
  {
    Serial.println(p[i]);
  }
  for (int i=6; i<10; i++)
  {
    EEPROM.write(address, p[i]);
    Serial.println(EEPROM.read(address),DEC);
    Serial.write(EEPROM.read(address));
    Serial.println();
    Serial.println(p[i]);
    
    address++;
  }
  EEPROM.write(address, 'z');
  Serial.println("FIN");
}

void readMACaddr()
{
  int address = 0;
  while (EEPROM.read(address) != 'z')
  {
    address++;
  }
  address++;
  while (EEPROM.read(address) != 'z')
  {
    address++;
  }
  address++;
  int l=0;
  char t = EEPROM.read(address);
  while (t != '-')
  {
    btDevices[l] = t;
    address++;
    l++;
    t = EEPROM.read(address);
  }
  btDevices[l] = t;
}

void readUser()
{
  //static char number[20];
  Serial.println("Reading user");
  int address = 0;
  byte inputChar = EEPROM.read(address);
  //Serial.println("all init");
  //Serial.println(inputChar);
  while (inputChar != 'z')   // make sure that this will wipe the number!!!!!!!!!!!
  {
    //Serial.println("in while");
    //Serial.print(inputChar);
    user[address] = inputChar;
    address++;
    inputChar = EEPROM.read(address);
  }
  //Serial.print("outside");
  //return number;
}

void readPin()
{
  //char pin[6];
  int address = 0;
  int i = 0;
  char inputChar = EEPROM.read(address);
  while (inputChar != 'z')
  {
    address++;
    inputChar = EEPROM.read(address);
  }
  
  address++;
  inputChar = EEPROM.read(address);
  while (inputChar != 'z')
  {
    pin[i] = inputChar;
    i++;
    address++;
    inputChar = EEPROM.read(address);
  }
  //return pin;
}

bool checkMode()
{
  //Serial.println("adding value");
  delay(10);
  byte value = EEPROM.read(0);
  delay(10);
  Serial.println(value, DEC);
  if (value == 255)    //need to change this to represent an empty character
  {
    Serial.println("This is equal to 255");
    return false;
  }
  else
  {
    Serial.println("This aint 255");
    return true;
  }
}

bool validPin(char pin[])
{
  for (int i=6; i<10; i++)    //may need to change this depending on wheter the char array has extra characters at the end
  {
    if (isDigit(pin[i]))
    {
      Serial.print("This is a valid digit : ");
      Serial.println(pin[i]);
    } 
    else
    {
      Serial.print("This is not a valid digit");
      Serial.println(pin[i]);
      return false;
    }
  }
  return true;
}


bool connectCompare(char test[], char set[]) //this only works for strings
{
  //Serial.print("this is the tester : ");
  //Serial.println(test);
  //Serial.print("this is the setup : ");
  //Serial.println(set);
  //Serial.println(charLen(set));
  //Serial.println(sizeof(set));
  for (int i=0; i<(charLen(set)-1); i++)
  {
    test[i] &= 0b11011111;  //capitalise both characters
    set[i] &= 0b11011111;
    if (test[i] != set[i])
    {
      //Serial.print("FALSE: ");
      //Serial.print(test[i]);
      //Serial.print("----");
      //Serial.println(set[i]);
      return false;
    }
    //Serial.print("TRUE: ");
    //Serial.print(test[i]);
    //Serial.print("----");
    //Serial.println(set[i]);
  }
  return true;
}

bool textCompare(char test[], char set[]) //this only works for strings
{
  //Serial.print("this is the tester : ");
  //Serial.println(test);
  //Serial.print("this is the setup : ");
  //Serial.println(set);
  //Serial.println(charLen(set));
  //Serial.println(sizeof(set));
  for (int i=0; i<(charLen(set)-1); i++)
  {
    test[i] &= 0b11011111;  //capitalise both characters
    set[i] &= 0b11011111;
    if (test[i] != set[i])
    {
      //Serial.print("FALSE: ");
      //Serial.print(test[i]);
      //Serial.print("----");
      //Serial.println(set[i]);
      return false;
    }
    //Serial.print("TRUE: ");
    //Serial.print(test[i]);
    //Serial.print("----");
    //Serial.println(set[i]);
  }
  return true;
}

int charLen(char word[])
{
  int count = 1;
  for (int addr = 0; word[addr] != '\0'; addr++)
  {
    count++;
  }
  if (count==1)
  {
    count = 0;
  }
  return count;
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

String removeZeros(String s){
  int i = s.length();
  while (s.substring(i-1) == "0") {
    s = s.substring(0,i-1);
    i = s.length();
  }
  return s;
}

void resetDevice()
{
  //Serial.println("Starting reset");
  wipeEEPROM();
  //Serial.println("Middle reset");
  rebootBT();
  resetArduino();
  //Serial.println("Ending reset");
}

void wipeEEPROM()
{
  //Serial.println("UH OH");
  int i=0;
  int v = EEPROM.read(i); //wipe eeprom and set eeprom 255 to 't'
  //Serial.println(v);
  //Serial.write(v);
  //Serial.println();
  //Serial.println("start");
  while (v != 255)
  {
    //Serial.println("middle");
    //Serial.write(v);
    //Serial.println();
    //Serial.println(v);
    EEPROM.write(i,255);
    i++;
    v = EEPROM.read(i);
  }
  //Serial.println("end");
  EEPROM.write(255,'t');
}

void resetArduino()
{
  //possibly reset fona
  //digitalWrite(reset,LOW);
  softReset();
}

void softReset(){
asm volatile ("  jmp 0");
}

//-----------SMSFUNCTIONS---------------------




void checkForSetupSMS()
{
  //char number[255];
  //char msg[255];
  char setupChar[] = {'s','e','t','u','p','\0'};
  uint16_t smslen;
  int8_t smsnum = fona.getNumSMS();
  Serial.println(smsnum);
  int8_t smsn = 1;
  Serial.println(smsn);
  while (smsn <= smsnum) 
  {
    fona.getSMSSender(smsn, replybuffer,250);
    char number[sizeof(replybuffer)];
    strncpy(number,replybuffer,sizeof(replybuffer));
    fona.readSMS(smsn, replybuffer, 250, &smslen);
    
    if (textCompare(replybuffer, setupChar)==true)     //if true then write to eeprom
    {
      if (validPin(replybuffer)==true)
      {
        Serial.println("This has been accepted");
        writeUser(number);
        writePin(replybuffer);
      }
    }
    Serial.print("This is the number : ");
    Serial.println(number);
    Serial.print("This is the message : ");
    Serial.println(replybuffer);
    fona.deleteSMS(smsn);
    Serial.println("Message deleted!");
    if (smslen == 0) 
    {
      smsnum++;
      //continue;
    }
    smsn++;
    Serial.print("This is the smsn : ");
    Serial.println(smsn);
  }
}

void checkForSMS()
{
  //String number;
  //String msg;
  uint16_t smslen;
  int8_t smsnum = fona.getNumSMS();
  int8_t smsn = 1;
  //Serial.println("Checking for SMS");
  while (smsn <= smsnum) 
  {
    fona.getSMSSender(smsn, replybuffer,250);
    char number[] = {replybuffer};
    fona.readSMS(smsn, replybuffer, 250, &smslen);
    char msg[] = {replybuffer};
    char scanNewDeviceMsg[] = {'s','c','a','n','\0'};
    //Serial.println(scanNewDeviceMsg);
    char resetMsg[] = {'r','e','s','e','t','\0'};
    //Serial.println(resetMsg);
    char finderMsg[] = {'f','i','n','d','\0'};
    //Serial.println(finderMsg);
    char silenceMsg[] = {'s','i','l','e','n','c','e','\0'};
    //Serial.println(silenceMsg);
    char unsilenceMsg[] = {'u','n','s','i','l','e','n','c','e','\0'};
    //Serial.println(unsilenceMsg);
    char helpMsg[] = {'h','e','l','p','\0'};
    //Serial.println(helpMsg);
    //Serial.println("------------");
    //Serial.println(scanNewDeviceMsg);
    //Serial.println(resetMsg);
    //Serial.println(finderMsg);
    //Serial.println(silenceMsg);
    //Serial.println(unsilenceMsg);
    //Serial.println(helpMsg);
    //Serial.println(replybuffer);
    //Serial.println("------------");
    
    if (textCompare(replybuffer, scanNewDeviceMsg))
    {
      //scan for new bluetooth device for 60 seconds
    }
    else if (textCompare(replybuffer, resetMsg))
    {
      resetDevice();// reset to factory aka wipe EEPROM and bt device and restart the device
    }
    else if (textCompare(replybuffer, finderMsg)==true)
    {
      sendLocation(user, currentGPS);//send sms//send text of current location to user
    }
    else if (textCompare(replybuffer, silenceMsg))
    {
      silentMode = true;//silence device
    }
    else if (textCompare(replybuffer, unsilenceMsg))
    {
      silentMode = false;//unsilence device aka reboot device--------should the device respond?
    }
    else if (textCompare(replybuffer, helpMsg))
    {
      fona.sendSMS(user,pin);//send pin code to the device
    }
    Serial.print("This is the number : ");
    Serial.println(number);
    Serial.print("This is the message : ");
    Serial.println(replybuffer);
    fona.deleteSMS(smsn);
    Serial.println("Message deleted!");
    if (smslen == 0) 
    {
      smsnum++;
      //continue;
    }
    smsn++;
    Serial.print("This is the smsn : ");
    Serial.println(smsn);
  }
}

void sendLocation(char number[],char location[])
{
  if (location=="https://www.google.ie/maps/search/0./0.")      //DOUBLE CHECK THIS CODE
  {
    String strGPS = readGPS();
    strcpy(location,strGPS.c_str());
  }
  fona.sendSMS(number,location);
}





//------------------BTFUNCTIONS--------------------


void connectBT()
{
  Serial3.write("$$$");
  delay(100);
  Serial3.write('\n');
  delay(10);
  while(Serial3.available())
  {
      Serial.write(Serial3.read());
  }
  int i = 0;
  Serial.println(btDevices);
  while (btDevices[i] != '-' && btConnected==false)//Serial3.write();
  {
    Serial3.write("c,");
    while (i<12)
    {
      Serial3.write(btDevices[i]);
      i++;
    }
    Serial3.write('\n');
    delay(100);
    while(Serial3.available())
    {
      Serial.write(Serial3.read());
    }
    Serial.println("TTY");
    delay(15000);
    checkBT();
    delay(100);
  }
}



void attemptConnect()
{
  Serial.println(macList);
  Serial3.write("$$$");
  delay(100);
  Serial3.write('\n');
  delay(10);
  while(Serial3.available())
  {
      Serial.write(Serial3.read());
  }
  int i=0;//0-11=addr 12=, 13=-?
  
  while(macList[i+1]!='-' && btConnected==false && macList[i]!='-')
  {
    Serial3.write("$$$");
    //delay(10000);
    Serial3.write('\n');
    delay(10);
    while(Serial3.available())
    {
      Serial.write(Serial3.read());
    }
    if (macList[i] == ',')
    {
      i++;
    }
    Serial.println("The following will should be equal to - or ,");
    Serial.println(macList[i]);
    delay(100);
    while(Serial3.available())
    {
      Serial.write(Serial3.read());
    }
    Serial.print("c,");
    Serial3.write("c,");
    int y = i;
    while (i<(y+12))
    {
      Serial.print(macList[i]);
      Serial3.write(macList[i]);
      i++;
    }
    Serial.print('\n');
    Serial3.write('\n');
    delay(100);
    while(Serial3.available())
    {
      Serial.write(Serial3.read());
    }
    Serial.println("TTY");
    delay(15000);
    checkBT();
    Serial.println("This is the answer: ");
    Serial.println(btConnected);
    
    if (btConnected==true)
    {
      i=i-12;
      int l = 0;
      while (i<12)//make a global variable with the bluetooth addr
      {
        btDevices[l] = macList[i];
        i++;
        l++;
      }
      btDevices[l] = '-';
    }
    //i++;
  }
  Serial.println("The following will should be equal to - because its the end ");
  Serial.println(macList[i+1]);
  //Serial.println(connectStatus);
}

void scan()
{
  int macLength=12;
  //Serial3.write("---");
  //Serial3.write('\n');
  delay(10);
  Serial3.write("$$$");
  delay(100);
  Serial3.write('\n');
  Serial.println("entering command mode");
  delay(10);
  
  while (Serial3.available())
  {
    Serial3.read();
  }
  delay(1);
  //Serial.println("GET NAME");
  Serial3.write("i,7");
  Serial3.write('\n');
  delay(100);
  while (Serial3.available())
  {
    Serial.write(Serial3.read());
  }
  Serial.println("Results");
  delay(13000);
  char tmpList[255];
  char tmp;
  int i = 0;
  while (Serial3.available())
  {
    tmp = Serial3.read();
    tmpList[i] = tmp;
    i++;
  }
  int j = 0;
  int mCount =0;
  while (j < i)
  {
    //Serial.println(tmpList[(j+macLength)]);
    if (tmpList[(j+macLength)] == ',')
    {
      while(macLength>0)
      {
        //Serial.println(tmpList[j]);
        macList[mCount]=tmpList[j];
        macLength--;
        mCount++;
        j++;
      }
      macList[mCount]=',';
      mCount++;
      macLength=12;
      j=j+macLength;
    }
    while(tmpList[j] != '\n' &&  j<i)
    {
      j++;
    }
    //j--;
    
    j++;
  }
  macList[mCount] = '-';
  mCount=0;
  while (macList[mCount] != '-')
  {
    Serial.print(macList[mCount]);
    mCount++;
  }
  delay(10);
  //delay(13000);
  Serial3.write("---");
  Serial3.write('\n');
  while (Serial3.available())
  {
    Serial.write(Serial3.read());
  }
}



void checkBT()
{
  while (Serial3.available())
  { 
    Serial.write(Serial3.read());
  }
  Serial3.write("$$$");//scan for bt
  delay(100);
  Serial3.write('\n');
  delay(100);
  while (Serial3.available())
  { 
    Serial.write(Serial3.read());
  }
  Serial3.write("GK");
  Serial3.write('\n');
  delay(100);
  int connectionStatus = readBTConnection();
  
  Serial3.write("---");
  Serial3.write('\n');
  if (connectionStatus == 1)
  {
    btConnected = true;//connection is present
  }
  
}

int readBTConnection()
{
  char tmp[225];
  int i =0;
  while (Serial3.available())
  {
    tmp[i] = Serial3.read();
    i++;
    //Serial.println(tmp);
  }
  Serial.print("this is tmp: ");
  Serial.println(tmp[0]);
  if(tmp[0] == '1')   // whatever byte that bluetooth is aka 1,0,0
  {
    Serial.println("THE CONNECTION IS PRESENT!");
    return 1;
  }
  else
  {
    return 0;
  }
}

void disconnectBT()
{
  Serial3.write("$$$");
  delay(100);
  Serial3.write('\n');
  delay(100);
  Serial3.write("k,");
  Serial3.write('\n');
  delay(100);
  Serial3.write("---");
  Serial3.write('\n');
  Serial.println("Disconnected");
}

void rebootBT()
{
  Serial3.write("$$$");
  delay(100);
  Serial3.write('\n');
  delay(100);
  Serial3.write("r,");
  Serial3.write('\n');
  delay(100);
  while (Serial3.available())
  {
    Serial.write(Serial3.read());
  }
  
}

void printBTpin()
{
  Serial3.write("$$$");
  delay(300);
  Serial3.write('\n');
  delay(10);
  Serial3.write("gp,");
  delay(10);
  Serial3.write('\n');
  delay(10);
  while (Serial3.available())
  {
    Serial.write(Serial3.read());
  }
  Serial3.write("---");
  delay(10);
  Serial3.write('\n');
}


//-----------------------GPSFUNCTIONS-------------------



float getSpeed()      //get the speed using the gps nmea sentence
{
  clearGPS();
  while(!GPS.newNMEAreceived()){
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  String NMEA1 = GPS.lastNMEA();
  String spd = getValue(NMEA1,',',7);
  float kph = (spd.toFloat() * 1.852);      //convert from knots to kilometres per hour

  return kph;
}

String readGPS(){
  Serial.println("Hdfffds");
  clearGPS();
  clearGPS();
  Serial.println("HUPsee");
  while(!GPS.newNMEAreceived()){
    c=GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  Serial.println("GETI");
  String NMEA1=GPS.lastNMEA();

  //Serial.print(NMEA1);
  //Serial.println(NMEA2);
  String lat = getValue(NMEA1,',',3);
  String latDir = getValue(NMEA1,',',4);
  String lon = getValue(NMEA1,',',5);
  String lonDir = getValue(NMEA1,',',6);
  String mapsLink = "https://www.google.ie/maps/search/" + getCoord(lat,latDir) + "," + getCoord(lon,lonDir);
  String spd = getValue(NMEA1,',',7);
  float kph = (spd.toFloat() * 1.852);
  //Serial.println(kph);
  //Serial.println(mapsLink);  //Printing gps link for google maps
  return mapsLink;
}

void clearGPS(){
  Serial.println("oidnfiods");
  delay(1000);
  //GPS.parse(GPS.lastNMEA());
  while(!GPS.newNMEAreceived()){
    c=GPS.read();
    //Serial.println(c);
  }
  Serial.println("Hdkjfn");
  //GPS.parse(GPS.lastNMEA());
  //while(!GPS.newNMEAreceived()){
  //  c=GPS.read();
  //}
  GPS.parse(GPS.lastNMEA());
  Serial.println("Exiting gpsclear");
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





  //Modes
  // 1.Setup
  // 2.Working
  // 3.listening


  
  //Look for bluetooth
  
    //No bluetooth found
    //Check for car movement

      //Car Moves

      //start timer, every 2 minutes issue update on location
      // until engine is cut

    //Car doesnt move loop back
    
  //Bluetooth found
  //End bluetooth but continue to track gps and wait for sms
