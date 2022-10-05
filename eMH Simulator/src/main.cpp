#include <Arduino.h>
#include "../lib/ehz_bin.h"


// set this to the hardware serial port you wish to use
#define HWSERIAL Serial1






void setup() {
  //pinMode(1, OUTPUT);
  //pinMode(1, HIGH);
  Serial.begin(115200);
  HWSERIAL.begin(9600, SERIAL_8N1	);

  Serial.println("Simulator begin...");
  delay(1000);
}

void loop() {
  Serial.println("Sending Binary Data.");
  for (unsigned int i = 0; i < ehz_bin_len; ++i) {
     //HWSERIAL.print(ehz_bin[i], DE);
     HWSERIAL.write(ehz_bin[i]);
     //if(ehz_bin[i]<0x10)Serial.print('0');
     //Serial.print(ehz_bin[i], HEX);
     //Serial.print(' ');
  }
  Serial.println("Done.\nWaiting 5 seconds.\n\n");
  delay(5000);
}