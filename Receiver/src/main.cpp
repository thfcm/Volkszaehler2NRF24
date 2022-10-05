#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

union u_tag {
    byte b[4];
    float fval;
} u;


// default pinout
#define DEF_RF24_CS_PIN         15//15    //D8
#define DEF_RF24_CE_PIN         2//2     //D4
#define DEF_RF24_IRQ_PIN        //0     //D3
#define DEF_RF24_MOSI_PIN       //13    //D7
#define DEF_RF24_MISO_PIN       //12    //D6
#define DEF_RF24_SCK_PIN        //14    //D5

#define NRF24_CHANNEL         100           // 0 ... 125
#define NRF24_DATA_RATE       RF24_250KBPS  // RF24_2MBPS, RF24_1MBPS, RF24_250KBPS


RF24 radio(DEF_RF24_CE_PIN, DEF_RF24_CS_PIN); // Create a Radio

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
char dataReceived[8]; // this must match dataToSend in the TX

//==============

void getData() {
  float currentPower = 0;
  float powerReceived = 0;
    if ( radio.available() ) {
      while(radio.available()) {
          radio.read(&dataReceived,sizeof(dataReceived));
          for(int i = 0 ; i< 4; i++){
            u.b[i] = dataReceived[i];
          }
          currentPower=u.fval;

          for(int i = 0 ; i<4; i++){
            u.b[i] = dataReceived[i+4];
          }
          powerReceived=u.fval;

          Serial.print("Z:"); Serial.println(powerReceived/1000);
          Serial.print("P:"); Serial.println(currentPower);
      }
    }
}


void setup() {
  Serial.begin(115200);
  //delay(1000);
  while(!Serial) {};
  Serial.println("RX Begin...");
  radio.begin();
  radio.setDataRate(NRF24_DATA_RATE);
  radio.setChannel(NRF24_CHANNEL);
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();
  /*Serial.println("RX Init Done");

  Serial.print("RX Address:");
  for(int i = 0 ; i<sizeof(thisSlaveAddress); i++) {
    if(thisSlaveAddress[i]<0x10) Serial.print('0');
    Serial.print(thisSlaveAddress[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  Serial.print("CRC Length:");
  Serial.println(radio.getCRCLength(),DEC);*/
}

void loop() {
    if(radio.available()) {
        getData();
        delay(100);
    }

    //Serial.println("test");
    //delay(1000);
}