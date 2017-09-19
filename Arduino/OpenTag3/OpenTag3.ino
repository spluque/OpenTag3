// Copyright Loggerhead Instruments, 2017
// David Mann

// OpenTag3 is an underwater motion datalogger
// designed around the ATMEGA328p

#include <SPI.h>
#include <SdFat.h>
#include <avr/sleep.h>
#include <avr/power.h>

// Using SoftWire because Arduino Wire library does not work with KMX62 accel/mag
#define SDA_PORT PORTC
#define SDA_PIN 4
#define SCL_PORT PORTC
#define SCL_PIN 5
#define I2C_TIMEOUT 1000
#define I2C_FASTMODE 1


#include <SoftI2CMaster.h>
#include <avr/io.h>
//#include <SoftWire.h> // Wire wrapper for SoftI2C
//SoftWire Wire = SoftWire();

//
// DEV SETTINGS
//
float codeVer = 1.00;

//
// DEV SETTINGS
//

// pin assignments
#define chipSelect 10
#define LED_GRN 4
#define LED_RED A3
#define BURN 8
#define VHFPOW 9
#define BUTTON1 A2
#define SD_POW 5

// SD file system
SdFat sd;
File dataFile;

// sensor values
int accelX, accelY, accelZ;
int magX, magY, magZ;
byte fifoVal[24];

void setup() {
  Serial.begin(115200);
  Serial.println("On");
  delay(1000);

  pinMode(LED_GRN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BURN, OUTPUT);
  pinMode(VHFPOW, OUTPUT);
  pinMode(BUTTON1, INPUT);
  digitalWrite(BURN,LOW);
  digitalWrite(LED_RED,LOW);
  digitalWrite(LED_GRN,HIGH);
  digitalWrite(VHFPOW, LOW);
  digitalWrite(BURN, LOW);
  pinMode(SD_POW, OUTPUT);      
  digitalWrite(SD_POW, HIGH); 
  
  byte I2C_check = i2c_init();
  if(I2C_check == false){
    Serial.println("I2C Init Failed--SDA or SCL may not be pulled up!");
     while(1){
       digitalWrite(LED_RED, HIGH);
       delay(500);
       digitalWrite(LED_RED, LOW);
       delay(500);
     }
  }
  


//  Serial.println("Init microSD");
//  // see if the card is present and can be initialized:
//  while (!sd.begin(chipSelect, SPI_FULL_SPEED)) {
//    Serial.println("Card failed");
//    digitalWrite(LED_RED, HIGH);
//    delay(200);
//    digitalWrite(LED_RED, LOW);
//    delay(100);
//  }

  initSensors();

}

void loop() {
    if(kmx62GetFifoPoints()>16){
      Serial.print(kmx62GetFifoPoints());
      Serial.print(" ");
      kmx62FifoRead();
      Serial.println(fifoVal[1]<<8 | fifoVal[0]);
    }
}

void initSensors(){
  Serial.println(kmx62TestResponse());
  kmx62Init(1); // init with FIFO mode
  kmx62SampleRate(25);
  
  kmx62Start(0x5F);
  kmx62ClearFifo();
  
  for(int x=0; x<100; x++){
    kmx62Read();
    Serial.print("Accel/Mag ");
    Serial.print(accelX); Serial.print(" ");
    Serial.print(accelY); Serial.print(" ");
    Serial.print(accelZ); Serial.print(" ");
    Serial.print(magX); Serial.print(" ");
    Serial.print(magY); Serial.print(" ");
    Serial.println(magZ);
    Serial.print("Fifo:");
    Serial.println(kmx62GetFifoPoints());
    delay(20);
  }

  kmx62ClearFifo();
}


