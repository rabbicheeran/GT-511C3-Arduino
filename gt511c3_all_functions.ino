#include <SoftwareSerial.h>
SoftwareSerial gt511c3(8, 9);//rx tx

byte highbyte = 0;
byte lowbyte = 0; 
word checksum = 0;
byte highcheck = 0;
byte lowcheck = 0;
byte response = 0;
int state = 0;
word parameterin = 0;
word checksumReply = 0;

boolean communicationError = false;
boolean checksumCorrect = true; 
boolean ack = true;

byte lbyte = 0;
byte hbyte = 0;
byte checklbyte = 0;
byte checkhbyte = 0;

const int transmitDelay = 500;

int id;

void setup() {
  Serial.begin(9600);
  gt511c3.begin(9600);
  openConnection();
}

void loop(){
      int i=0;
      char commandbuffer[100];
      String serialCommand = "";
      String resp = "";

      String input;
      String para_ip;
      int para;

      Serial.println("Enter : ");
      Serial.println("1 : LED ON or OFF");
      Serial.println("2 : To get enrollment count");
      Serial.println("3 : Check whether specified ID is enrolled or not");
      Serial.println("4 : Enroll a new finger");
      Serial.println("5 : Is finger pressed");
      Serial.println("6 : Delete by ID");
      Serial.println("7 : Delete whole added fingers from database");
      Serial.println("8 : Checks the currently pressed finger against a specific ID");
      Serial.println("9 : Checks the currently pressed finger against all enrolled fingerprints");
      Serial.println(" ");

      while(Serial.available() == 0){}
      
      if(Serial.available()>0){
      input = Serial.readString();
      
          if(input == "1"){
            Serial.println("Enter LED status 1 - ON or 0 - OFF");
            while(Serial.available() == 0){}
            para_ip = Serial.readString();
            para = para_ip.toInt();
            LED(para);
          }
          
          else if(input == "2"){
            Serial.print("Enrollment count : ");
            Serial.println(getEnrollCount());
          }
          
          else if(input == "3"){
            Serial.println("Enter ID to check whether ID is enrolled or not?");
            while(Serial.available() == 0){}
            para_ip = Serial.readString();
            para = para_ip.toInt();
            if(checkEnrolled(para) == 0){
                Serial.print("ID ");
                Serial.print(para);
                Serial.println(" is enrolled");
              }
            else{
                Serial.print("ID ");
                Serial.print(para);
                Serial.println(" is not enrolled");
              }
          }
          
          else if(input == "4"){
            LED(1);
            startEnroll();
            LED(0);
          }
          
          else if(input == "5"){
            LED(1);
            if(isFingerPressed() == 0){
                Serial.println("Finger pressed");
              }
            else{
                Serial.println("Finger not  pressed");
              }
            LED(0);
          }
          
          else if(input == "6"){
            Serial.println("Enter ID to delete");
            while(Serial.available() == 0){}
            para_ip = Serial.readString();
            para = para_ip.toInt();
            if(deleteId(para) == 0){
               Serial.print("ID ");
               Serial.print(para);
               Serial.println(" deleted successfully");
              }
            else{
               Serial.print("No data found in the ID : ");
               Serial.println(para);
              }
          }
          
          else if(input == "7"){
            Serial.println("Enter 1 to delete whole database or 0 to cancel");
            while(Serial.available() == 0){}
            para_ip = Serial.readString();
            para = para_ip.toInt();
            if(para == 1){
              deleteAll();
              Serial.println("Whole database deleted");
            }
            else{
              Serial.println("Cancled");
              }
          }
          
          else if(input == "8"){
            LED(1);
            Serial.println("Enter ID to verify your pressed finger");
            while(Serial.available() == 0){}
            para_ip = Serial.readString();
            para = para_ip.toInt();
            captureFinger(1);
            if(verify(para) == 0){
                Serial.print("Same finger found in the ID : ");
                Serial.println(para);            
              }
            else{
                Serial.println("No match found");
              }
            LED(0);
          }
          
          else if(input == "9"){
            LED(1);
            captureFinger(1);
            int identify_id = identify();
            if(identify_id < 200){
                Serial.print("Finger found in the ID : ");
                Serial.println(identify_id);
              }
            else{
                Serial.println("Finger not found");
              }
            LED(0);
          }
          
          else{
            Serial.println("INVALID INPUT");
            }
          Serial.println(" ");
          delay_anim();
          Serial.println(" ");
      }
}


//-------------------------------- METHODS -----------------------------------//

// Delay animatoiin.
void delay_anim(){
    for(int z=0; z<75; z++){
      Serial.print("=");
      delay(40);
      }
  }


// Starts the Enrollment Process
// Parameter: 0-199
// Return:
//  0 - ACK
//  1 - Database is full
//  2 - Invalid Position
//  3 - Position(ID) is already used
void startEnroll() {
  Serial.println("Starting enrollment process");

  id = getEnrollCount();
  Serial.print("Enroll Count : ");
  Serial.println(id);
  
  // start the enrollment
  scannerCommand(0x22, id);
  waitForReply();
  
  Serial.println(">> press finger to begin");
  
  // delay while waiting for the finger press
  while(isFingerPressed() != 0) delay(100);

  // capture a high quality finger touch
  bool bret = captureFinger(true);
  int iret = 0;
  
  // if there is a positive response (finger detected and captured);
  if (bret != false) {
    Serial.println("Remove finger");
    enroll1();
    while(isFingerPressed() == 0);
    
    Serial.println(">> Press same finger again");
    while(isFingerPressed() != 0);
    bret = captureFinger(true);
    if (bret != false) {
      Serial.println("Remove finger");
      enroll2();
      while(isFingerPressed() == 0);
      Serial.println(">> Press same finger again");
      while(isFingerPressed() != 0);
      bret = captureFinger(true);
      if (bret != false) {
        iret = enroll3();
        Serial.println("Remove finger");
        if (iret == 0) {
          Serial.println("Enrolling Successfull");
        } 
        else {
          Serial.print("Enrolling Failed with error code : ");
          Serial.println(iret);
        }
      } else {
        Serial.println("ERROR READING FINGER THIRD TIME");
      }
    } else {
      Serial.println("ERROR READING FINGER SECOND TIME");
    }
  } else {
    Serial.println("ERROR READING FINGER FIRST TIME");
  }
  
  state = 0;
}


// Captures the currently pressed finger into onboard ram use this prior to other commands
// Parameter: true for high quality image(slower), false for low quality image (faster)
// Generally, use high quality for enrollment, and low quality for verification/identification
// Returns: True if ok, false if no finger pressed
bool captureFinger(bool highquality) {
  Serial.println("Capturing your finger print");
  if (highquality) {
    scannerCommand(0x60, 1);
  } else {
    scannerCommand(0x60, 0);
  }
  waitForReply();
  return (parameterin == 0);
}


//Open connecion
int openConnection(){
  scannerCommand(0x01, 0); 
  waitForReply();
  return parameterin;
  }

//Close connection
int closeConnection(){
  scannerCommand(0x02, 0); 
  waitForReply();
  return parameterin;
  }

//Turn LED ON and OFF  by parameter 1 and 0
int LED(int parameter) {
  if(parameter == 1){
      Serial.println("LED turned ON");
    }
  else{
      Serial.println("LED turned OFF");
    }
  scannerCommand(0x12, parameter);
   waitForReply();
   return parameterin;
}

// Get total number of enrolled fingers
int getEnrollCount() {
  scannerCommand(0x20, 0);
   waitForReply();
   return parameterin;
}

// Check whether input parameter is enrolled
int checkEnrolled(int parameter) {
  scannerCommand(0x21, parameter);
   waitForReply();
   return parameterin;
}

// Is finget pressed
// Retern 0 if finger pressed
// Retern non 0 if finger not pressed
int isFingerPressed() {
  scannerCommand(0x26, 0);
   waitForReply();
   return parameterin;
}

// Delete by ID
int deleteId(int parameter) {
  scannerCommand(0x40, parameter);
   waitForReply();
   return parameterin;
}

// Delete ALL
int deleteAll() {
  scannerCommand(0x41, 0);
   waitForReply();
   return parameterin;
}

// Checks the currently pressed finger against a specific ID
// Parameter: 0-199 (id number to be checked)
// Returns:
int verify(int parameter) {
  scannerCommand(0x50, parameter);
   waitForReply();
   return parameterin;
}

// Checks the currently pressed finger against all enrolled fingerprints
// Returns:
//  0-199: Verified against the specified ID (found, and here is the ID number)
//  200: Failed to find the fingerprint in the database
int identify() {
  scannerCommand(0x51, 0);
   waitForReply();
   return parameterin;
}

 //------------------Functions used in startEnroll()--------------------//
//---------------------------------------------------------------------//
      // Enroll Start
      int enrollStart(int parameter) {
        scannerCommand(0x22, parameter);
         waitForReply();
         return parameterin;
      }
      
      // Enroll 1
      int enroll1() {
        scannerCommand(0x23, 0);
         waitForReply();
         return parameterin;
      }
      
      // Enroll 2
      int enroll2() {
        scannerCommand(0x24, 0);
         waitForReply();
         return parameterin;
      }
      
      // Enroll 3
      int enroll3() {
        scannerCommand(0x25, 0);
         waitForReply();
         return parameterin;
      }
      
//---------------------------- END OF METHODS -----------------------------//
//------------------------------------------------------------------------//



//------------------------Functinos for GT-511C3--------------------------//
//-----------------------------------------------------------------------//
void scannerCommand(byte com, int param) { 
  valueToWORD(param);
  calcChecksum(com, highbyte, lowbyte);
  gt511c3.write(0x55);
  gt511c3.write(0xaa);
  gt511c3.write(0x01);
  gt511c3.write((byte)0);
  gt511c3.write(lowbyte);
  gt511c3.write(highbyte);
  gt511c3.write((byte)0);
  gt511c3.write((byte)0);
  gt511c3.write(com);
  gt511c3.write((byte)0);
  gt511c3.write(lowcheck);
  gt511c3.write(highcheck);
}

// helper for getting input from the serial
String getStringFromBuffer(char* bufferArray) {
  String returnString = "";
  
   for (int j = 0; j < 100; j++){
    if (bufferArray[j] == '\0') return returnString;
      returnString += bufferArray[j];
   }
   
   return returnString;
}

void calcChecksum(byte c, byte h, byte l){
  checksum = 256 + c + h + l; //adds up all the bytes sent
  highcheck = highByte(checksum); //then turns this checksum which is a word into 2 bytes
  lowcheck = lowByte(checksum);
}

void waitForReply(){ 
  communicationError = false;
  while(gt511c3.available() == 0){}
  delay(transmitDelay);
  if(gt511c3.read() == 0x55){
  } else {
    communicationError = true;
  }

  if(gt511c3.read() == 0xAA){
  } else {
    communicationError = true;
  }

  if(gt511c3.read() == 0x01){
  } else {
    communicationError = true;
  }

  if(gt511c3.read() == 0x00){
  } else {
    communicationError = true;
  }

  lbyte = gt511c3.read();
  hbyte = gt511c3.read();

  parameterin = word(hbyte, lbyte);

  gt511c3.read();
  gt511c3.read();

  response = gt511c3.read();

  if(response == 0x30){
    ack = true;
  } else {
    ack = false;
  }
  gt511c3.read();

  checklbyte = gt511c3.read();
  checkhbyte = gt511c3.read();

  checksumReply = word(checkhbyte, checklbyte);

  if(checksumReply == 256 + lbyte + hbyte + response){
    checksumCorrect = true;
  } else {
    checksumCorrect = false;
  }
} 

//turns the word you put into it (the paramter in the code above) to two bytes
void valueToWORD(int v){ 
  highbyte = highByte(v); //the high byte is the first byte in the word
  lowbyte = lowByte(v); //the low byte is the last byte in the word (there are only 2 in a word)
}

//----------------------------------END-----------------------------------//
//-----------------------------------------------------------------------//
