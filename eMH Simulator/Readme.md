## eMH Simulator

This little piece of code will simulate an eMH energy-meter and will send simulated data using an IR Diode.
This way you can test your Volkszaehler2NRF24 on your desk until everything works.
  
This sketch uses a binary file from the [libsml library from https://github.com/olliiiver/sml_parser](https://github.com/olliiiver/sml_parser)
                                         
I used a Teensy 3.1 Board that I had laying around but any other board should work fine, too.
                                         
Simply solder an IR TX Diode between 3.3V and your TX Pin. Depending on the Diode you are using, you should install the matching current limiting resistor.

                                         
                                         
                                         
