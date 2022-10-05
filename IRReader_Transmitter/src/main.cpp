#include <Arduino.h>
#include <main.h>
#include <AltSoftSerial.h>
#include "sml.h"
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>





RF24 radio(CE_PIN, CSN_PIN);                // Create a Radio
AltSoftSerial mySerial;                     // Create a SoftwareSerial Instance
sml_states_t currentState;

unsigned long wake_up_timestamp           = 0;
unsigned int iHandler                     = 0;
#ifdef USE_SERIAL_DEBUG 
const char msgTextBegin[] PROGMEM         = "Begin ";
#endif
double CPwr = -2, SumWh = -2;
byte dataToSend[10]                       = {0x0A,0xFF,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0xFF} ;   //Buffer will hold 2x double (4Byte) and 2x 1Byte additional info. Increase, if double uses more Bytes
const byte slaveAddress[5]                = {'R','x','A','A','A'};


/* ####################################################################################### *
 *
 * 
 * ####################################################################################### */


typedef struct {
  const unsigned char OBIS[6];
  void (*Handler)();
} OBISHandler;



void PowerSum() {
  smlOBISWh(SumWh);
}


void CurrentPower() {
  smlOBISW(CPwr);
}


// clang-format off
OBISHandler OBISHandlers[] = {
  {{ 0x01, 0x00, /*0x0f*/ 0x10, 0x07, 0x00, 0xff }, &CurrentPower},      //  1-0:1.8.0*255  Bezug Total
  {{ 0x01, 0x00, 0x01, 0x08, 0x00, 0xff }, &PowerSum},                  //  1-0:15.7.0*255 Aktueller Bezug
  {{ 0, 0 }}
};
// clang-format on


/**
 * @brief Set the Power Save Options object
 * 
 */
void setPowerSaveOptions() {
  MCUCR = bit (BODS) | bit (BODSE);         // turn on brown-out enable select
  MCUCR = bit (BODS);                       // this must be done within 4 clock cycles of above
  ADCSRA = 0;                               // disable ADC
  power_adc_disable();                      // disable ADC
  power_twi_disable();                      // disable TWI (I2C)
  power_timer2_disable();
  power_timer2_disable();
  //power_timer1_disable();                 //AltSoftSerial nutzt Timer1
}


// watchdog interrupt
ISR (WDT_vect) {
   wdt_disable();  // disable watchdog
}  // end of WDT_vect


/**
 * @brief Send the CPU to a Sleep state for 8 seconds
 * 
 */
void sendCPUtoSleep() {
    // clear various "reset" flags
    MCUSR = 0;     
    // allow changes, disable reset
    WDTCSR = bit (WDCE) | bit (WDE);
    // set interrupt mode and an interval 
    WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
    wdt_reset();  // pat the dog
  
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
    noInterrupts ();           // timed sequence follows
    sleep_enable();

    // turn off brown-out enable in software
    MCUCR = bit (BODS) | bit (BODSE);
    MCUCR = bit (BODS); 
    interrupts ();             // guarantees next instruction executed
    sleep_cpu ();  
  
    // cancel sleep as a precaution
    sleep_disable();
}


void sleep16(){
    #ifdef USE_SERIAL_DEBUG 
      Serial.println("sleeping");
    #endif
    delay(100);
    sendCPUtoSleep();         //Send CPU to sleep for 8 Seconds
    delay(100);               //needs some delay inbetween or it won't sleep a second time
    sendCPUtoSleep();         //Send CPU to sleep for 8 Seconds
    #ifdef USE_SERIAL_DEBUG 
      Serial.println("\n\nwoke up");
    #endif
    setPowerSaveOptions();
    wake_up_timestamp=millis();   
}


void setup() {
  if(F_CPU == 8000000) clock_prescale_set(clock_div_2);
  if(F_CPU == 4000000) clock_prescale_set(clock_div_4);


  setPowerSaveOptions();

#ifdef USE_SERIAL_DEBUG 
  Serial.begin(HW_SERIAL_BAUD);
  Serial.print((__FlashStringHelper*)msgTextBegin);
  Serial.print(F_CPU/1000000);
  Serial.println("MHz");
#endif
  pinMode(5, INPUT_PULLUP);           //ToDo: Entferne mich nach dem Umbau

  radio.begin();
  radio.setDataRate(NRF24_DATA_RATE);
  radio.setChannel(NRF24_CHANNEL);

  radio.setRetries(NRF24_RETRY_DELAY, NRF24_RETRY_COUNT); // delay, count
  radio.openWritingPipe(slaveAddress);

  

  //Software-Serial 1 : lesen aus dem IR-Sensor
  mySerial.begin(SW_SERIAL_BAUD);
  mySerial.listen();

  bool rslt;
  rslt = radio.write( &dataToSend, sizeof(dataToSend) );     //Send Dummy Data on Startup (mostly for debug purposes but also useful to count restarts)
  radio.powerDown(); 
#ifdef USE_SERIAL_DEBUG
  Serial.print("Sending Dummy ...");
  if(rslt)Serial.println("success");
  else Serial.println("failed");
#endif

  
}


/**
 * @brief Sendet die Daten aus dem Buffer
 * Hierzu wird das nRF Modul kurz eingeschaltet und dann wieder in den Standby geschickt
 */
void sendData(double currentPower, double accumulatedPower) {
  radio.powerUp();
  bool rslt;
  int pos = 0;
  byte * valuePtr = (byte *) &currentPower;                 //we find the address of the first byte
  for(unsigned int i=0; i<sizeof(double); i++){
    dataToSend[pos]=*(valuePtr + i);                      //copy data byte-wise to the array starting at 0
    pos++;
  }

  valuePtr = (byte *) &accumulatedPower;                    //we find the address of the first byte
  for(unsigned int i=0;i<sizeof(double); i++){
    dataToSend[pos]=*(valuePtr + i);                      //copy data byte-wise to the array starting at the end of the data copied above
    pos++;
  }

  //dataToSend[pos++] = 0x80;                                 //Dummy Data for later use (e.g. battery status)
  //dataToSend[pos] = 0xFF;                                 //Dummy Data for later use (e.g. system status)


  rslt = radio.write( &dataToSend, sizeof(dataToSend) );
      // Always use sizeof() as it gives the size as the number of bytes.
      // For example if dataToSend was an int sizeof() would correctly return 2

#ifdef USE_SERIAL_DEBUG 
  Serial.print("Data Send: ( ");
  for(unsigned int i=0;i<sizeof(dataToSend);i++) {
    if((byte)dataToSend[i]<0x10) Serial.print('0');
    Serial.print((byte)dataToSend[i], HEX);
    Serial.print(' ');
  }
  Serial.println(")");
  if (rslt) {
      Serial.println("Ack received");
  } else {
      Serial.println("TXS failed");
  }
#endif
  radio.powerDown();
}

/**
 * @brief put a Byte into the SML library
 * 
 * @param currentChar 
 */
void smlReadByte(unsigned char currentChar) {
  currentState = smlState(currentChar);
  if (currentState == SML_START) {
    /* reset local vars */
    CPwr = -3;
    SumWh = -3;
  }

  if (currentState == SML_LISTEND) {
        /* check handlers on last received list */
        for (iHandler = 0; OBISHandlers[iHandler].Handler != 0 &&
                           !(smlOBISCheck(OBISHandlers[iHandler].OBIS));
             iHandler++)
          ;
        if (OBISHandlers[iHandler].Handler != 0) {
          OBISHandlers[iHandler].Handler();
        }
  }


  if (currentState == SML_FINAL) {
        sendData(CPwr, SumWh);
        #ifdef USE_SERIAL_DEBUG 
          Serial.print(F("\n\n"));
          Serial.print(F("CP: "));
          Serial.println(CPwr, DEC);
          Serial.print(F("ZS: "));
          Serial.println(SumWh, DEC);
          Serial.println("S2");
        #endif
        delay(10);
        sleep16();
      }
}


/**
 * @brief Main Loop
 * 
 */
void loop() {
  if(mySerial.available()>0) {
    while(mySerial.available()>0) {
      //byte b = mySerial.read();
      //if(b<0x10)Serial.print('0');
      //Serial.print(b,HEX);
      //Serial.print(' ');
      //smlReadByte(b);
      smlReadByte(mySerial.read());
    }
  }


  /*
  Wurde kein Paket empfangen und/oder ist die Wartezeit abgelaufen, dann schicke die CPU in den Sleep-Mode.
  Der Sleep-Mode wird auch aufgerufen, wenn das Paket vollständig empfangen und ausgewertet wurde.
  */
  if(millis()>wake_up_timestamp+WAKE_WAIT_TIME) {
    #ifdef USE_SERIAL_DEBUG 
      Serial.println("\nS1"); delay(10);
    #endif
    sleep16();
  }
}