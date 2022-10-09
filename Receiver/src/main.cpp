#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

union u_tag {
    byte b[4];
    float fval;
} u;


// default pinout
#ifdef ESP8266
  #define DEF_RF24_CS_PIN         15      //D8
  #define DEF_RF24_CE_PIN         2       //D4
  #define DEF_RF24_IRQ_PIN        //0     //D3
#endif

#ifdef NANOPRO
  #define DEF_RF24_CS_PIN         A1
  #define DEF_RF24_CE_PIN         A0
  #define DEF_RF24_IRQ_PIN        3 //can only be 2 or 3 on a 328-Based Controller
#endif

#define NRF24_CHANNEL         100           // 0 ... 125
#define NRF24_DATA_RATE       RF24_250KBPS  // RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
boolean data_received = false;

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
    radio.flush_rx();
    data_received=false;    //reset flag
}

void interruptHandler() {
  delayMicroseconds(250);                         // wait until the transition process has settled 
  bool tx_ok, tx_fail, rx_ready;                  // declare variables for IRQ masks
  radio.whatHappened(tx_ok, tx_fail, rx_ready);   // get values for IRQ masks
  if(rx_ready){
    data_received=true;                           //set flag to TRUE so we can handle received data - do not do heavy operations within an intrrupt routine
  }
}  // interruptHandler


void setup() {
  Serial.begin(115200);

  while(!Serial) {};
  Serial.println("RX Begin");
  pinMode(DEF_RF24_IRQ_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(DEF_RF24_IRQ_PIN), interruptHandler, FALLING);

  radio.begin();
  radio.setDataRate(NRF24_DATA_RATE);
  radio.setChannel(NRF24_CHANNEL);
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.maskIRQ(1, 1, 0);    //tell the nRF Module ro raise interrupt when Data is received
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
    if(data_received) {
      getData();
    }
}