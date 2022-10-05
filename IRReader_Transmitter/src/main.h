#define HW_SERIAL_BAUD 19200                //Baud-Rate für die Hardware-Serielle Schnittstelle (Debug Monitor)
#define SW_SERIAL_BAUD 9600                 //Baud-Rate f+r den IR Lesekopf
#define WAKE_WAIT_TIME 5000                 //Wir warten 5000ms auf ein Datagramm über die IT-Schnittstelle und gehen danach wieder schlafen

#define CE_PIN   9
#define CSN_PIN 10
#define NRF24_CHANNEL         100           // 0 ... 125
#define NRF24_DATA_RATE       RF24_250KBPS  // RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
#define NRF24_RETRY_DELAY     2             // Delay bewteen retries, 1..15.  Multiples of 250µs.
#define NRF24_RETRY_COUNT     2             // Number of retries, 1..15.

#define USE_SERIAL_DEBUG

void PowerSum();                                            //Callback to extract the accumulated power from the SML Data
void CurrentPower();                                        //Callback to extract the current power from the SML Data
void setPowerSaveOptions();                                 //sets all Power-Safe-Options for the Microcontroller
void sendCPUtoSleep();                                      //sends the CPU to sleep for 8 Seconds
void sleep16();                                             //sends the CPU to Sleep for 16Seconds (uses sendCPUtoSleep)
void setup();
void loop();
void sendData(double currentPower, double accumulatedPower);//send Data using the nRF24 Modem
void smlReadByte(unsigned char currentChar);                //add a byte to the SML Parser





