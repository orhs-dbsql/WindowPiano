#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
#include "SPI.h"
#include <Wire.h>
#include "Adafruit_MPR121.h"

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap1 = Adafruit_MPR121();
Adafruit_MPR121 cap2 = Adafruit_MPR121();
TMRpcm tmrpcm;

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;
uint16_t lasttouched2 = 0;
uint16_t currtouched2 = 0;
void setup() {
  tmrpcm.speakerPin = 9;
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  
  Serial.println("Adafruit MPR121 cap1acitive Touch sensor test"); 

  if (!cap1.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }else{
    Serial.println("Found 1");
  }
  if (!cap2.begin(0x5B)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  }

  tmrpcm.setVolume(6);
  
}
int base[24];
boolean flag = false;
void loop() {
  // Get the currently touched pads
  currtouched = cap1.touched();
  if(!flag){
    for(int a=0; a<1000; a++){
      for(int j=0; j<12; j++){
        base[j] = (base[j]+cap1.filteredData(j))/2;
        base[j+12] = (base[j+12]+cap2.filteredData(j))/2;
      }
    }
    flag=true;
  }
  
  lasttouched = currtouched;
  currtouched2 = cap2.touched();

  lasttouched2 = currtouched2;
  int temp[24];
  for (uint8_t i2=0; i2<12; i2++) {
      temp[i2]=cap1.filteredData(i2);
  }  
  for (uint8_t i2=0; i2<12; i2++) {
      temp[i2+12]=cap2.filteredData(i2);
  }
  for(int i=0; i<24; i++)
  {
    if(base[i]-temp[i]>15){
      Serial.println(i);
      tmrpcm.play(String(i)+”.wav");

    }
  }
}
