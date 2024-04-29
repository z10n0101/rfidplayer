#include <SPI.h>
#include <MFRC522.h>                
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

SoftwareSerial mySoftwareSerial(/*rx =*/6, /*tx =*/7);

#define SS_PIN 10
#define RST_PIN 9
#define POTENTIOMETER_PIN A3

DFRobotDFPlayerMini myMP3;

MFRC522 mfrc522(SS_PIN, RST_PIN);  
uint16_t folderNumber = 1;
int lastVolume = 0;
int currentSong = 1;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  mySoftwareSerial.begin(9600);

  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  delay(4);

  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  if (!myMP3.begin(mySoftwareSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  // Set initial volume
  int initialVolume = map(analogRead(POTENTIOMETER_PIN), 0, 1023, 0, 30); // Map potentiometer value to volume range (0-30)
  myMP3.volume(initialVolume);
}
void loop() {
  
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Read RFID tag
    String rfid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      rfid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      rfid += String(mfrc522.uid.uidByte[i], HEX);
    }
    rfid.toUpperCase();
    Serial.println(rfid);
    int folderNumber = getFolderNumber(rfid);
    myMP3.playFolder(folderNumber, currentSong);
    Serial.println("Folder and song:");
    Serial.print(folderNumber);
    Serial.print(" ");
    Serial.println(currentSong);
    currentSong++;

    delay(1000);
  }

  int potValue = analogRead(POTENTIOMETER_PIN);
  int volume = map(potValue, 0, 687, 0, 30); // Map potentiometer value to volume range (0-30)
  if (volume != lastVolume) {
    myMP3.volume(volume);
    lastVolume = volume;
    Serial.println(lastVolume);
  }
  delay(500); // Delay for stability
}

uint16_t getFolderNumber(String rfid) {    
    if (rfid == "224945D5") {
      if (folderNumber != 1) {
        currentSong = 1;
      }
      if (currentSong > myMP3.readFileCountsInFolder(1)) {
        currentSong = 1;
      }
      folderNumber = 1;
    } else if (rfid == "A39316EE") {
      if (folderNumber != 2) {
        currentSong = 1;
      }
      if (currentSong > myMP3.readFileCountsInFolder(2)) {
            currentSong = 1;
      }
      folderNumber = 2;
    } else if (rfid == "83AB14EE") {
        if (folderNumber != 3) {
          currentSong = 1;
        }
      if (currentSong > myMP3.readFileCountsInFolder(3)) {
            currentSong = 1;
      }
      folderNumber = 3;
    }
    return folderNumber; 
}
